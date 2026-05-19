#pragma once

#include "hazel/Core.h"
#include "imgui.h"

namespace hazel {
class HAZEL_API KeyUtil {
public:
	/**
	 * @brief 将GLFW按键码转换为ImGui按键码的静态函数
	 * @param key GLFW按键码
	 * @return 对应的ImGui按键码
	 */
	static ImGuiKey gLFWKeyToImGuiKey(int key);
};
}  // namespace hazel