#pragma once

#include "application.h"
#include "log.h"

#ifdef HZ_PLATFORM_WINDOWS
int main(int argc, char** argv) {
    Hazel::Log::init();
    HZ_CORE_WARN("Initialized Log!");
    int a = 5;
    HZ_INFO("Hello! Var={0}", a);

    auto* app = Hazel::createApplication();
    app->run();
    delete app;

    return 0;
}

#endif