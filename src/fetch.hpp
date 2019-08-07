#pragma once
#ifndef GFX_IMAGE_FETCH_HPP
#define GFX_IMAGE_FETCH_HPP

#include "al2o3_platform/platform.h"
#include "al2o3_cmath/colour.h"
#include <numeric>

namespace Image {

template<typename type_>
auto FetchRaw(uint8_t const *ptr_) -> type_ {
	return *((type_ const *) ptr_);
}

template<typename type_>
auto FetchHomoChannel(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return (double) std::numeric_limits<type_>::max();
	return (double) FetchRaw<type_>(ptr_ + (sizeof(type_) * channel_));
}

template<typename type_>
auto FetchHomoChannel_NORM(int8_t channel_, uint8_t const *ptr_) -> double {
	return FetchHomoChannel<type_>(channel_, ptr_) / (double) std::numeric_limits<type_>::max();
}

template<typename type_>
auto FetchHomoChannel_sRGB(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	// channel 3 is never sRGB encoded
	if(channel_ == 3) {
		return FetchHomoChannel<type_>(channel_, ptr_) / (double) std::numeric_limits<type_>::max();
	} else {
		return Math_SRGB2Float(FetchRaw<type_>(ptr_ + sizeof(type_) * channel_));
	}

}

auto FetchHomoChannel_nibble(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 15.0;

	uint8_t const bite = *(ptr_ + ((int) channel_ / 2));
	return (double) ((channel_ & 0x1) ?	((bite >> 0) & 0xF) :	((bite >> 4) & 0xF));
}

auto FetchHomoChannel_nibble_UNORM(int8_t channel_, uint8_t const *ptr_) -> double {
	return FetchHomoChannel_nibble(channel_, ptr_) / 15.0;
}

auto FetchChannel_5_6_5_UNORM(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	uint16_t pixel = FetchRaw<uint16_t>(ptr_);
	if (channel_ == 0) {
		return ((double) ((pixel >> 0) & 0x1F)) / 31.0;
	} else if (channel_ == 1) {
		return ((double) ((pixel >> 5) & 0x3F)) / 63.0;
	} else if (channel_ == 2) {
		return ((double) ((pixel >> 11) & 0x1F)) / 31.0;
	}

	return 0.0;
}

auto FetchChannel_5_5_5_1_UNORM(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	uint16_t pixel = FetchRaw<uint16_t>(ptr_);

	uint32_t x = 0;
	if (channel_ == 0) {
		x = (pixel >> 0) & 0x1F;
	} else if (channel_ == 1) {
		x = (pixel >> 5) & 0x1F;
	} else if (channel_ == 2) {
		x = (pixel >> 10) & 0x1F;
	} else	if (channel_ == 3) {
		return ((double) ((pixel >> 15) & 0x1));
	}

	return ((double) (x)) / 31.0;

}

auto FetchChannel_1_5_5_5_UNORM(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	uint16_t pixel = FetchRaw<uint16_t>(ptr_);
	uint32_t x = 0;
	if (channel_ == 0) {
		return ((double) ((pixel >> 15) & 0x1));
	} else if (channel_ == 1) {
		x = (pixel >> 0) & 0x1F;
	} else if (channel_ == 2) {
		x = (pixel >> 5) & 0x1F;
	} else if (channel_ == 3) {
		x = (pixel >> 10) & 0x1F;
	}

	return ((double) (x)) / 31.0;
}

auto FetchHomoChannel_FP16(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	return (double) Math_Half2Float(FetchRaw<uint16_t>(ptr_ + (sizeof(uint16_t) * channel_)));
}

auto FetchChannel_2_10_10_10(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	// fake fetch a single 32 bit format
	uint32_t pixel = FetchRaw<uint32_t>(ptr_);

	uint32_t x = 0;
	if (channel_ == 0) {
		return ((double) ((pixel >> 30) & 0x3));
	} else if (channel_ == 1) {
		x = (pixel >> 0) & 0x3FF;
	} else if (channel_ == 2) {
		x = (pixel >> 10) & 0x3FF;
	} else if (channel_ == 3) {
		x = (pixel >> 20) & 0x3FF;
	}

	return ((double) (x));
}

auto FetchChannel_2_10_10_10_UNORM(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	if (channel_ == 0) {
		return FetchChannel_2_10_10_10(0, ptr_) / 3.0;
	} else {
		return FetchChannel_2_10_10_10(channel_, ptr_) / 1023.0;
	}
}

auto FetchChannel_8_24_UINT_UNORM(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	// fake fetch a single 32 bit format
	uint32_t pixel = FetchRaw<uint32_t>(ptr_);
	if (channel_ == 0) {
		return ((double) ((pixel & 0xFF000000) >> 24));
	} else {
		return ((double) (pixel & 0x00FFFFFF) / 16777215.0);
	}
}

auto FetchChannel_24_8_UNORM_UINT(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	uint32_t pixel = FetchRaw<uint32_t>(ptr_);
	if (channel_ == 0) {
		return ((double) ((pixel & 0xFFFFFF00) >> 8) / 16777215.0);
	} else {
		return (double) (pixel & 0x000000FF);
	}
}

auto FetchChannel_16_8_UNORM_UINT(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	if (channel_ == 0) {
		return FetchHomoChannel_NORM<uint16_t>(0, ptr_);
	} else {
		return FetchHomoChannel<uint8_t>(0, ptr_ + 2);
	}
}

auto FetchChannel_32_8_SFLOAT_UINT(int8_t channel_, uint8_t const *ptr_) -> double {
	if (channel_ == -1)
		return 0.0;
	if (channel_ == -2)
		return 1.0;

	if (channel_ == 0) {
		return FetchHomoChannel<float>(0, ptr_);
	} else {
		return FetchHomoChannel<uint8_t>(0, ptr_ + 4);
	}
}

auto CompressedChannelAt(Image_ImageHeader const *image, int8_t channel_, size_t index_) -> double {
	ASSERT(false);
	return 0.0;
}

#define SWIZ TinyImageFormat_LogicalChannelToPhysical(fmt_, channel_)
auto BitWidth256ChannelAt(TinyImageFormat_LogicalChannel const channel_,
													TinyImageFormat const fmt_,
													uint8_t const *ptr_) -> double {
	switch (fmt_) {
	case TinyImageFormat_R64G64B64A64_UINT:
		return FetchHomoChannel<uint64_t>(SWIZ, ptr_); // potentially lossy!
	case TinyImageFormat_R64G64B64A64_SINT:
		return FetchHomoChannel<int64_t>(SWIZ, ptr_); // potentially lossy!
	case TinyImageFormat_R64G64B64A64_SFLOAT:
		return FetchHomoChannel<double>(SWIZ, ptr_);
	default: break;
	}
	LOGERRORF("%s not handled by bitWidth256ChannelAt", TinyImageFormat_Name(fmt_));
	return 0.0;
}

auto BitWidth192ChannelAt(TinyImageFormat_LogicalChannel const channel_,
													TinyImageFormat const fmt_,
													uint8_t const *ptr_) -> double {
	switch (fmt_) {
	case TinyImageFormat_R64G64B64_UINT:
		return FetchHomoChannel<uint64_t>(SWIZ, ptr_); // potentially lossy!
	case TinyImageFormat_R64G64B64_SINT:
		return FetchHomoChannel<int64_t>(SWIZ, ptr_); // potentially lossy!
	case TinyImageFormat_R64G64B64_SFLOAT:
		return FetchHomoChannel<double>(SWIZ, ptr_);
	default: break;
	}
	LOGERRORF("%s not handled by bitWidth192ChannelAt", TinyImageFormat_Name(fmt_));
	return 0.0;
}

auto BitWidth128ChannelAt(TinyImageFormat_LogicalChannel const channel_,
													TinyImageFormat const fmt_,
													uint8_t const *ptr_) -> double {

	switch (fmt_) {
	case TinyImageFormat_R64G64_UINT:
		return FetchHomoChannel<uint64_t>(SWIZ,ptr_); // potentially lossy!
	case TinyImageFormat_R64G64_SINT:
		return FetchHomoChannel<int64_t>(SWIZ, ptr_); // potentially lossy!
	case TinyImageFormat_R64G64_SFLOAT:return FetchHomoChannel<double>(SWIZ, ptr_);
	case TinyImageFormat_R32G32B32A32_UINT:return FetchHomoChannel<uint32_t>(SWIZ, ptr_);
	case TinyImageFormat_R32G32B32A32_SINT:return FetchHomoChannel<int32_t>(SWIZ, ptr_);
	case TinyImageFormat_R32G32B32A32_SFLOAT:return FetchHomoChannel<float>(SWIZ, ptr_);
	default: break;
	}
	LOGERRORF("%s not handled by bitWidth128ChannelAt", TinyImageFormat_Name(fmt_));
	return 0.0;
}

auto BitWidth96ChannelAt(TinyImageFormat_LogicalChannel const channel_,
												 TinyImageFormat const fmt_,
												 uint8_t const *ptr_) -> double {
	switch (fmt_) {
	case TinyImageFormat_R32G32B32_UINT:return FetchHomoChannel<uint32_t>(SWIZ, ptr_);
	case TinyImageFormat_R32G32B32_SINT:return FetchHomoChannel<int32_t>(SWIZ, ptr_);
	case TinyImageFormat_R32G32B32_SFLOAT:return FetchHomoChannel<float>(SWIZ, ptr_);
	default:break;
	}

	LOGERRORF("%s not handled by bitWidth96ChannelAt", TinyImageFormat_Name(fmt_));
	return 0.0;
}

auto BitWidth64ChannelAt(TinyImageFormat_LogicalChannel const channel_,
												 TinyImageFormat const fmt_,
												 uint8_t const *ptr_) -> double {
	switch (fmt_) {
	case TinyImageFormat_R64_UINT:
		return FetchHomoChannel<uint64_t>(SWIZ, ptr_); // potentially lossy!
	case TinyImageFormat_R64_SINT:
		return FetchHomoChannel<int64_t>(SWIZ, ptr_); // potentially lossy!
	case TinyImageFormat_R64_SFLOAT:return FetchHomoChannel<double>(SWIZ, ptr_);
	case TinyImageFormat_R32G32_UINT:return FetchHomoChannel<uint32_t>(SWIZ, ptr_);
	case TinyImageFormat_R32G32_SINT:return FetchHomoChannel<int32_t>(SWIZ, ptr_);
	case TinyImageFormat_R32G32_SFLOAT:return FetchHomoChannel<float>(SWIZ, ptr_);
	case TinyImageFormat_R16G16B16A16_UNORM:
		return FetchHomoChannel_NORM<uint16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16B16A16_SNORM:
		return FetchHomoChannel_NORM<int16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16B16A16_UINT:
		return FetchHomoChannel<uint16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16B16A16_SINT:
		return FetchHomoChannel<int16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16B16A16_SFLOAT:
		return FetchHomoChannel_FP16(SWIZ, ptr_);
	case TinyImageFormat_D32_SFLOAT_S8_UINT:
		return FetchChannel_32_8_SFLOAT_UINT(SWIZ, ptr_);

	default: break;
	}
	LOGERRORF("%s not handled by bitWidth64ChannelAt", TinyImageFormat_Name(fmt_));
	return 0.0;
}

auto BitWidth48ChannelAt(TinyImageFormat_LogicalChannel const channel_,
												 TinyImageFormat const fmt_,
												 uint8_t const *ptr_) -> double {
	switch (fmt_) {
	case TinyImageFormat_R16G16B16_UNORM:
		return FetchHomoChannel_NORM<uint16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16B16_SNORM:
		return FetchHomoChannel_NORM<int16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16B16_UINT:
		return FetchHomoChannel<uint16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16B16_SINT:
		return FetchHomoChannel<int16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16B16_SFLOAT:
		return FetchHomoChannel_FP16(SWIZ, ptr_);

	default: break;
	}
	LOGERRORF("%s not handled by bitWidth48ChannelAt", TinyImageFormat_Name(fmt_));
	return 0.0;
}

auto BitWidth32ChannelAt(TinyImageFormat_LogicalChannel const channel_,
												 TinyImageFormat const fmt_,
												 uint8_t const *ptr_) -> double {
	switch (fmt_) {
	case TinyImageFormat_R32_UINT:return FetchHomoChannel<uint32_t>(SWIZ, ptr_);
	case TinyImageFormat_R32_SINT:return FetchHomoChannel<int32_t>(SWIZ, ptr_);
	case TinyImageFormat_D32_SFLOAT:
	case TinyImageFormat_R32_SFLOAT:return FetchHomoChannel<float>(SWIZ, ptr_);
	case TinyImageFormat_R16G16_UNORM:return FetchHomoChannel_NORM<uint16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16_SNORM:return FetchHomoChannel_NORM<int16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16_UINT:
		return FetchHomoChannel<uint16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16_SINT:
		return FetchHomoChannel<int16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16G16_SFLOAT:return FetchHomoChannel_FP16(SWIZ, ptr_);

	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_B8G8R8A8_UNORM:
	case TinyImageFormat_R8G8B8X8_UNORM:
	case TinyImageFormat_B8G8R8X8_UNORM:
		return FetchHomoChannel_NORM<uint8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8B8A8_SNORM:
	case TinyImageFormat_B8G8R8A8_SNORM:
		return FetchHomoChannel_NORM<int8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8B8A8_UINT:
	case TinyImageFormat_B8G8R8A8_UINT:
		return FetchHomoChannel<uint8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8B8A8_SINT:
	case TinyImageFormat_B8G8R8A8_SINT:
		return FetchHomoChannel<int8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8B8A8_SRGB:
	case TinyImageFormat_B8G8R8A8_SRGB:
		return FetchHomoChannel_sRGB<uint8_t>(SWIZ, ptr_);

	case TinyImageFormat_A2R10G10B10_UNORM:
	case TinyImageFormat_A2B10G10R10_UNORM:
		return FetchChannel_2_10_10_10_UNORM(SWIZ, ptr_);
	case TinyImageFormat_A2R10G10B10_UINT:
	case TinyImageFormat_A2B10G10R10_UINT:
		return FetchChannel_2_10_10_10(SWIZ, ptr_);
	case TinyImageFormat_X8_D24_UNORM:return FetchChannel_8_24_UINT_UNORM(SWIZ, ptr_);
	case TinyImageFormat_D24_UNORM_S8_UINT:return FetchChannel_24_8_UNORM_UINT(SWIZ, ptr_);

	case TinyImageFormat_B10G11R11_UFLOAT:break;
	case TinyImageFormat_E5B9G9R9_UFLOAT:break;

	default: break;
	}
	LOGERRORF("%s not handled by bitWidth32ChannelAt", TinyImageFormat_Name(fmt_));
	return 0.0;
}

auto BitWidth24ChannelAt(TinyImageFormat_LogicalChannel const channel_,
												 TinyImageFormat const fmt_,
												 uint8_t const *ptr_) -> double {
	switch (fmt_) {
	case TinyImageFormat_R8G8B8_UNORM:
	case TinyImageFormat_B8G8R8_UNORM: return FetchHomoChannel_NORM<uint8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8B8_SNORM:
	case TinyImageFormat_B8G8R8_SNORM: return FetchHomoChannel_NORM<int8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8B8_UINT:
	case TinyImageFormat_B8G8R8_UINT: return FetchHomoChannel<uint8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8B8_SINT:
	case TinyImageFormat_B8G8R8_SINT: return FetchHomoChannel<int8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8B8_SRGB:
	case TinyImageFormat_B8G8R8_SRGB: return FetchHomoChannel_sRGB<uint8_t>(SWIZ, ptr_);

	case TinyImageFormat_D16_UNORM_S8_UINT:return FetchChannel_16_8_UNORM_UINT(SWIZ, ptr_);

	default: break;
	}

	LOGERRORF("%s not handled by bitWidth24ChannelAt", TinyImageFormat_Name(fmt_));
	return 0.0;
}

auto BitWidth16ChannelAt(TinyImageFormat_LogicalChannel const channel_,
												 TinyImageFormat const fmt_,
												 uint8_t const *ptr_) -> double {
	switch (fmt_) {
	case TinyImageFormat_R4G4B4A4_UNORM:
	case TinyImageFormat_B4G4R4A4_UNORM:
	case TinyImageFormat_A4R4G4B4_UNORM:
	case TinyImageFormat_A4B4G4R4_UNORM:
	case TinyImageFormat_X4R4G4B4_UNORM:
	case TinyImageFormat_X4B4G4R4_UNORM:
		return FetchHomoChannel_nibble_UNORM(SWIZ, ptr_);
	case TinyImageFormat_R5G6B5_UNORM:
	case TinyImageFormat_B5G6R5_UNORM:
		return FetchChannel_5_6_5_UNORM(SWIZ, ptr_);
	case TinyImageFormat_R5G5B5A1_UNORM:
	case TinyImageFormat_B5G5R5A1_UNORM:
	case TinyImageFormat_R5G5B5X1_UNORM:
	case TinyImageFormat_B5G5R5X1_UNORM:
		return FetchChannel_5_5_5_1_UNORM(SWIZ, ptr_);

	case TinyImageFormat_A1R5G5B5_UNORM:
	case TinyImageFormat_A1B5G5R5_UNORM:
	case TinyImageFormat_X1R5G5B5_UNORM:
	case TinyImageFormat_X1B5G5R5_UNORM:
		return FetchChannel_1_5_5_5_UNORM(SWIZ, ptr_);

	case TinyImageFormat_R8G8_UNORM:return FetchHomoChannel_NORM<uint8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8_SNORM:return FetchHomoChannel_NORM<int8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8_UINT:return FetchHomoChannel<uint8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8_SINT:return FetchHomoChannel<int8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8G8_SRGB:return FetchHomoChannel_sRGB<uint8_t>(SWIZ, ptr_);
	case TinyImageFormat_D16_UNORM:
	case TinyImageFormat_R16_UNORM:return FetchHomoChannel_NORM<uint16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16_SNORM:return FetchHomoChannel_NORM<int16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16_UINT:
		return FetchHomoChannel<uint16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16_SINT:
		return FetchHomoChannel<int16_t>(SWIZ, ptr_);
	case TinyImageFormat_R16_SFLOAT:return FetchHomoChannel_FP16(SWIZ, ptr_);
	default: break;
	}

	LOGERRORF("%s not handled by bitWidth16ChannelAt", TinyImageFormat_Name(fmt_));
	return 0.0;
}

auto BitWidth8ChannelAt(TinyImageFormat_LogicalChannel const channel_,
												TinyImageFormat const fmt_,
												uint8_t const *ptr_) -> double {
	switch (fmt_) {
	case TinyImageFormat_R4G4_UNORM:
		return FetchHomoChannel_nibble_UNORM(SWIZ, ptr_);
	case TinyImageFormat_A8_UNORM:
	case TinyImageFormat_R8_UNORM: return FetchHomoChannel_NORM<uint8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8_SNORM: return FetchHomoChannel_NORM<int8_t>(SWIZ, ptr_);
	case TinyImageFormat_S8_UINT:
	case TinyImageFormat_R8_UINT: return FetchHomoChannel<uint8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8_SINT: return FetchHomoChannel<int8_t>(SWIZ, ptr_);
	case TinyImageFormat_R8_SRGB: return FetchHomoChannel_sRGB<uint8_t>(SWIZ, ptr_);

	default:LOGERRORF("%s not handled by bitWidth8ChannelAt", TinyImageFormat_Name(fmt_));
		return 0.0;
	}
}
#undef SWIZ

} // end image namespace

#endif //WYRD_FETCH_HPP
