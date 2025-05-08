#include "maximum_matching.h"

#include <unordered_set>

namespace NPropertyModels::NSolver {

namespace {

struct TEdge {
	size_t FromId;
	size_t ToId;
};

struct TBipartiteGraph {
	size_t FirstPartCount;
	size_t SecondPartCount;
	std::vector<TEdge> Edges;
};

struct TGraph {
	size_t VerticesCount;
	std::vector<TEdge> Edges;
};

// kuhn O(VE)
[[nodiscard]] std::vector<size_t> GetMaxCostMatching(
    const TBipartiteGraph &graph) {
	std::vector<std::vector<size_t>> edgeIds(graph.FirstPartCount);
	for (size_t id = 0; id < graph.Edges.size(); ++id) {
		const TEdge &edge = graph.Edges[id];

		edgeIds[edge.FromId].push_back(id);
	}

	std::vector<bool> included(graph.FirstPartCount);
	std::vector<size_t> choosenEdge(graph.SecondPartCount, -1);

	std::vector<bool> visited(graph.FirstPartCount);
	auto dfsImpl = [&edges = std::as_const(graph.Edges),
	                &edgeIds = std::as_const(edgeIds),
	                &included,
	                &choosenEdge,
	                &visited](const auto &self, size_t u) -> bool {
		if (visited[u]) {
			return false;
		}
		visited[u] = true;

		for (const auto &id : edgeIds[u]) {
			size_t v = edges[id].ToId;

			if (choosenEdge[v] == -1 ||
			    self(self, edges[choosenEdge[v]].FromId)) {
				choosenEdge[v] = id;
				return true;
			}
		}

		return false;
	};
	auto dfs = [&dfsImpl, &visited](size_t u) -> bool {
		visited.assign(visited.size(), false);
		return dfsImpl(dfsImpl, u);
	};

	size_t matchingSize = 0;
	for (size_t i = 0; i < graph.FirstPartCount; ++i) {
		if (included[i]) {
			continue;
		}

		matchingSize += static_cast<size_t>(dfs(i));
	}

	std::vector<size_t> result;
	result.reserve(matchingSize);
	for (const auto &id : choosenEdge) {
		if (id == -1) {
			continue;
		}

		result.push_back(id);
	}

	return result;
}

[[nodiscard]] std::optional<std::vector<size_t>> GetTopOrder(
    const TGraph &graph) {
	std::vector<std::vector<size_t>> adjacencyList(graph.VerticesCount);
	for (const auto &[fromId, toId] : graph.Edges) {
		adjacencyList[fromId].push_back(toId);
	}

	std::vector<uint8_t> visited(graph.VerticesCount);
	std::vector<size_t> visitedOrder;
	visitedOrder.reserve(graph.VerticesCount);
	auto dfsImpl = [&adjacencyList = std::as_const(adjacencyList),
	                &visited,
	                &visitedOrder](const auto &self, size_t u) -> bool {
		if (visited[u] == 2) {
			return false;
		}
		if (visited[u] == 1) {
			return true;
		}
		++visited[u];

		for (const auto &v : adjacencyList[u]) {
			if (!self(self, v)) {
				continue;
			}

			return true;
		}

		++visited[u];
		visitedOrder.push_back(u);
		return false;
	};
	auto dfs = [&dfsImpl, &visited](size_t u) -> bool {
		return dfsImpl(dfsImpl, u);
	};

	for (size_t i = 0; i < graph.VerticesCount; ++i) {
		if (!dfs(i)) {
			continue;
		}

		return std::nullopt;
	}

	std::vector<size_t> topOrder(graph.VerticesCount);
	for (size_t i = 0; i < graph.VerticesCount; ++i) {
		topOrder[visitedOrder[i]] = graph.VerticesCount - i - 1;
	}

	return topOrder;
};

}  // namespace

EApplicability TMaximumMatchingSolver::IsApplicable(const TTask &task) const {
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

		std::unordered_set<size_t> inputSet(csm.InputPropertieIds.begin(),
		                                    csm.InputPropertieIds.end());
		for (const auto &id : csm.OutputPropertieIds) {
			if (inputSet.contains(id)) {
				throw std::invalid_argument(
				    "input and output properties intersect");
			}
		}

		if (csm.OutputPropertieIds.size() > 1) {
			return EApplicability::NOT_APPLICABLE;
		}
	}

	return EApplicability::MAYBE_APPLICABLE;  // ¯\_(ツ)_/¯
}

std::optional<TSolution> TMaximumMatchingSolver::TrySolve(
    const TTask &task) const {
	switch (IsApplicable(task)) {
		case EApplicability::NOT_APPLICABLE: {
			return std::nullopt;
		}
		case EApplicability::MAYBE_APPLICABLE: {
			break;
		}
		case EApplicability::APPLICABLE: {
			// std::unreachable()
			break;
		}
	}

	TBipartiteGraph matchingGraph{
	    .FirstPartCount = task.ConstraintsCount,
	    .SecondPartCount = task.PropertiesCount,
	};
	matchingGraph.Edges.reserve(task.CSMs.size());
	for (const auto &csm : task.CSMs) {
		if (csm.OutputPropertieIds.empty()) {
			// add fictitious vertex to both parts of the graph to work around
			// degenerate CSMs
			matchingGraph.Edges.push_back({
			    .FromId = matchingGraph.FirstPartCount++,
			    .ToId = matchingGraph.SecondPartCount++,
			});
			continue;
		}

		matchingGraph.Edges.push_back({
		    .FromId = csm.ConstraintId,
		    .ToId = csm.OutputPropertieIds.at(0),
		});
	}

	TSolution solution{
	    .CSMIds = GetMaxCostMatching(matchingGraph),
	};

	// check solution before reporting it;
	TGraph solutionGraph{
	    .VerticesCount = task.PropertiesCount + task.ConstraintsCount,
	};
	for (const auto &id : solution.CSMIds) {
		const auto &csm = task.CSMs[id];

		for (const auto &inputId : csm.InputPropertieIds) {
			solutionGraph.Edges.push_back({
			    .FromId = inputId + task.ConstraintsCount,
			    .ToId = csm.ConstraintId,
			});
		}

		for (const auto &outputId : csm.OutputPropertieIds) {
			solutionGraph.Edges.push_back({
			    .FromId = csm.ConstraintId,
			    .ToId = outputId + task.ConstraintsCount,
			});
		}
	}

	auto topOrder = GetTopOrder(solutionGraph);
	if (!topOrder.has_value()) {
		return std::nullopt;  // cycle encountered, maximum matching is
		                      // anapplicable
	}

	std::ranges::sort(
	    solution.CSMIds,
	    [&topOrder = std::as_const(topOrder.value()),
	     &task = std::as_const(task)](size_t a, size_t b) -> bool {
		    auto getValue = [&](size_t i) -> size_t {
			    return topOrder[task.CSMs[i].ConstraintId];
		    };
		    return getValue(a) < getValue(b);
	    });

	return solution;
}

}  // namespace NPropertyModels::NSolver

