#pragma once

namespace apn::timeline_map
{
#pragma pack(push)
#pragma pack(1)
	//
	// このクラスはRGBAを保持します。
	//
	union rgba_t {
		uint32_t value; // 0xrrggbbaa;
		uint8_t vec[4];
		struct { uint8_t a, b, g, r; };
		rgba_t() {}
		rgba_t(const rgba_t& rhs) : value(rhs.value) {}
		rgba_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			set(r, g, b, a);
		}
		void set(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			this->r = r, this->g = g, this->b = b, this->a = a;
		}
		operator D2D1::ColorF() const {
			return { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
		}
	};
#pragma pack(pop)
}
