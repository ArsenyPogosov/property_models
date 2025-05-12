#ifndef NPROPERTY_MODELS_IMPL_ALLOWED
#error "This header may not be included directly. Please include \"property_models/model.h\" instead"
#endif

#include "csm.h"

namespace NPropertyModels {

template <typename TModel>
[[nodiscard]] size_t TConstraint<TModel>::GetImportance() const {
	return Importance_;
}

template <typename TModel>
void TConstraint<TModel>::SetImportance(size_t importance) {
	Importance_ = importance;
	OnSet();
}

template <typename TModel>
[[nodiscard]] bool TConstraint<TModel>::IsEnabled() const {
	return Enabled_;
}

template <typename TModel>
void TConstraint<TModel>::Enable() {
	if (Enabled_) {
		return;
	}

	Enabled_ = true;
	OnSet();
}

template <typename TModel>
void TConstraint<TModel>::Disable() {
	if (!Enabled_) {
		return;
	}

	Enabled_ = false;
	OnSet();
}

template <typename TModel>
[[nodiscard]] bool TConstraint<TModel>::IsFulfilled() const {
	return Enabled_ && Fulfilled_;
}

template <typename TModel>
[[nodiscard]] TConstraint<TModel>::operator bool() const {
	return IsFulfilled();
}

template <typename TModel>
template <typename... T>
    requires((std::same_as<std::remove_cvref_t<T>, TCSM<TModel>> && ...))
TConstraint<TModel>::TConstraint(TModel& model, size_t importance, T&&... csms)
    : Model_(model), Id_(Model_.RegisterConstraint(*this)), Importance_(importance), CSMs_{std::forward<T>(csms)...} {
}

template <typename TModel>
template <typename... T>
    requires((std::same_as<std::remove_cvref_t<T>, TCSM<TModel>> && ...))
TConstraint<TModel>::TConstraint(TModel& model, T&&... csms)
    : TConstraint(model, 0, std::forward<T>(csms)...) {
}

template <typename TModel>
TConstraint<TModel>::~TConstraint() = default;

template <typename TModel>
[[nodiscard]] std::vector<std::reference_wrapper<TCSM<TModel>>> TConstraint<TModel>::GetCSMs() {
	std::vector<std::reference_wrapper<TCSM<TModel>>> result;
	result.reserve(CSMs_.size());
	for (auto& csm : CSMs_) {
		result.emplace_back(csm);
	}
	return result;
}

template <typename TModel>
void TConstraint<TModel>::OnSet() {
	Model_.OnConstraintSet(Id_);
}

}  // namespace NPropertyModels

