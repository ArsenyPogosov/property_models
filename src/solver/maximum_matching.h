#pragma once

#include "solver.h"

namespace NPropertyModels::NSolver {

class TMaximumMatchingSolver {
public:
	[[nodiscard]] EApplicability IsApplicable(const TTask &task) const;

	[[nodiscard]] std::optional<TSolution> TrySolve(const TTask &task) const;

private:
	std::vector<TSolver> Slaves_;
};

}  // namespace NPropertyModels::NSolver

