#pragma once

#include <vector>

#include "solver.h"

namespace NPropertyModels::NSolver {

class TCombinedSolver {
public:
	explicit TCombinedSolver(std::vector<TSolver> slaves);

	[[nodiscard]] EApplicability IsApplicable(const TTask &task) const;

	[[nodiscard]] std::optional<TSolution> TrySolve(const TTask &task) const;

private:
	std::vector<TSolver> Slaves_;
};

}  // namespace NPropertyModels::NSolver

