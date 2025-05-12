#include "combined.h"

namespace NPropertyModels::NSolver {

TCombinedSolver::TCombinedSolver(std::vector<TSolver> slaves)
    : Slaves_(std::move(slaves)) {
}

EApplicability TCombinedSolver::IsApplicable(const TTask &task) const {
	bool unknownEncountered = false;
	for (const auto &slave : Slaves_) {
		switch (slave.IsApplicable(task)) {
			case EApplicability::NOT_APPLICABLE: {
				continue;
			}
			case EApplicability::APPLICABLE: {
				return EApplicability::APPLICABLE;
			}
			case EApplicability::MAYBE_APPLICABLE: {
				unknownEncountered = true;
				continue;
			}
		}
	}

	return unknownEncountered ? EApplicability::MAYBE_APPLICABLE
	                          : EApplicability::NOT_APPLICABLE;
}

std::optional<TSolution> TCombinedSolver::TrySolve(const TTask &task) const {
	std::vector<std::reference_wrapper<const TSolver>> maybeApplicableSlaves;
	for (const auto &slave : Slaves_) {
		switch (slave.IsApplicable(task)) {
			case EApplicability::NOT_APPLICABLE: {
				continue;
			}
			case EApplicability::APPLICABLE: {
				auto maybeResult = slave.TrySolve(task);
				if (maybeResult) {
					return maybeResult;
				}
			}
			case EApplicability::MAYBE_APPLICABLE: {
				maybeApplicableSlaves.emplace_back(slave);
				continue;
			}
		}
	}

	for (const auto &slaveWrapped : maybeApplicableSlaves) {
		const auto &slave = slaveWrapped.get();

		auto maybeResult = slave.TrySolve(task);
		if (maybeResult) {
			return maybeResult;
		}
	}

	return std::nullopt;
}

}  // namespace NPropertyModels::NSolver

