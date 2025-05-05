#pragma once

#include <vector>

#include "solver.h"

namespace NPropertyModels::NSolver {

class TCombinedSolver {
public:
	explicit TCombinedSolver(std::vector<TSolver> slaves);

	[[nodiscard]] bool IsApplicable(const TTask &task) const;

	[[nodiscard]] TSolution Solve(const TTask &task) const;

private:
	std::vector<TSolver> Slaves_;
};

}  // namespace NPropertyModels::NSolver

