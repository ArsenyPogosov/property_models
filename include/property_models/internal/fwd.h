#pragma once

#ifndef NPROPERTY_MODELS_IMPL_ALLOWED
#error "This header may not be included directly. Please include \"property_models/model.h\" instead"
#endif

#include <cstdint>

namespace NPropertyModels {

template <typename TModel>
class TPropertyModel;

template <typename TValue, typename TModel>
class TProperty;

template <typename TModel>
class TConstraint;

// internal
enum class EAccess : uint8_t;

template <typename TValue, typename TModel, EAccess ACCESS>
class TPropertyView;

template <typename TModel>
class TCSM;
// internal

}  // namespace NPropertyModels

