#include "hazel/core/Input.h"

#ifdef HZ_PLATFORM_WINDOWS
#include "platform/windows/WindowsInput.h"
#endif

namespace hazel {

Scope<Input> Input::instance_ = Input::create();

Scope<Input> Input::create() {
#ifdef HZ_PLATFORM_WINDOWS
	return createScope<WindowsInput>();
#else
	HZ_CORE_ASSERT(false, "Unknown platform!");
	return nullptr;
#endif
}
}  // namespace hazel