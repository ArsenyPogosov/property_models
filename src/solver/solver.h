#pragma once

#include <any>
#include <functional>
#include <vector>

namespace NPropertyModels::NSolver {

struct TCSM {
	size_t ConstraintId;
	std::vector<size_t> InputPropertieIds;
	std::vector<size_t> OutputPropertieIds;
};

struct TTask {
	size_t PropertiesCount;
	size_t ConstraintsCount;
	std::vector<TCSM> CSMs;
};

struct TSolution {
	std::vector<size_t> CSMIds;
};

class TSolver {
public:
	template <typename T>
	    requires(!std::is_same_v<std::decay_t<T>, TSolver>)
	explicit(false) TSolver(T &&solver);

	[[nodiscard]] bool IsApplicable(const TTask &task) const;

	[[nodiscard]] TSolution Solve(const TTask &task) const;

private:
	std::any Solver_;
	std::function<bool(const std::any &, const TTask &)> IsApplicable_;
	std::function<TSolution(const std::any &, const TTask &task)> Solve_;
};

TSolver GetSolver();

/////////////////////////////////////////////////////////////////////////

template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, TSolver>)
TSolver::TSolver(T &&solver)
    : Solver_(std::forward<T>(solver)),
      IsApplicable_([](const std::any &solver, const TTask &task) -> bool {
	      return std::any_cast<const T &>(solver).IsApplicable(task);
      }),
      Solve_([](const std::any &solver, const TTask &task) -> TSolution {
	      return std::any_cast<const T &>(solver).Solve(task);
      }) {
}

[[nodiscard]] bool TSolver::IsApplicable(const TTask &task) const {
	return IsApplicable_(Solver_, task);
}

[[nodiscard]] TSolution TSolver::Solve(const TTask &task) const {
	return Solve_(Solver_, task);
}

}  // namespace NPropertyModels::NSolver

