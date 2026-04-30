#pragma once
#include "Core.h"

namespace hazel {
class HAZEL_API Application {
public:
    Application();
    virtual ~Application();
    void run();
};
// 应该在客户端实现
Application* createApplication();
}  // namespace hazel