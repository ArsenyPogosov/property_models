include(FetchContent)
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v3.8.1
)
FetchContent_MakeAvailable(Catch2)
include(CTest)
include(Catch)
list(
	APPEND
	CMAKE_MODULE_PATH
	${catch2_SOURCE_DIR}/extras
)

add_executable(
	tests
)
add_subdirectory(solver)
target_include_directories(
	tests
	PRIVATE "${CMAKE_SOURCE_DIR}/include/property_models"
			"${CMAKE_SOURCE_DIR}/src"
)
target_link_libraries(
	tests
	PRIVATE property_models
			Catch2::Catch2WithMain
)

catch_discover_tests(tests)

