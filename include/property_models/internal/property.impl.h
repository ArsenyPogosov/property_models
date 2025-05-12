#ifndef NPROPERTY_MODELS_IMPL_ALLOWED
#error "This header may not be included directly. Please include \"property_models/model.h\" instead"
#endif

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <vector>

namespace NPropertyModels {

template <typename TValue, typename TModel>
template <typename... TArgs>
TProperty<TValue, TModel>::TProperty(TModel &model, TArgs &&...args)
    : Model_(model), Id_(Model_.RegisterProperty()), Value_(std::forward<TArgs>(args)...) {
}

template <typename TValue, typename TModel>
TProperty<TValue, TModel>::~TProperty() = default;

template <typename TValue, typename TModel>
TProperty<TValue, TModel>::operator const TValue &() const {
	OnGet();
	return Value_;
}

// NOLINTNEXTLINE
template <typename TValue, typename TModel>
template <typename TOther>
// NOLINTNEXTLINE
TValue &TProperty<TValue, TModel>::operator=(TOther &&other) {
	Value_ = std::forward<TOther>(other);
	OnSet();

	return Value_;
}

template <typename TValue, typename TModel>
const TValue &TProperty<TValue, TModel>::Get() const {
	return Value_;
}

template <typename TValue, typename TModel>
const TValue &TProperty<TValue, TModel>::Set(const TValue &value) {
	Value_ = value;
	OnSet();

	return Value_;
}

template <typename TValue, typename TModel>
const TValue &TProperty<TValue, TModel>::Set(TValue &&value) {
	Value_ = std::move(value);
	OnSet();

	return Value_;
}

template <typename TValue, typename TModel>
void TProperty<TValue, TModel>::OnGet() const {
}

template <typename TValue, typename TModel>
void TProperty<TValue, TModel>::OnSet() {
	Model_.OnPropertySet(Id_);
}

enum class EAccess : uint8_t {
	NONE,
	READ,
	WRITE,
	READ_WRITE,
};

constexpr bool CanRead(EAccess access) {
	switch (access) {
		case EAccess::NONE:
			return false;
		case EAccess::READ:
			return true;
		case EAccess::WRITE:
			return false;
		case EAccess::READ_WRITE:
			return true;
	}
	return false;  // avoids warning
}

constexpr bool CanWrite(EAccess access) {
	switch (access) {
		case EAccess::NONE:
			return false;
		case EAccess::READ:
			return false;
		case EAccess::WRITE:
			return true;
		case EAccess::READ_WRITE:
			return true;
	}
	return false;  // avoids warning
}

template <typename TValue, typename TModel, EAccess ACCESS>
class TPropertyView {
public:
	// NOLINTNEXTLINE
	explicit(false) operator const TValue &() const {
		static_assert(CanRead(ACCESS), "Property is not marked as input");
		return static_cast<const TValue &>(Underlying_);
	}

	[[nodiscard]] const TValue &Get() const {
		static_assert(CanRead(ACCESS), "Property is not marked as input");
		return Underlying_.Get();
	}

	template <typename TOther>
	// NOLINTNEXTLINE
	TValue &operator=(TOther &&other) {
		static_assert(CanWrite(ACCESS), "Property is not marked as output");
		return Underlying_ = std::forward<TOther>(other);
	}

	const TValue &Set(const TValue &value) {
		static_assert(CanWrite(ACCESS), "Property is not marked as output");
		return Underlying_.Set(value);
	}

	const TValue &Set(TValue &&value) {
		static_assert(CanWrite(ACCESS), "Property is not marked as output");
		return Underlying_.Set(std::move(value));
	}

private:
	friend TModel;

	explicit TPropertyView(TProperty<TValue, TModel> &underlying)
	    : Underlying_(underlying) {
	}

private:
	template <EAccess ACCESS_, typename TModel_, typename... TValues_>
	friend constexpr std::tuple<TPropertyView<TValues_, TModel_, ACCESS_>...>
	ViewProperties(TProperty<TValues_, TModel_> &...);

private:
	TProperty<TValue, TModel> &Underlying_;
};

template <EAccess ACCESS, typename TModel, typename... TValues>
[[nodiscard]] constexpr std::tuple<TPropertyView<TValues, TModel, ACCESS>...>
ViewProperties(TProperty<TValues, TModel> &...properties) {
	return std::make_tuple(TPropertyView<TValues, TModel, ACCESS>(properties)...);
}

template <typename TModel, typename... TValues>
[[nodiscard]] constexpr std::vector<size_t>
GetIds(const TProperty<TValues, TModel> &...properties) {
	return {properties.Id_...};
}

}  // namespace NPropertyModels

