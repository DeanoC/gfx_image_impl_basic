#pragma once
#ifndef GFX_IMAGE_PUT_HPP
#define GFX_IMAGE_PUT_HPP

#include "al2o3_platform/platform.h"
#include "al2o3_cmath/scalar.h"
#include "al2o3_cmath/scalar.hpp"
#include "fetch.hpp"
#include <numeric>
#include <limits>

namespace Image {

// Puts are always in channel number NOT the name i.e. channel 0 is always
// the rightmost channel whethers its holding Alpha or Red etc.
// swizzling we have moved everything to the right place

template<typename type_>
auto PutRaw(uint8_t *ptr_, type_ const value_) -> void {
  type_ const v = Math::Clamp(value_, std::numeric_limits<type_>::min(), std::numeric_limits<type_>::max());

  *((type_ *) ptr_) = v;
}

template<typename type_>
auto PutHomoChannel(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
	PutRaw<type_>(ptr_ + (sizeof(type_) * channel_), (type_) value_);
}

template<typename type_>
auto PutHomoChannel_NORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
	PutHomoChannel<type_>(channel_, ptr_, value_ * (double) std::numeric_limits<type_>::max());
}

template<typename type_>
auto PutHomoChannel_sRGB(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
	PutHomoChannel<type_>(channel_, ptr_, Math_Float2SRGB((float) value_));
}

auto PutHomoChannel_nibble(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
	// fetch the byte to merge the nibble into
	uint8_t bite = ptr_[channel_ / 2];
	uint8_t nibble = (uint8_t) Math_ClampD(value_, 0.0, 15.0);
	bite = (channel_ & uint8_t(0x1u)) ?
				 (bite & uint8_t(0xF0u)) | (nibble << 0u) :
				 (bite & uint8_t(0x0Fu)) | (nibble << 4u);
	ptr_[channel_ / 2] = bite;
}

auto PutHomoChannel_nibble_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
	PutHomoChannel_nibble(channel_, ptr_, value_ * 15.0);
}

auto PutChannel_5_6_5_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
	auto pixel = FetchRaw<uint16_t>(ptr_);
  if (channel_ == 0) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
		pixel = (pixel & uint16_t(0xFFE0u)) | (uint16_t) v << 0u;
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_ * 63.0, 0.0, 63.0);
    pixel = (pixel & uint16_t(0xF81Fu)) | (uint16_t) v << 5u;
  } else if (channel_ == 2) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
		pixel = (pixel & uint16_t(0x07FF)) | (uint16_t) v << 11u;
  } else {
    ASSERT(channel_ < 3);
  }

  PutRaw(ptr_, pixel);
}

auto PutChannel_1_5_5_5_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint16_t>(ptr_);

  if (channel_ == 0) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & ~0x001Fu) | ((uint16_t) round(v)) << 0u;
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & ~0x03E0u) | ((uint16_t) round(v)) << 5u;
  } else if (channel_ == 2) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & ~0x7C00u) | ((uint16_t) round(v)) << 10u;
  } else if (channel_ == 3) {
    double const v = Math_ClampD(value_, 0.0, 1.0);
    pixel = (pixel & ~0x8000) | ((uint16_t) round(v)) << 15u;
  } else {
    ASSERT(channel_ < 4);
  }
  PutRaw(ptr_, pixel);
}

auto PutChannel_5_5_5_1_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
	auto pixel = FetchRaw<uint16_t>(ptr_);
	if (channel_ == 0) {
		double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
		pixel = (pixel & ~0x001Fu) | ((uint16_t) v) << 0u;
	} else if (channel_ == 1) {
		double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
		pixel = (pixel & ~0x03E0u) | ((uint16_t) v) << 5u;
	} else if (channel_ == 2) {
		double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
		pixel = (pixel & ~0x7C00u) | ((uint16_t) v) << 10u;
	} else if (channel_ == 3) {
		double const v = Math_ClampD(value_, 0.0, 1.0);
		pixel = (pixel & ~0x8000) | (uint16_t) v << 15u;
	}

		PutRaw(ptr_, pixel);
}

