#pragma once

#define NPROPERTY_MODELS_IMPL_ALLOWED
#include "internal/solver/solver.h"
#undef NPROPERTY_MODELS_IMPL_ALLOWED

namespace NPropertyModels::NSolver {

class TQuickPlanSolver {
public:
	[[nodiscard]] EApplicability IsApplicable(const TTask &task) const;

	[[nodiscard]] std::optional<TSolution> TrySolve(const TTask &task) const;
};

}  // namespace NPropertyModels::NSolver

