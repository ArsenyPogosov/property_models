#ifndef NPROPERTY_MODELS_IMPL_ALLOWED
#error "This header may not be included directly. Please include \"property_models/model.h\" instead"
#endif

namespace NPropertyModels {

#define NPROPERTY_MODELS_PROPERTY_MODEL_IMPL(name) \
	class name : public NPropertyModels::TPropertyModel<name>

#define NPROPERTY_MODELS_PROPERTY_IMPL(type, name, ...) \
	NPropertyModels::TProperty<type, TThis> name {      \
		*this __VA_OPT__(, ) __VA_ARGS__                \
	}

#define NPROPERTY_MODELS_IMPORTANCE_IMPL(num) num
#define NPROPERTY_MODELS_CONSTRAINT_IMPL(name, ...)                                  \
	NPropertyModels::TConstraint<TThis> name = NPropertyModels::TConstraint<TThis> { \
		*this __VA_OPT__(, ) __VA_ARGS__                                             \
	}

#define NPROPERTY_MODELS_IN_IMPL(...) __VA_ARGS__
#define NPROPERTY_MODELS_OUT_IMPL(...) __VA_ARGS__
#define NPROPERTY_MODELS_CSM_DEFINE_IN(...) __VA_OPT__(auto [__VA_ARGS__] = getIn();)
#define NPROPERTY_MODELS_CSM_DEFINE_OUT(...) __VA_OPT__(auto [__VA_ARGS__] = getOut();)
#define NPROPERTY_MODELS_CSM_IMPL(in_args, out_args, ...)                                                \
	[this]() -> NPropertyModels::TCSM<TThis> {                                                           \
		std::vector<size_t> inIds = NPropertyModels::GetIds<TThis>(in_args);                             \
		std::vector<size_t> outIds = NPropertyModels::GetIds<TThis>(out_args);                           \
		auto getIn = [this]() {                                                                          \
			return NPropertyModels::ViewProperties<NPropertyModels::EAccess::READ, TThis>(in_args);      \
		};                                                                                               \
		auto getOut = [this]() {                                                                         \
			return NPropertyModels::ViewProperties<NPropertyModels::EAccess::WRITE, TThis>(out_args);    \
		};                                                                                               \
		std::function<void()> apply = [getIn = std::move(getIn), getOut = std::move(getOut)]() -> void { \
			NPROPERTY_MODELS_CSM_DEFINE_IN(in_args)                                                      \
			NPROPERTY_MODELS_CSM_DEFINE_OUT(out_args)                                                    \
			__VA_ARGS__                                                                                  \
		};                                                                                               \
		return {std::move(inIds), std::move(outIds), std::move(apply)};                                  \
	}()

}  // namespace NPropertyModels

