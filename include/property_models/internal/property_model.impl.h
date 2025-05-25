#ifndef NPROPERTY_MODELS_IMPL_ALLOWED
#error "This header may not be included directly. Please include \"property_models/model.h\" instead"
#endif

#include <numeric>
#include <unordered_set>

#include "solver/solver.h"

namespace NPropertyModels {

template <typename TModel>
template <typename... TArgs>
void TPropertyModel<TModel>::RegisterCallback(TArgs &&...args) {
	Callback_ = std::function<void()>{std::forward<TArgs>(args)...};
}

template <typename TModel>
void TPropertyModel<TModel>::UnregisterCallback() {
	Callback_ = {};
}

// TFreezeGuard Implementation
template <typename TModel>
TPropertyModel<TModel>::TFreezeGuard::TFreezeGuard(TPropertyModel &base)
    : Base_(base) {
	Base_.DoFreeze();
}

template <typename TModel>
TPropertyModel<TModel>::TFreezeGuard::~TFreezeGuard() {
	Base_.DoUnfreeze();
}

template <typename TModel>
typename TPropertyModel<TModel>::TFreezeGuard TPropertyModel<TModel>::Freeze() {
	return TFreezeGuard(*this);
}

template <typename TModel>
size_t TPropertyModel<TModel>::RegisterProperty() {
	PropertySetTime_.push_back(0);
	return PropertySetTime_.size() - 1;
}

template <typename TModel>
size_t TPropertyModel<TModel>::RegisterConstraint(TConstraint<TThis> &constraint) {
	Constraints_.push_back(constraint);
	return Constraints_.size() - 1;
}

template <typename TModel>
void TPropertyModel<TModel>::OnPropertySet(size_t id) {
	if (Updating_) {
		return;
	};

	PropertySetTime_[id] = ++Time_;
	Update();
}

template <typename TModel>
void TPropertyModel<TModel>::OnConstraintSet(size_t id) {
	if (Updating_) {
		return;
	};

	Update();
}

template <typename TModel>
void TPropertyModel<TModel>::Update() {
	if (Updating_) {
		return;
	}
	Updating_ = true;

	std::vector<size_t> propertyOrder(PropertySetTime_.size());
	std::iota(propertyOrder.begin(), propertyOrder.end(), 0u);
	std::ranges::sort(propertyOrder, [this](size_t a, size_t b) { return PropertySetTime_[a] > PropertySetTime_[b]; });

	std::vector<size_t> constraintOrder(Constraints_.size());
	std::iota(constraintOrder.begin(), constraintOrder.end(), 0u);
	std::ranges::sort(constraintOrder, [this](size_t a, size_t b) {
		return Constraints_[a].get().GetImportance() < Constraints_[b].get().GetImportance();
	});

	NSolver::TTask task{
	    .PropertiesCount = PropertySetTime_.size(),
	    .ConstraintsCount = Constraints_.size() + PropertySetTime_.size(),
	};
	std::vector<TCSM<TThis> *> backPointers;

	for (size_t constraintNewId = 0; constraintNewId < constraintOrder.size(); ++constraintNewId) {
		auto &constraint = Constraints_[constraintOrder[constraintNewId]].get();
		if (!constraint.IsEnabled()) {
			continue;
		}

		for (auto &csmWrap : constraint.GetCSMs()) {
			TCSM<TThis> &csm = csmWrap.get();

			task.CSMs.emplace_back(
			    constraintNewId,
			    csm.GetInputPropertyIds(),
			    csm.GetOutputPropertyIds()
			);
			backPointers.push_back(&csm);
		}
	}

	for (size_t stayConstraintId = 0; stayConstraintId < propertyOrder.size(); ++stayConstraintId) {
		task.CSMs.emplace_back(
		    Constraints_.size() + stayConstraintId,
		    std::vector<size_t>{},
		    std::vector<size_t>{propertyOrder[stayConstraintId]}
		);
		backPointers.push_back(nullptr);
	}

	NSolver::TSolver solver = NSolver::GetSolver();
	auto maybeSolution = solver.TrySolve(task);

	if (!maybeSolution) {
		throw std::logic_error("Property model is to complex to be resolved.");
	}
	NSolver::TSolution &solution = maybeSolution.value();

	for (const auto &csmId : solution.CSMIds) {
		if (!backPointers[csmId]) {
			continue;
		};
		TCSM<TThis> &csm = *backPointers[csmId];

		csm.Apply();
	}

	std::unordered_set<size_t> fulfilledConstraintIds;
	for (const auto &csmId : solution.CSMIds) {
		size_t newConstraintId = task.CSMs[csmId].ConstraintId;
		if (newConstraintId >= constraintOrder.size()) {
			continue;
		}
		fulfilledConstraintIds.insert(constraintOrder[newConstraintId]);
	}

	for (auto &constraint : Constraints_) {
		constraint.get().Fulfilled_ = fulfilledConstraintIds.contains(constraint.get().Id_);
	}

	Updating_ = false;

	DoCallback();
}

template <typename TModel>
void TPropertyModel<TModel>::DoFreeze() {
	Freezed_ = true;
}

template <typename TModel>
void TPropertyModel<TModel>::DoUnfreeze() {
	Freezed_ = false;
	Update();
}

template <typename TModel>
void TPropertyModel<TModel>::DoCallback() {
	if (!Callback_) {
		return;
	}

	Callback_();
}

}  // namespace NPropertyModels

