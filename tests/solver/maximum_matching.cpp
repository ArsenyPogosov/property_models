#include "solver/maximum_matching.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "catch2/matchers/catch_matchers_vector.hpp"

namespace NPropertyModels::NSolver::NTesting {

namespace {

using namespace Catch::Matchers;

TEST_CASE("maximum matching implemets is applicable right", "[solver][maximum_matching][is_applicable]") {
	TSolver solver{TMaximumMatchingSolver{}};

	SECTION("incorrect task") {
		TTask task = GENERATE(
		    TTask{
		        .PropertiesCount = 2,
		        .ConstraintsCount = 0,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {1},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 1,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {1},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 1,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {0},
		            },
		        },
		    }
		);

		CHECK_THROWS(solver.IsApplicable(task));
	}

	SECTION("unappicable task") {
		TTask task{
		    .PropertiesCount = 3,
		    .ConstraintsCount = 1,
		    .CSMs{
		        {
		            .ConstraintId = 0,
		            .InputPropertyIds = {0},
		            .OutputPropertyIds = {1, 2},
		        },
		    },
		};

		CHECK(solver.IsApplicable(task) == EApplicability::NOT_APPLICABLE);
	}

	SECTION("maybe applicable task") {
		TTask task = GENERATE(
		    TTask{
		        .PropertiesCount = 3,
		        .ConstraintsCount = 3,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {1},
		            },
		            {
		                .ConstraintId = 1,
		                .InputPropertyIds = {1},
		                .OutputPropertyIds = {2},
		            },
		            {
		                .ConstraintId = 2,
		                .InputPropertyIds = {2},
		                .OutputPropertyIds = {0},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 2,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {1},
		            },
		        },
		    }
		);

		CHECK(solver.IsApplicable(task) == EApplicability::MAYBE_APPLICABLE);
	}
}

TEST_CASE("maximum matching implemets try solve right", "[solver][maximum_matching][try_solve]") {
	TSolver solver{TMaximumMatchingSolver{}};

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
		    }
		);

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
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {1},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 1,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {1},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 1,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {0},
		            },
		        },
		    }
		);

		std::optional<TSolution> solution;
		CHECK_THROWS(solution = solver.TrySolve(task));
	}

	SECTION("unappicable task") {
		TTask task = GENERATE(
		    TTask{
		        .PropertiesCount = 3,
		        .ConstraintsCount = 1,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {1, 2},
		            },
		        },
		    },
		    TTask{
		        .PropertiesCount = 3,
		        .ConstraintsCount = 3,
		        .CSMs{
		            {
		                .ConstraintId = 0,
		                .InputPropertyIds = {0},
		                .OutputPropertyIds = {1},
		            },
		            {
		                .ConstraintId = 1,
		                .InputPropertyIds = {1},
		                .OutputPropertyIds = {2},
		            },
		            {
		                .ConstraintId = 2,
		                .InputPropertyIds = {2},
		                .OutputPropertyIds = {0},
		            },
		        },
		    }
		);

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
		            .InputPropertyIds = {0},
		            .OutputPropertyIds = {1},
		        },
		        {
		            .ConstraintId = 0,
		            .InputPropertyIds = {0},
		            .OutputPropertyIds = {2},
		        },
		        {
		            .ConstraintId = 1,
		            .InputPropertyIds = {0},
		            .OutputPropertyIds = {1},
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
		            .InputPropertyIds = {},
		            .OutputPropertyIds = {2},
		        },
		        {
		            .ConstraintId = 1,
		            .InputPropertyIds = {2},
		            .OutputPropertyIds = {0},
		        },
		        {
		            .ConstraintId = 1,
		            .InputPropertyIds = {0},
		            .OutputPropertyIds = {2},
		        },
		        {
		            .ConstraintId = 3,
		            .InputPropertyIds = {},
		            .OutputPropertyIds = {3},
		        },
		        {
		            .ConstraintId = 4,
		            .InputPropertyIds = {0, 2},
		            .OutputPropertyIds = {3},
		        },
		        {
		            .ConstraintId = 4,
		            .InputPropertyIds = {0, 3},
		            .OutputPropertyIds = {2},
		        },
		        {
		            .ConstraintId = 4,
		            .InputPropertyIds = {2, 3},
		            .OutputPropertyIds = {0},
		        },
		        {
		            .ConstraintId = 5,
		            .InputPropertyIds = {2, 3},
		            .OutputPropertyIds = {},
		        },
		    },
		};

		std::optional<TSolution> solution;
		REQUIRE_NOTHROW(solution = solver.TrySolve(task));

		REQUIRE(solution.has_value());
		CHECK_THAT(solution.value().CSMIds, UnorderedEquals(std::vector<size_t>{0, 1, 3, 7}));
	}

	SECTION("right order") {
		TTask task{
		    .PropertiesCount = 6,
		    .ConstraintsCount = 5,
		    .CSMs{
		        {
		            .ConstraintId = 3,
		            .InputPropertyIds = {3},
		            .OutputPropertyIds = {4},
		        },
		        {
		            .ConstraintId = 1,
		            .InputPropertyIds = {1},
		            .OutputPropertyIds = {2},
		        },
		        {
		            .ConstraintId = 0,
		            .InputPropertyIds = {0},
		            .OutputPropertyIds = {1},
		        },
		        {
		            .ConstraintId = 4,
		            .InputPropertyIds = {4},
		            .OutputPropertyIds = {5},
		        },
		        {
		            .ConstraintId = 2,
		            .InputPropertyIds = {2},
		            .OutputPropertyIds = {3},
		        },
		    },
		};

		std::optional<TSolution> solution;
		REQUIRE_NOTHROW(solution = solver.TrySolve(task));

		REQUIRE(solution.has_value());
		CHECK_THAT(solution.value().CSMIds, Equals(std::vector<size_t>{2, 1, 4, 0, 3}));
	}
}

}  // namespace

}  // namespace NPropertyModels::NSolver::NTesting

