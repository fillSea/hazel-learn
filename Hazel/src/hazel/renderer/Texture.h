#pragma once

#include <string>

#include "hazel/Core.h"

namespace hazel {

class HAZEL_API Texture {
public:
	Texture() = default;
	virtual ~Texture() = default;

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;

	virtual uint32_t getWidth() const = 0;
	virtual uint32_t getHeight() const = 0;

	virtual void bind(uint32_t slot = 0) const = 0;
};

class HAZEL_API Texture2D : public Texture {
public:
	static Ref<Texture2D> create(const std::string& path);
};

}  // namespace hazel