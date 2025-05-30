#pragma once

#ifndef NPROPERTY_MODELS_IMPL_ALLOWED
#error "This header may not be included directly. Please include \"property_models/model.h\" instead"
#endif

#include <any>
#include <functional>
#include <vector>

namespace NPropertyModels::NSolver {

struct TCSM {
	size_t ConstraintId;
	std::vector<size_t> InputPropertyIds;
	std::vector<size_t> OutputPropertyIds;
};

struct TTask {
	size_t PropertiesCount;
	size_t ConstraintsCount;
	std::vector<TCSM> CSMs;
};

struct TSolution {
	std::vector<size_t> CSMIds;
};

enum class EApplicability : std::uint8_t {
	NOT_APPLICABLE,
	APPLICABLE,
	MAYBE_APPLICABLE,
};

class TSolver {
public:
	template <typename T>
	    requires(!std::is_same_v<std::decay_t<T>, TSolver>)
	explicit(false) TSolver(T &&solver);

	[[nodiscard]] EApplicability IsApplicable(const TTask &task) const;

	[[nodiscard]] std::optional<TSolution> TrySolve(const TTask &task) const;

private:
	std::any Solver_;
	std::function<EApplicability(const std::any &, const TTask &)>
	    IsApplicable_;
	std::function<std::optional<TSolution>(const std::any &, const TTask &task)>
	    Solve_;
};

[[nodiscard]] TSolver GetSolver();

/////////////////////////////////////////////////////////////////////////

template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, TSolver>)
inline TSolver::TSolver(T &&solver)
    : Solver_(std::forward<T>(solver)),
      IsApplicable_(
          [](const std::any &solver, const TTask &task) -> EApplicability {
	          return std::any_cast<const T &>(solver).IsApplicable(task);
          }
      ),
      Solve_([](const std::any &solver, const TTask &task) -> std::optional<TSolution> {
	      return std::any_cast<const T &>(solver).TrySolve(task);
      }) {
}

[[nodiscard]] inline EApplicability TSolver::IsApplicable(
    const TTask &task
) const {
	return IsApplicable_(Solver_, task);
}

[[nodiscard]] inline std::optional<TSolution> TSolver::TrySolve(
    const TTask &task
) const {
	return Solve_(Solver_, task);
}

}  // namespace NPropertyModels::NSolver