auto PutHomoChannel_FP16(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  // clamp half?
  uint16_t h = Math_Float2Half((float) value_);
  PutRaw(ptr_ + (sizeof(uint16_t) * channel_), h);
}

auto PutChannel_2_10_10_10(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint32_t>(ptr_);
  if (channel_ == 0) {
    double const v = Math_ClampD(value_, 0.0, 3.0);
    pixel = (pixel & 0x3FFFFFFFu) | (uint32_t) v << 30u;
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_, 0.0, 1023.0);
    pixel = (pixel & 0xC00FFFFFu) | (uint32_t) v << 20u;
  } else if (channel_ == 2) {
    double const v = Math_ClampD(value_, 0.0, 1023.0);
    pixel = (pixel & 0xFFF003FFu) | (uint32_t) v << 10u;
  } else if (channel_ == 3) {
    double const v = Math_ClampD(value_, 0.0, 1023.0);
    pixel = (pixel & 0xFFFFFC00u) | (uint32_t) v << 0u;
  }

  PutRaw(ptr_, pixel);

}

auto PutChannel_2_10_10_10_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
	if (channel_ == 0) { PutChannel_2_10_10_10(channel_, ptr_, value_ * 3.0); }
  else { PutChannel_2_10_10_10(channel_, ptr_, value_ * 1023.0); }
}

auto PutChannel_8_24_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint32_t>(ptr_);
  if (channel_ == 0) {
    double const v = Math_ClampD(value_ * 255.0, 0.0, 255.0);
    pixel = (pixel & 0x00FFFFFFu) | (uint32_t) v << 24u;
  } else if (channel_ == 1) {
    static const double Max24Bit = double(1ul << 24ul) - 1.0;
    double const v = Math_ClampD(value_ * Max24Bit, 0.0, Max24Bit);
    pixel = (pixel & 0xFF000000u) | (uint32_t) v << 0u;
  } else {
    ASSERT(channel_ < 2);
  }
  PutRaw(ptr_, pixel);
}

auto PutChannel_24_8_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint32_t>(ptr_);
  if (channel_ == 0) {
    static const double Max24Bit = double(1u << 24u) - 1.0;
    double const v = Math_ClampD(value_ * Max24Bit, 0.0, Max24Bit);
    pixel = (pixel & 0x000000FFu) | (uint32_t) v << 8u;
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_ * 255.0, 0.0, 255.0);
    pixel = (pixel & 0xFFFFFF00u) | (uint32_t) v << 0u;
  } else {
    ASSERT(channel_ < 2);
  }
  PutRaw(ptr_, pixel);
}

auto PutChannel_16_8_UNORM_UINT(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  if (channel_ == 0) {
    double const v = Math_ClampD(value_ * 65535.0, 0.0, 65535.0);
    PutHomoChannel<uint16_t>(0, ptr_, (uint16_t) v);
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_, 0.0, 255.0);
    PutHomoChannel<uint16_t>(0, ptr_ + 2, (uint8_t) v);
  } else {
    ASSERT(channel_ < 2);
  }

}

auto BitWidth256SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                             enum TinyImageFormat const fmt_,
                             uint8_t *ptr_,
                             double const value_) -> void {
  switch (fmt_) {
    case TinyImageFormat_R64G64B64A64_UINT:PutHomoChannel<uint64_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R64G64B64A64_SINT:PutHomoChannel<int64_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R64G64B64A64_SFLOAT:PutHomoChannel<double>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }
}

auto BitWidth192SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                             enum TinyImageFormat const fmt_,
                             uint8_t *ptr_,
                             double const value_) -> void {
  switch (fmt_) {
    case TinyImageFormat_R64G64B64_UINT:PutHomoChannel<uint64_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R64G64B64_SINT:PutHomoChannel<int64_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R64G64B64_SFLOAT:PutHomoChannel<double>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }
}

auto BitWidth128SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                             enum TinyImageFormat const fmt_,
                             uint8_t *ptr_,
                             double const value_) -> void {

  switch (fmt_) {
    case TinyImageFormat_R64G64_UINT:PutHomoChannel<uint64_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R64G64_SINT:PutHomoChannel<int64_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R64G64_SFLOAT:PutHomoChannel<double>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R32G32B32A32_UINT:PutHomoChannel<uint32_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R32G32B32A32_SINT:PutHomoChannel<int32_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R32G32B32A32_SFLOAT:PutHomoChannel<float>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }
}

