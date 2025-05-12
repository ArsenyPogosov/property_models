#include "quick_plan.h"

#include <set>
#include <unordered_set>

namespace NPropertyModels::NSolver {

namespace {

template <typename TKey, typename TValue>
class TBidirectionalMap {
public:
	const TValue &operator[](const TKey &key) const {
		return KeyToValue_.at(key);
	}

	void Insert(const TKey &key, const TValue &value) {
		Erase(key);
		KeyToValue_[key] = value;
		ValueToKeys_[value].insert(key);
	}

	void Erase(const TKey &key) {
		auto it = KeyToValue_.find(key);
		if (it != KeyToValue_.end()) {
			const TValue &value = it->second;
			auto vit = ValueToKeys_.find(value);
			if (vit != ValueToKeys_.end()) {
				vit->second.erase(key);
				if (vit->second.empty()) {
					ValueToKeys_.erase(vit);
				}
			}
			KeyToValue_.erase(it);
		}
	}

	[[nodiscard]] bool Contains(const TKey &key) const {
		return KeyToValue_.contains(key);
	}

	[[nodiscard]] bool ContainsValue(const TValue &value) const {
		return ValueToKeys_.contains(value);
	}

	[[nodiscard]] std::unordered_set<TKey> Keys(const TValue &value) const {
		auto it = ValueToKeys_.find(value);
		if (it != ValueToKeys_.end()) {
			return it->second;
		}
		return {};
	}

private:
	std::unordered_map<TKey, TValue> KeyToValue_;
	std::unordered_map<TValue, std::unordered_set<TKey>> ValueToKeys_;
};

class TConstraintGraph {
public:
	TConstraintGraph() = default;

	explicit TConstraintGraph(const TTask &task) {
		for (size_t constraintId = 0; constraintId < task.ConstraintsCount; ++constraintId) {
			ConstraintIds_.insert(constraintId);
		}
		for (size_t propertyId = 0; propertyId < task.PropertiesCount; ++propertyId) {
			PropertyIdToDegree_.Insert(propertyId, 0);
			PropertyIdToCSMs_.insert({propertyId, {}});
		}

		std::vector<std::unordered_set<size_t>> propertyToConstraints(task.PropertiesCount);

		for (size_t csmId = 0; csmId < task.CSMs.size(); ++csmId) {
			const auto &csm = task.CSMs[csmId];
			CSMIdToConstraintId_.Insert(csmId, csm.ConstraintId);
			CSMIdToOutputDegree_.Insert(csmId, csm.OutputPropertieIds.size());
			CSMIdToInputPropertyIds_[csmId] = {};
			CSMIdToOutputPropertyIds_[csmId] = {};

			for (size_t propertyId : csm.InputPropertieIds) {
				propertyToConstraints[propertyId].insert(csm.ConstraintId);
				PropertyIdToCSMs_[propertyId].insert(csmId);
				CSMIdToInputPropertyIds_[csmId].insert(propertyId);
			}
			for (size_t propertyId : csm.OutputPropertieIds) {
				propertyToConstraints[propertyId].insert(csm.ConstraintId);
				PropertyIdToCSMs_[propertyId].insert(csmId);
				CSMIdToOutputPropertyIds_[csmId].insert(propertyId);
			}
		}

		for (size_t propertyId = 0; propertyId < task.PropertiesCount; ++propertyId) {
			PropertyIdToDegree_.Insert(propertyId, propertyToConstraints[propertyId].size());
		}
	}

	void RemoveProperty(size_t propertyId) {
		if (!PropertyIdToDegree_.Contains(propertyId)) {
			throw std::runtime_error("Property was already removed");
		}
		PropertyIdToDegree_.Erase(propertyId);

		for (size_t csmId : PropertyIdToCSMs_[propertyId]) {
			CSMIdToInputPropertyIds_[csmId].erase(propertyId);
			if (CSMIdToOutputPropertyIds_[csmId].erase(propertyId)) {
				CSMIdToOutputDegree_.Insert(csmId, CSMIdToOutputDegree_[csmId] - 1);
			}
		}
		PropertyIdToCSMs_.erase(propertyId);
	}

