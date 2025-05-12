#define NPROPERTY_MODELS_IMPL_ALLOWED
#include "internal/solver/solver.h"
#undef NPROPERTY_MODELS_IMPL_ALLOWED

#include "combined.h"
#include "maximum_matching.h"
#include "quick_plan.h"

namespace NPropertyModels::NSolver {

TSolver GetSolver() {
	return TCombinedSolver({TQuickPlanSolver{}, TMaximumMatchingSolver{}});
}

}  // namespace NPropertyModels::NSolver