auto BitWidth96SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                            enum TinyImageFormat const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case TinyImageFormat_R32G32B32_UINT:PutHomoChannel<uint32_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R32G32B32_SINT:PutHomoChannel<int32_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R32G32B32_SFLOAT:PutHomoChannel<float>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }

}

auto BitWidth64SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                            enum TinyImageFormat const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case TinyImageFormat_R64_UINT:
      PutHomoChannel<uint64_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                               ptr_,
                               value_); // potentially lossy!
      break;
    case TinyImageFormat_R64_SINT:
      PutHomoChannel<int64_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                              ptr_,
                              value_); // potentially lossy!
      break;
    case TinyImageFormat_R64_SFLOAT: PutHomoChannel<double>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R32G32_UINT: PutHomoChannel<uint32_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R32G32_SINT: PutHomoChannel<int32_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R32G32_SFLOAT: PutHomoChannel<float>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R16G16B16A16_UNORM:
      PutHomoChannel_NORM<uint16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                                    ptr_,
                                    value_);
      break;
    case TinyImageFormat_R16G16B16A16_SNORM:
      PutHomoChannel_NORM<int16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                                   ptr_,
                                   value_);
      break;
    case TinyImageFormat_R16G16B16A16_UINT:
      PutHomoChannel<uint16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                               ptr_,
                               value_);
      break;
    case TinyImageFormat_R16G16B16A16_SINT:
      PutHomoChannel<int16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                              ptr_,
                              value_);
      break;
    case TinyImageFormat_R16G16B16A16_SFLOAT: PutHomoChannel_FP16(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_D32_SFLOAT_S8_UINT:
      if (channel_ == TinyImageFormat_LC_Depth) {
        PutHomoChannel<float>(0, ptr_, value_);
      } else {
        PutHomoChannel<uint8_t>(0, ptr_ + 4, value_);
      }
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }

}

auto BitWidth48SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                            enum TinyImageFormat const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case TinyImageFormat_R16G16B16_UNORM:
      PutHomoChannel_NORM<uint16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                                    ptr_,
                                    value_);
      break;
    case TinyImageFormat_R16G16B16_SNORM:
      PutHomoChannel_NORM<int16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                                   ptr_,
                                   value_);
      break;
    case TinyImageFormat_R16G16B16_UINT:
    	PutHomoChannel<uint16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R16G16B16_SINT:
    	PutHomoChannel<int16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R16G16B16_SFLOAT: PutHomoChannel_FP16(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }
}

auto BitWidth32SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                            enum TinyImageFormat const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case TinyImageFormat_R32_UINT: PutHomoChannel<uint32_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R32_SINT: PutHomoChannel<int32_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_D32_SFLOAT:
    case TinyImageFormat_R32_SFLOAT: PutHomoChannel<float>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R16G16_UNORM: PutHomoChannel_NORM<uint16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R16G16_SNORM: PutHomoChannel_NORM<int16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R16G16_UINT:
    	PutHomoChannel<uint16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R16G16_SINT:
    	PutHomoChannel<int16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R16G16_SFLOAT: PutHomoChannel_FP16(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8G8B8A8_UNORM:
		case TinyImageFormat_B8G8R8A8_UNORM:
    	PutHomoChannel_NORM<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8G8B8A8_SNORM:
		case TinyImageFormat_B8G8R8A8_SNORM:
			PutHomoChannel_NORM<int8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8G8B8A8_UINT:
		case TinyImageFormat_B8G8R8A8_UINT:
			PutHomoChannel<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8G8B8A8_SINT:
		case TinyImageFormat_B8G8R8A8_SINT:
			PutHomoChannel<int8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8G8B8A8_SRGB:
		case TinyImageFormat_B8G8R8A8_SRGB:
      if (channel_ == TinyImageFormat_LC_Alpha) {
        PutHomoChannel<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      } else {
        PutHomoChannel_sRGB<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      }
      break;
    case TinyImageFormat_A2R10G10B10_UNORM:
		case TinyImageFormat_A2B10G10R10_UNORM:
      PutChannel_2_10_10_10_UNORM(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                                   ptr_,
                                   value_);
      break;
    case TinyImageFormat_A2R10G10B10_UINT:
		case TinyImageFormat_A2B10G10R10_UINT:
      PutChannel_2_10_10_10(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                             ptr_,
                             value_);
      break;
    case TinyImageFormat_X8_D24_UNORM: PutChannel_8_24_UNORM(channel_, ptr_, value_);
      break;
    case TinyImageFormat_D24_UNORM_S8_UINT: PutChannel_24_8_UNORM(channel_, ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }
}