	void RemoveConstraint(size_t constraintId) {
		if (!ConstraintIds_.contains(constraintId)) {
			throw std::runtime_error("Constraint was already removed");
		}

		ConstraintIds_.erase(constraintId);

		std::unordered_set<size_t> removedPropertyLinks;
		auto csmIds = CSMIdToConstraintId_.Keys(constraintId);
		for (size_t csmId : csmIds) {
			for (size_t propertyId : CSMIdToInputPropertyIds_[csmId]) {
				PropertyIdToCSMs_[propertyId].erase(csmId);
				removedPropertyLinks.insert(propertyId);
			}
			CSMIdToInputPropertyIds_.erase(csmId);

			for (size_t propertyId : CSMIdToOutputPropertyIds_[csmId]) {
				PropertyIdToCSMs_[propertyId].erase(csmId);
				removedPropertyLinks.insert(propertyId);
			}
			CSMIdToOutputPropertyIds_.erase(csmId);

			CSMIdToConstraintId_.Erase(csmId);
			CSMIdToOutputDegree_.Erase(csmId);
		}

		for (const auto &propertyId : removedPropertyLinks) {
			PropertyIdToDegree_.Insert(propertyId, PropertyIdToDegree_[propertyId] - 1);
		}
	}

	void CopyConstraintFrom(const TConstraintGraph &other, size_t constraintId) {
		if (ConstraintIds_.contains(constraintId)) {
			throw std::runtime_error("Constraint already present in this graph");
		}
		ConstraintIds_.insert(constraintId);

		std::unordered_set<size_t> addedPropertyLinks;

		auto csmIds = other.CSMIdToConstraintId_.Keys(constraintId);
		for (size_t csmId : csmIds) {
			if (CSMIdToConstraintId_.Contains(csmId)) {
				throw std::runtime_error("CSM already present in this graph");
			}
			CSMIdToConstraintId_.Insert(csmId, constraintId);

			CSMIdToOutputDegree_.Insert(csmId, other.CSMIdToOutputDegree_[csmId]);

			CSMIdToInputPropertyIds_[csmId] = other.CSMIdToInputPropertyIds_.at(csmId);
			CSMIdToOutputPropertyIds_[csmId] = other.CSMIdToOutputPropertyIds_.at(csmId);

			for (size_t propertyId : CSMIdToInputPropertyIds_[csmId]) {
				if (!PropertyIdToDegree_.Contains(propertyId)) {
					PropertyIdToDegree_.Insert(propertyId, 0);
					PropertyIdToCSMs_.insert({propertyId, {}});
				}
				addedPropertyLinks.insert(propertyId);
				PropertyIdToCSMs_[propertyId].insert(csmId);
			}
			for (size_t propertyId : CSMIdToOutputPropertyIds_[csmId]) {
				if (!PropertyIdToDegree_.Contains(propertyId)) {
					PropertyIdToDegree_.Insert(propertyId, 0);
					PropertyIdToCSMs_.insert({propertyId, {}});
				}
				addedPropertyLinks.insert(propertyId);
				PropertyIdToCSMs_[propertyId].insert(csmId);
			}
		}

		for (const auto &propertyId : addedPropertyLinks) {
			PropertyIdToDegree_.Insert(propertyId, PropertyIdToDegree_[propertyId] + 1);
		}
	}

	[[nodiscard]] bool HasCSMs() const {
		return !CSMIdToInputPropertyIds_.empty();
	}

	[[nodiscard]] std::set<size_t> GetConstraintIds() const {
		return {ConstraintIds_.begin(), ConstraintIds_.end()};
	}

	[[nodiscard]] size_t GetConstraintIdByCSM(size_t csmId) const {
		return CSMIdToConstraintId_[csmId];
	}

	[[nodiscard]] bool HasPropertyWithDegree(size_t degree) const {
		return PropertyIdToDegree_.ContainsValue(degree);
	}

	[[nodiscard]] size_t GetPropertyWithDegree(size_t degree) const {
		auto s = PropertyIdToDegree_.Keys(degree);
		if (s.empty()) {
			throw std::out_of_range("No property with given degree");
		}
		return *s.begin();
	}

	[[nodiscard]] bool HasCSMWithOutputDegree(size_t degree) const {
		return CSMIdToOutputDegree_.ContainsValue(degree);
	}

