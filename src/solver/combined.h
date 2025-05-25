#pragma once

#define NPROPERTY_MODELS_IMPL_ALLOWED
#include "internal/solver/solver.h"
#undef NPROPERTY_MODELS_IMPL_ALLOWED

#include <vector>

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