auto BitWidth24SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                            enum TinyImageFormat const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case TinyImageFormat_R8G8B8_UNORM:
		case TinyImageFormat_B8G8R8_UNORM:
			PutHomoChannel_NORM<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8G8B8_SNORM:
		case TinyImageFormat_B8G8R8_SNORM:
			PutHomoChannel_NORM<int8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8G8B8_UINT:
		case TinyImageFormat_B8G8R8_UINT:
			PutHomoChannel<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8G8B8_SINT:
		case TinyImageFormat_B8G8R8_SINT:
			PutHomoChannel<int8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8G8B8_SRGB:
		case TinyImageFormat_B8G8R8_SRGB:
    	PutHomoChannel_sRGB<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
      case TinyImageFormat_D16_UNORM_S8_UINT: PutChannel_16_8_UNORM_UINT(channel_, ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }

}

auto BitWidth16SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                            enum TinyImageFormat const fmt_,
                            uint8_t *ptr_,
                            double const v) -> void {
  switch (fmt_) {
    case TinyImageFormat_R4G4B4A4_UNORM:
		case TinyImageFormat_B4G4R4A4_UNORM:
		case TinyImageFormat_A4R4G4B4_UNORM:
      PutHomoChannel_nibble_UNORM(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
																	ptr_,
																	v);
      break;

    case TinyImageFormat_R5G6B5_UNORM:
    case TinyImageFormat_B5G6R5_UNORM:
    	PutChannel_5_6_5_UNORM(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_R5G5B5A1_UNORM:
    case TinyImageFormat_B5G5R5A1_UNORM:
      PutChannel_5_5_5_1_UNORM(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
															 ptr_,
															 v);
      break;
    case TinyImageFormat_A1R5G5B5_UNORM:
      PutChannel_1_5_5_5_UNORM(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;

    case TinyImageFormat_R8G8_UNORM: PutHomoChannel_NORM<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_R8G8_SNORM: PutHomoChannel_NORM<int8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_R8G8_UINT: PutHomoChannel<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_R8G8_SINT: PutHomoChannel<int8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_R8G8_SRGB: PutHomoChannel_sRGB<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_D16_UNORM:
    case TinyImageFormat_R16_UNORM: PutHomoChannel_NORM<uint16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_R16_SNORM: PutHomoChannel_NORM<int16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_R16_UINT:
    	PutHomoChannel<uint16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_R16_SINT:
    	PutHomoChannel<int16_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    case TinyImageFormat_R16_SFLOAT:
    	PutHomoChannel_FP16(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, v);
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }

}

auto BitWidth8SetChannelAt(TinyImageFormat_LogicalChannel const channel_,
                           enum TinyImageFormat const fmt_,
                           uint8_t *ptr_,
                           double const value_) -> void {
  switch (fmt_) {
    case TinyImageFormat_R4G4_UNORM:
      PutHomoChannel_nibble_UNORM(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_),
                                  ptr_,
                                  value_);
      break;
    case TinyImageFormat_R8_UNORM:
		case TinyImageFormat_A8_UNORM:
			PutHomoChannel_NORM<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8_SNORM: PutHomoChannel_NORM<int8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_S8_UINT:
    case TinyImageFormat_R8_UINT: PutHomoChannel<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8_SINT: PutHomoChannel<int8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    case TinyImageFormat_R8_SRGB: PutHomoChannel_sRGB<uint8_t>(TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", TinyImageFormat_Name(fmt_));
  }

}

} // end Image namespace

#endif //WYRD_PUT_HPP
