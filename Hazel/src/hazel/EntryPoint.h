#pragma once

#include "Application.h"
#include "Log.h"

#ifdef HZ_PLATFORM_WINDOWS
int main(int argc, char** argv) {
    hazel::Log::init();
    HZ_CORE_WARN("Initialized Log!");
    int a = 5;
    HZ_INFO("Hello! Var={0}", a);

    auto* app = hazel::createApplication();
    app->run();
    delete app;

    return 0;
}

#endif