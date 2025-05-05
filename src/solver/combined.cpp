#include "combined.h"

namespace NPropertyModels::NSolver {

TCombinedSolver::TCombinedSolver(std::vector<TSolver> slaves)
    : Slaves_(std::move(slaves)) {
}

bool TCombinedSolver::IsApplicable(const TTask &task) const {
	for (const auto &slave : Slaves_) {
		if (!slave.IsApplicable(task)) {
			continue;
		}

		return true;
	}

	return false;
}

TSolution TCombinedSolver::Solve(const TTask &task) const {
	for (const auto &slave : Slaves_) {
		if (!slave.IsApplicable(task)) {
			continue;
		}

		return slave.Solve(task);
	}

	throw std::logic_error("this solver is not applicable");
}

}  // namespace NPropertyModels::NSolver

