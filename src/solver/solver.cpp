#include "solver.h"

#include "combined.h"

namespace NPropertyModels::NSolver {

TSolver GetSolver() {
	return TCombinedSolver({});
}

}  // namespace NPropertyModels::NSolver