	[[nodiscard]] size_t GetCSMWithOutputDegree(size_t degree) const {
		auto s = CSMIdToOutputDegree_.Keys(degree);
		if (s.empty()) {
			throw std::out_of_range("No CSM with given output degree");
		}
		return *s.begin();
	}

private:
	std::unordered_set<size_t> ConstraintIds_;
	TBidirectionalMap<size_t, size_t> PropertyIdToDegree_;
	TBidirectionalMap<size_t, size_t> CSMIdToOutputDegree_;
	TBidirectionalMap<size_t, size_t> CSMIdToConstraintId_;
	std::unordered_map<size_t, std::unordered_set<size_t>> PropertyIdToCSMs_;
	std::unordered_map<size_t, std::unordered_set<size_t>> CSMIdToInputPropertyIds_;
	std::unordered_map<size_t, std::unordered_set<size_t>> CSMIdToOutputPropertyIds_;
};

std::vector<size_t> SieveDown(TConstraintGraph &graph) {
	std::vector<size_t> result;

	while (graph.HasCSMs()) {
		if (graph.HasPropertyWithDegree(1u)) {
			size_t propertyId = graph.GetPropertyWithDegree(1u);
			graph.RemoveProperty(propertyId);
			continue;
		}

		if (graph.HasCSMWithOutputDegree(0u)) {
			size_t csmId = graph.GetCSMWithOutputDegree(0u);
			size_t constraintId = graph.GetConstraintIdByCSM(csmId);
			graph.RemoveConstraint(constraintId);
			result.push_back(csmId);
			continue;
		}

		break;
	}

	return result;
}

std::vector<size_t> SieveUp(TConstraintGraph &graph) {
	std::vector<size_t> result;
	TConstraintGraph preGraph;
	TConstraintGraph postGraph;

	for (const auto &constraint : graph.GetConstraintIds()) {
		TConstraintGraph newPreGraph = preGraph;
		newPreGraph.CopyConstraintFrom(graph, constraint);

		TConstraintGraph newPostGraph = newPreGraph;
		std::vector<size_t> newResult = SieveDown(newPostGraph);

		if (!newPostGraph.HasCSMs()) {
			result = std::move(newResult);
			preGraph = std::move(newPreGraph);
			postGraph = std::move(newPostGraph);
		}
	}

	graph = postGraph;
	return result;
}

}  // namespace

EApplicability TQuickPlanSolver::IsApplicable(const TTask &task) const {
	std::vector<std::unordered_set<size_t>> constraintDomains(task.ConstraintsCount);
	for (const auto &csm : task.CSMs) {
		if (csm.ConstraintId >= task.ConstraintsCount) {
			throw std::invalid_argument("constraint id is to large");
		}
		for (const auto &id : csm.InputPropertieIds) {
			if (id >= task.PropertiesCount) {
				throw std::invalid_argument("propertie id is to large");
			}
		}
		for (const auto &id : csm.OutputPropertieIds) {
			if (id >= task.PropertiesCount) {
				throw std::invalid_argument("propertie id is to large");
			}
		}

		std::unordered_set<size_t> inputSet(csm.InputPropertieIds.begin(), csm.InputPropertieIds.end());
		for (const auto &id : csm.OutputPropertieIds) {
			if (inputSet.contains(id)) {
				throw std::invalid_argument(
				    "input and output properties intersect"
				);
			}
		}

		auto &domain = constraintDomains[csm.ConstraintId];
		if (domain.empty()) {
			domain.insert(csm.InputPropertieIds.begin(), csm.InputPropertieIds.end());
			domain.insert(csm.OutputPropertieIds.begin(), csm.OutputPropertieIds.end());
			continue;
		}

		if (domain.size() != csm.InputPropertieIds.size() + csm.OutputPropertieIds.size()) {
			return EApplicability::NOT_APPLICABLE;
		}

		for (const auto id : csm.InputPropertieIds) {
			if (domain.contains(id)) {
				continue;
			}

			return EApplicability::NOT_APPLICABLE;
		}

		for (const auto id : csm.OutputPropertieIds) {
			if (domain.contains(id)) {
				continue;
			}

			return EApplicability::NOT_APPLICABLE;
		}
	}

	return EApplicability::APPLICABLE;
}

std::optional<TSolution> TQuickPlanSolver::TrySolve(const TTask &task) const {
	if (IsApplicable(task) != EApplicability::APPLICABLE) {
		return std::nullopt;
	}

	TConstraintGraph graph(task);

	TSolution solution{
	    .CSMIds = SieveDown(graph),
	};
	auto h = SieveUp(graph);
	solution.CSMIds.insert(solution.CSMIds.begin(), h.begin(), h.end());

	std::ranges::reverse(solution.CSMIds);

	return solution;
}

}  // namespace NPropertyModels::NSolver

