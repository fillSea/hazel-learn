#include <Hazel.h>
#include <hazel/core/EntryPoint.h>

#include "EditorLayer.h"

namespace hazel {

class Hazelnut : public Application {
public:
	Hazelnut() : Application("Hazelnut") { pushLayer(new EditorLayer()); }
	~Hazelnut() override = default;

	Hazelnut(const Hazelnut&) = delete;
	Hazelnut& operator=(const Hazelnut&) = delete;
	Hazelnut(Hazelnut&&) = delete;
	Hazelnut& operator=(Hazelnut&&) = delete;
};

Application* createApplication() {
	return new Hazelnut();
}

}  // namespace hazel