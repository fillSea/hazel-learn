#include <Hazel.h>
#include <hazel/core/EntryPoint.h>

#include "EditorLayer.h"

namespace hazel {

class HazelEditor : public Application {
public:
	HazelEditor() : Application("Hazel Editor") { pushLayer(new EditorLayer()); }
	~HazelEditor() override = default;

	HazelEditor(const HazelEditor&) = delete;
	HazelEditor& operator=(const HazelEditor&) = delete;
	HazelEditor(HazelEditor&&) = delete;
	HazelEditor& operator=(HazelEditor&&) = delete;
};

hazel::Application* hazel::createApplication() {
	return new HazelEditor();
}

}  // namespace hazel