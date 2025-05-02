cmake_minimum_required(VERSION 4.0)

project(property_models VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(
	PROPERTY_MODELS_BUILD_TESTS
	"Build tests for property_models library"
	OFF
)
option(
	PROPERTY_MODELS_BUILD_EXAMPLE
	"Build example for property_models library"
	OFF
)

# property_models
add_library(
	property_models
	src/temp.cpp
)
target_include_directories(
	property_models
	INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
	PRIVATE "${PROJECT_BINARY_DIR}"
			"${CMAKE_CURRENT_SOURCE_DIR}/include/property_models"
			"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# tests
if(PROPERTY_MODELS_BUILD_TESTS)
	add_subdirectory(tests)
endif()

# example
if(PROPERTY_MODELS_BUILD_EXAMPLE)
	add_subdirectory(example)
endif()

