#pragma once

#ifndef NPROPERTY_MODELS_IMPL_ALLOWED
#error "This header may not be included directly. Please include \"property_models/model.h\" instead"
#endif

#include <functional>
#include <vector>

namespace NPropertyModels {

template <typename>
class TPropertyModel;

template <typename TModel>
class TCSM {
public:
	TCSM() = delete;

private:
	friend TModel;
	friend TPropertyModel<TModel>;

	TCSM(std::vector<size_t> inputPropertyIds, std::vector<size_t> outputPropertyIds, std::function<void()> apply)
	    : InputPropertyIds_(std::move(inputPropertyIds)), OutputPropertyIds_(std::move(outputPropertyIds)), Apply_(std::move(apply)) {
	}

private:
	[[nodiscard]] std::vector<size_t> GetInputPropertyIds() const {
		return InputPropertyIds_;
	}

	[[nodiscard]] std::vector<size_t> GetOutputPropertyIds() const {
		return OutputPropertyIds_;
	}

	void Apply() {
		if (!Apply_) {
			return;
		}
		Apply_();
	}

private:
	std::vector<size_t> InputPropertyIds_;
	std::vector<size_t> OutputPropertyIds_;
	std::function<void()> Apply_;
};

}  // namespace NPropertyModels

