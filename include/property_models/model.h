#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>
#include <vector>

namespace NPropertyModels {

#define PM_PROPERTY_MODEL NPROPERTY_MODELS_PROPERTY_MODEL_IMPL

#define PM_PARAMETER NPROPERTY_MODELS_PARAMETER_IMPL

#define PM_IMPORTANCE NPROPERTY_MODELS_IMPORTANCE_IMPL
#define PM_CONSTRAINT NPROPERTY_MODELS_CONSTRAINT_IMPL

#define PM_IN NPROPERTY_MODELS_IN_IMPL
#define PM_OUT NPROPERTY_MODELS_OUT_IMPL
#define PM_CSM NPROPERTY_MODELS_CSM_IMPL

template <typename TModel>
class TConstraint;

template <typename TModel>
class TPropertyModel {
public:
	template <typename... TArgs>
	void RegisterCallback(TArgs &&...args);

	void UnregisterCallback();

	class TFreezeGuard {
	public:
		explicit TFreezeGuard(TPropertyModel &base);
		TFreezeGuard() = delete;
		TFreezeGuard(const TFreezeGuard &) = delete;
		TFreezeGuard(TFreezeGuard &&) = delete;
		TFreezeGuard &operator=(const TFreezeGuard &) = delete;
		TFreezeGuard &operator=(TFreezeGuard &&) = delete;
		~TFreezeGuard();

	private:
		TPropertyModel &Base_;
	};
	TFreezeGuard Freeze();

protected:
	using TThis = TModel;

private:
	friend TModel;
	template <typename, typename>
	friend class TProperty;
	template <typename>
	friend class TConstraint;
	friend class TFreezeGuard;
	TPropertyModel() = default;

private:
	size_t RegisterProperty();
	size_t RegisterConstraint(TConstraint<TThis> &constraint);
	void OnPropertySet(size_t id);
	void OnConstraintSet(size_t id);
	void Update();
	void DoFreeze();
	void DoUnfreeze();
	void DoCallback();

private:
	bool Freezed_ = false;
	bool Updating_ = false;
	std::function<void()> Callback_;
	size_t Time_ = 0;
	std::vector<size_t> PropertySetTime_;
	std::vector<std::reference_wrapper<TConstraint<TThis>>> Constraints_;
};

enum class EAccess : uint8_t;

template <typename TValue, typename TModel, EAccess ACCESS>
class TPropertyView;

template <typename TValue, typename TModel>
class TProperty {
public:
	template <typename... TArgs>
	explicit(false) TProperty(TModel &model, TArgs &&...args);
	TProperty(const TProperty &other) = delete;
	TProperty(TProperty &&other) = delete;

public:
	// NOLINTNEXTLINE
	explicit(false) operator const TValue &() const;

	template <typename TOther>
	// NOLINTNEXTLINE
	TValue &operator=(TOther &&other);

	[[nodiscard]] const TValue &Get() const;

	const TValue &Set(const TValue &value);
	const TValue &Set(TValue &&value);

private:
	friend TModel;
	friend TPropertyModel<TModel>;
	~TProperty();

private:
	void OnGet() const;
	void OnSet();

	template <EAccess ACCESS_, typename TModel_, typename... TValues_>
	friend constexpr std::tuple<TPropertyView<TValues_, TModel_, ACCESS_>...>
	ViewProperties(TProperty<TValues_, TModel_> &...);

	template <typename TModel_, typename... TValues_>
	friend constexpr std::vector<size_t> GetIds(const TProperty<TValues_, TModel_> &...);

private:
	TModel &Model_;
	const size_t Id_;
	TValue Value_;
};

template <typename TModel>
class TCSM;

template <typename TModel>
class TConstraint {
public:
	[[nodiscard]] size_t GetImportance() const;
	void SetImportance(size_t importance);
	[[nodiscard]] bool IsEnabled() const;
	void Enable();
	void Disable();
	[[nodiscard]] bool IsFulfilled() const;
	// NOLINTNEXTLINE
	[[nodiscard]] explicit(false) operator bool() const;

private:
	friend TModel;
	friend TPropertyModel<TModel>;

	template <typename... T>
	    requires((std::same_as<std::remove_cvref_t<T>, TCSM<TModel>> && ...))
	explicit TConstraint(TModel &model, size_t importance, T &&...csms);

	template <typename... T>
	    requires((std::same_as<std::remove_cvref_t<T>, TCSM<TModel>> && ...))
	explicit TConstraint(TModel &model, T &&...csms);

	~TConstraint();

private:
	[[nodiscard]] std::vector<std::reference_wrapper<TCSM<TModel>>> GetCSMs();
	void OnSet();

private:
	TModel &Model_;
	size_t Id_;

	size_t Importance_ = 0;
	bool Enabled_ = true;
	bool Fulfilled_ = true;

	std::vector<TCSM<TModel>> CSMs_;
};

}  // namespace NPropertyModels

#define NPROPERTY_MODELS_IMPL_ALLOWED
#include "internal/constraint.impl.h"
#include "internal/macros.impl.h"
#include "internal/property.impl.h"
#include "internal/property_model.impl.h"
#undef NPROPERTY_MODELS_IMPL_ALLOWED

