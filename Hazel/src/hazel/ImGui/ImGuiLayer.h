#pragma once

#include "hazel/Layer.h"

namespace hazel {
class HAZEL_API ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer();

	void onAttach() override;
	void onDetach() override;
	void onUpdate() override;
	void onEvent(Event& event) override;

private:
	// 时间戳, 用于帧同步
	float time_ = 0.0F;
};
}  // namespace hazel