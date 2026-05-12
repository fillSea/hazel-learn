#include "LayerStack.h"

#include <algorithm>

namespace hazel {
LayerStack::LayerStack() {
	layer_insert_ = layers_.begin();
}

LayerStack::~LayerStack() {
	for (Layer* layer : layers_) {
		delete layer;
	}
}

void LayerStack::pushLayer(Layer* layer) {
	layer_insert_ = layers_.emplace(layer_insert_, layer);
}

void LayerStack::pushOverlay(Layer* overlay) {
	layers_.emplace_back(overlay);
}

void LayerStack::popLayer(Layer* layer) {
	auto it = std::ranges::find(layers_, layer);
	if (it != layers_.end()) {
		layers_.erase(it);
		layer_insert_--;
	}
}

void LayerStack::popOverlay(Layer* overlay) {
	auto it = std::ranges::find(layers_, overlay);
	if (it != layers_.end()) {
		layers_.erase(it);
	}
}
}  // namespace hazel