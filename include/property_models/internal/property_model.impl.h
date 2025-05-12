#ifndef NPROPERTY_MODELS_IMPL_ALLOWED
#error "This header may not be included directly. Please include \"property_models/model.h\" instead"
#endif

#include <numeric>

namespace NPropertyModels {

template <typename TModel>
template <typename... TArgs>
void TPropertyModel<TModel>::RegisterCallback(TArgs &&...args) {
	Callback_ = std::function<void()>{std::forward<TArgs>(args)...};
}

template <typename TModel>
void TPropertyModel<TModel>::UnregisterCallback() {
	Callback_ = {};
}

// TFreezeGuard Implementation
template <typename TModel>
TPropertyModel<TModel>::TFreezeGuard::TFreezeGuard(TPropertyModel &base)
    : Base_(base) {
	Base_.DoFreeze();
}

template <typename TModel>
TPropertyModel<TModel>::TFreezeGuard::~TFreezeGuard() {
	Base_.DoUnfreeze();
}

template <typename TModel>
typename TPropertyModel<TModel>::TFreezeGuard TPropertyModel<TModel>::Freeze() {
	return TFreezeGuard(*this);
}

template <typename TModel>
size_t TPropertyModel<TModel>::RegisterProperty() {
	PropertySetTime_.push_back(0);
	return PropertySetTime_.size() - 1;
}

template <typename TModel>
size_t TPropertyModel<TModel>::RegisterConstraint(TConstraint<TThis> &constraint) {
	Constraints_.push_back(constraint);
	return Constraints_.size() - 1;
}

template <typename TModel>
void TPropertyModel<TModel>::OnPropertySet(size_t id) {
	PropertySetTime_[id] = ++Time_;
	Update();
}

template <typename TModel>
void TPropertyModel<TModel>::OnConstraintSet(size_t id) {
	Update();
}

template <typename TModel>
void TPropertyModel<TModel>::Update() {
	if (Freezed_) {
		return;
	}
	Freezed_ = true;

	std::vector<size_t> order(PropertySetTime_.size());
	std::iota(order.begin(), order.end(), 0u);
	std::ranges::sort(order, [this](size_t a, size_t b) { return PropertySetTime_[a] > PropertySetTime_[b]; });

	Freezed_ = false;

	DoCallback();
}

template <typename TModel>
void TPropertyModel<TModel>::DoFreeze() {
	Freezed_ = true;
}

template <typename TModel>
void TPropertyModel<TModel>::DoUnfreeze() {
	Freezed_ = false;
	Update();
}

template <typename TModel>
void TPropertyModel<TModel>::DoCallback() {
	if (!Callback_) {
		return;
	}

	Callback_();
}

}  // namespace NPropertyModels

