#include "LayerStack.h"

#include <algorithm>

namespace hazel {
LayerStack::LayerStack() = default;

LayerStack::~LayerStack() {
	for (Layer* layer : layers_) {
		layer->onDetach();
		delete layer;
	}
}

void LayerStack::pushLayer(Layer* layer) {
	layers_.emplace(layers_.begin() + layer_insert_index_, layer);
	layer_insert_index_++;
}

void LayerStack::pushOverlay(Layer* overlay) {
	layers_.emplace_back(overlay);
}

void LayerStack::popLayer(Layer* layer) {
	auto it = std::ranges::find(layers_, layer);
	if (it != layers_.end()) {
		layer->onDetach();
		layers_.erase(it);
		layer_insert_index_--;
	}
}

void LayerStack::popOverlay(Layer* overlay) {
	auto it = std::ranges::find(layers_, overlay);
	if (it != layers_.end()) {
		overlay->onDetach();
		layers_.erase(it);
	}
}
}  // namespace hazel