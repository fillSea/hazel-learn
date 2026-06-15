#include "LayerStack.h"

#include <algorithm>

namespace hazel {
LayerStack::LayerStack() {}

LayerStack::~LayerStack() {
	for (Layer* layer : layers_) {
		delete layer;
	}
}

void LayerStack::pushLayer(Layer* layer) {
	layers_.emplace(layers_.begin() + layer_insert_index_, layer);
	layer->onAttach();
	layer_insert_index_++;
}

void LayerStack::pushOverlay(Layer* overlay) {
	layers_.emplace_back(overlay);
	overlay->onAttach();
}

void LayerStack::popLayer(Layer* layer) {
	auto it = std::ranges::find(layers_, layer);
	if (it != layers_.end()) {
		layers_.erase(it);
		layer_insert_index_--;
		layer->onDetach();
	}
}

void LayerStack::popOverlay(Layer* overlay) {
	auto it = std::ranges::find(layers_, overlay);
	if (it != layers_.end()) {
		layers_.erase(it);
		overlay->onDetach();
	}
}
}  // namespace hazel