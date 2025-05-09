#include "solver/quick_plan.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "catch2/matchers/catch_matchers_vector.hpp"

namespace NPropertyModels::NSolver::NTesting {

namespace {

using namespace Catch::Matchers;

TEST_CASE("quick plan implemets is applicable right",
          "[solver][quick_plan][is_applicable]") {
	TSolver solver{TQuickPlanSolver{}};

	SECTION("incorrect task") {
		TTask task = GENERATE(
		    TTask{
		        .PropertiesCount = 2,
		        .ConstraintsCount = 0,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertieIds = {0},
		                .OutputPropertieIds = {1},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 1,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertieIds = {0},
		                .OutputPropertieIds = {1},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 1,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertieIds = {0},
		                .OutputPropertieIds = {0},
		            },
		        },
		    });

		CHECK_THROWS(solver.IsApplicable(task));
	}

	SECTION("unappicable task") {
		TTask task{
		    .PropertiesCount = 3,
		    .ConstraintsCount = 1,
		    .CSMs{
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {0},
		            .OutputPropertieIds = {1},
		        },
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {0},
		            .OutputPropertieIds = {2},
		        },
		    },
		};

		CHECK(solver.IsApplicable(task) == EApplicability::NOT_APPLICABLE);
	}

	SECTION("applicable task") {
		TTask task{
		    .PropertiesCount = 3,
		    .ConstraintsCount = 2,
		    .CSMs{
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {0, 1},
		            .OutputPropertieIds = {2},
		        },
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {0, 2},
		            .OutputPropertieIds = {1},
		        },
		        {
		            .ConstraintId = 1,
		            .InputPropertieIds = {0, 1, 2},
		            .OutputPropertieIds = {},
		        },
		    },
		};

		CHECK(solver.IsApplicable(task) == EApplicability::APPLICABLE);
	}
}

TEST_CASE("quick plan implemets try solve right",
          "[solver][quick_plan][try_solve]") {
	TSolver solver{TQuickPlanSolver{}};

	SECTION("empty task") {
		TTask task = GENERATE(
		    TTask{
		        .PropertiesCount = 0,
		        .ConstraintsCount = 0,
		        .CSMs{},
		    },
		    TTask{
		        .PropertiesCount = 5,
		        .ConstraintsCount = 0,
		        .CSMs{},
		    },
		    TTask{
		        .PropertiesCount = 0,
		        .ConstraintsCount = 5,
		        .CSMs{},
		    },
		    TTask{
		        .PropertiesCount = 5,
		        .ConstraintsCount = 5,
		        .CSMs{},
		    });

		std::optional<TSolution> solution;
		REQUIRE_NOTHROW(solution = solver.TrySolve(task));

		REQUIRE(solution.has_value());
		CHECK(solution.value().CSMIds.empty());
	}

	SECTION("incorrect task") {
		TTask task = GENERATE(
		    TTask{
		        .PropertiesCount = 2,
		        .ConstraintsCount = 0,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertieIds = {0},
		                .OutputPropertieIds = {1},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 1,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertieIds = {0},
		                .OutputPropertieIds = {1},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 1,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertieIds = {0},
		                .OutputPropertieIds = {0},
		            },
		        },
		    });

		std::optional<TSolution> solution;
		CHECK_THROWS(solution = solver.TrySolve(task));
	}

	SECTION("unappicable task") {
		TTask task{
		    .PropertiesCount = 3,
		    .ConstraintsCount = 1,
		    .CSMs{
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {0},
		            .OutputPropertieIds = {1},
		        },
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {0},
		            .OutputPropertieIds = {2},
		        },
		    },
		};

		std::optional<TSolution> solution;
		REQUIRE_NOTHROW(solution = solver.TrySolve(task));

		CHECK_FALSE(solution.has_value());
	}

	SECTION("simple") {
		/*
		        o
		       / \
		      x   x
		     / \ /
		    o   o
		*/
		TTask task{
		    .PropertiesCount = 3,
		    .ConstraintsCount = 2,
		    .CSMs{
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {0, 2},
		            .OutputPropertieIds = {1},
		        },
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {0, 1},
		            .OutputPropertieIds = {2},
		        },
		        {
		            .ConstraintId = 1,
		            .InputPropertieIds = {0},
		            .OutputPropertieIds = {1},
		        },
		    },
		};

		std::optional<TSolution> solution;
		REQUIRE_NOTHROW(solution = solver.TrySolve(task));

		REQUIRE(solution.has_value());
		CHECK_THAT(solution.value().CSMIds, UnorderedEquals(std::vector<size_t>{1, 2}));
	}

	SECTION("hard") {
		/*
		   o--x  o
		   |  |
		   x--o--x
		   |  |
		   o--x
		   |
		   x  x
		*/
		TTask task{
		    .PropertiesCount = 4,
		    .ConstraintsCount = 6,
		    .CSMs{
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {},
		            .OutputPropertieIds = {2},
		        },
		        {
		            .ConstraintId = 1,
		            .InputPropertieIds = {2},
		            .OutputPropertieIds = {0},
		        },
		        {
		            .ConstraintId = 1,
		            .InputPropertieIds = {0},
		            .OutputPropertieIds = {2},
		        },
		        {
		            .ConstraintId = 3,
		            .InputPropertieIds = {},
		            .OutputPropertieIds = {3},
		        },
		        {
		            .ConstraintId = 4,
		            .InputPropertieIds = {0, 2},
		            .OutputPropertieIds = {3},
		        },
		        {
		            .ConstraintId = 4,
		            .InputPropertieIds = {0, 3},
		            .OutputPropertieIds = {2},
		        },
		        {
		            .ConstraintId = 4,
		            .InputPropertieIds = {2, 3},
		            .OutputPropertieIds = {0},
		        },
		        {
		            .ConstraintId = 5,
		            .InputPropertieIds = {2, 3},
		            .OutputPropertieIds = {},
		        },
		    },
		};

		std::optional<TSolution> solution;
		REQUIRE_NOTHROW(solution = solver.TrySolve(task));

		REQUIRE(solution.has_value());
		CHECK_THAT(solution.value().CSMIds,
		           UnorderedEquals(std::vector<size_t>{0, 1, 3, 7}));
	}

	SECTION("right order") {
		TTask task{
		    .PropertiesCount = 6,
		    .ConstraintsCount = 5,
		    .CSMs{
		        {
		            .ConstraintId = 3,
		            .InputPropertieIds = {3},
		            .OutputPropertieIds = {4},
		        },
		        {
		            .ConstraintId = 1,
		            .InputPropertieIds = {1},
		            .OutputPropertieIds = {2},
		        },
		        {
		            .ConstraintId = 0,
		            .InputPropertieIds = {0},
		            .OutputPropertieIds = {1},
		        },
		        {
		            .ConstraintId = 4,
		            .InputPropertieIds = {4},
		            .OutputPropertieIds = {5},
		        },
		        {
		            .ConstraintId = 2,
		            .InputPropertieIds = {2},
		            .OutputPropertieIds = {3},
		        },
		    },
		};

		std::optional<TSolution> solution;
		REQUIRE_NOTHROW(solution = solver.TrySolve(task));

		REQUIRE(solution.has_value());
		CHECK_THAT(solution.value().CSMIds,
		           Equals(std::vector<size_t>{2, 1, 4, 0, 3}));
	}
}

}  // namespace

}  // namespace NPropertyModels::NSolver::NTesting

