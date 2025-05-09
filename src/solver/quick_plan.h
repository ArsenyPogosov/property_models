#pragma once

#include "solver.h"

namespace NPropertyModels::NSolver {

class TQuickPlanSolver {
public:
	[[nodiscard]] EApplicability IsApplicable(const TTask &task) const;

	[[nodiscard]] std::optional<TSolution> TrySolve(const TTask &task) const;
};

}  // namespace NPropertyModels::NSolver

