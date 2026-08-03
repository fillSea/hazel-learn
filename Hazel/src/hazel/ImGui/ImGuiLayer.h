#pragma once

#include "hazel/core/Layer.h"

namespace hazel {
class HAZEL_API ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer() override = default;

	ImGuiLayer(const ImGuiLayer&) = delete;
	ImGuiLayer& operator=(const ImGuiLayer&) = delete;
	ImGuiLayer(ImGuiLayer&&) = delete;
	ImGuiLayer& operator=(ImGuiLayer&&) = delete;

	void onAttach() override;
	void onDetach() override;
	void onImGuiRender() override;

	void begin();
	void end();

private:
	// 时间戳, 用于帧同步
	float time_ = 0.0F;
};
}  // namespace hazel