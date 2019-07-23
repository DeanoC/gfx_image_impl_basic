#include "al2o3_platform/platform.h"
#include "gfx_imageformat/format.h"
#include "gfx_imageformat/formatcracker.h"
#include "gfx_image/image.h"
#include "al2o3_memory/memory.h"

AL2O3_EXTERN_C Image_ImageHeader const *Image_Create(uint32_t width,
                                         uint32_t height,
                                         uint32_t depth,
                                         uint32_t slices,
                                         enum ImageFormat format) {
  auto image = Image_CreateNoClear(width, height, depth, slices, format);
  if (image) {
    memset(Image_RawDataPtr(image), 0, image->dataSize);
  }

  return image;
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_CreateNoClear(uint32_t width,
                                                uint32_t height,
                                                uint32_t depth,
                                                uint32_t slices,
                                                enum ImageFormat format) {
	if(width == 0)return nullptr;
  if(height == 0) height = 1;
  if(depth == 0) depth = 1;
  if(slices == 0) slices = 1;

	Image_ImageHeader tmp;
	Image_FillHeader(width, height, depth, slices, format, &tmp);

  auto *image = (Image_ImageHeader *) MEMORY_MALLOC(sizeof(Image_ImageHeader) + tmp.dataSize);
  if (!image) { return nullptr; }

  memcpy(image, &tmp, sizeof(Image_ImageHeader));

  return image;
}
AL2O3_EXTERN_C void Image_FillHeader(uint32_t width,
                               uint32_t height,
                               uint32_t depth,
                               uint32_t slices,
                               enum ImageFormat format,
                               Image_ImageHeader *header) {


  if(ImageFormat_IsCompressed(format)) {
  	uint32_t const blockW = ImageFormat_WidthOfBlock(format);
		uint32_t const blockH = ImageFormat_HeightOfBlock(format);

		// smallest sized a block compressed texture can have is hte block size
		width = Math_MaxU32(width, blockW);
		height = Math_MaxU32(height, blockH);

		// round up to block size
  	width = (width + (blockW-1)) & ~(blockW-1);
		height = (height + (blockH-1)) & ~(blockH-1);

		uint64_t const pixelCount = width * height * depth * slices;
		uint32_t const blockBitSize = ImageFormat_BitSizeOfBlock(format);
		header->dataSize = (pixelCount * blockBitSize) / (blockW * blockH * 8);
	} else {
		uint64_t const pixelCount = width * height * depth * slices;
		header->dataSize = (pixelCount * ImageFormat_BitWidth(format)) / 8;
  }

  header->width = width;
	header->height = height;
  header->depth = depth;
  header->slices = slices;
  header->format = format;
  header->flags = 0;
  header->nextType = Image_NT_None;
  header->nextImage = nullptr;
}

AL2O3_EXTERN_C Image_ImageHeader const* Image_CreateHeaderOnly(	uint32_t width,
																																 uint32_t height,
																																 uint32_t depth,
																																 uint32_t slices,
																																 enum ImageFormat format) {
	auto *image = (Image_ImageHeader *) MEMORY_MALLOC(sizeof(Image_ImageHeader));
	if (!image) { return nullptr; }
	Image_FillHeader(width, height, depth, slices, format, image);

	return image;
}

AL2O3_EXTERN_C void Image_Destroy(Image_ImageHeader const *image) {
  // recursively free next chain
  switch (image->nextType) {
    case Image_NT_MipMaps:
    case Image_NT_Layers: {
      if (image->nextImage != nullptr) {
        Image_Destroy(image->nextImage);
      }
    }
      break;
    default:
    case Image_NT_None:break;
  }
  MEMORY_FREE((Image_ImageHeader*)image);
}

// we include fetch after swizzle so hopefully the compiler will inline it...
AL2O3_EXTERN_C inline enum Image_Channel Image_Channel_Swizzle(enum ImageFormat format, enum Image_Channel channel) {
  Image_Swizzle swizzler = ImageFormat_Swizzle(format);
  return (enum Image_Channel) swizzler[channel];
}

#include "fetch.hpp"
#include "put.hpp"

AL2O3_EXTERN_C double Image_GetChannelAt(Image_ImageHeader const *image, enum Image_Channel channel, size_t index) {
  ASSERT(image);

  using namespace Image;
  // seperate out the block compressed format first
  if (ImageFormat_IsCompressed(image->format)) {
    return Image::CompressedChannelAt(image, channel, index);
  }

  // split into bit width grouped formats
  ASSERT(ImageFormat_BitWidth(image->format) >= 8);

  uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
      index * (ImageFormat_BitWidth(image->format) / 8);

  switch (ImageFormat_BitWidth(image->format)) {
    case 256:return BitWidth256ChannelAt(channel, image->format, pixelPtr);
    case 192:return BitWidth192ChannelAt(channel, image->format, pixelPtr);
    case 128:return BitWidth128ChannelAt(channel, image->format, pixelPtr);
    case 96:return BitWidth96ChannelAt(channel, image->format, pixelPtr);
    case 64:return BitWidth64ChannelAt(channel, image->format, pixelPtr);
    case 48:return BitWidth48ChannelAt(channel, image->format, pixelPtr);
    case 32:return BitWidth32ChannelAt(channel, image->format, pixelPtr);
    case 24:return BitWidth24ChannelAt(channel, image->format, pixelPtr);
    case 16:return BitWidth16ChannelAt(channel, image->format, pixelPtr);
    case 8:return BitWidth8ChannelAt(channel, image->format, pixelPtr);
    default:LOGERROR("Bitwidth of format not supported");
      return 0.0;
  }

}
AL2O3_EXTERN_C size_t Image_LinkedImageCountOf(Image_ImageHeader const *image) {
  size_t count = 1;

  while (image && image->nextImage != nullptr) {
    count++;
    image = image->nextImage;
  }

  return count;
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_LinkedImageOf(Image_ImageHeader const *image, size_t const index) {
  size_t count = 0;

  do {
    if (count == index) {
      return image;
    }
    count++;
    image = image->nextImage;
  }  while (image);

return nullptr;
}

AL2O3_EXTERN_C void Image_SetChannelAt(Image_ImageHeader const *image,
                                 enum Image_Channel channel,
                                 size_t index,
                                 double value) {
  using namespace Image;

  // block compressed not handled ye
  ASSERT(!ImageFormat_IsCompressed(image->format));

  // split into bit width grouped formats
  uint32_t const pixelSize = ImageFormat_BitWidth(image->format);
  ASSERT(pixelSize >= 8);
  uint8_t *pixelPtr = (uint8_t *) Image_RawDataPtr(image) + (index * pixelSize / 8);

  switch (pixelSize) {
    case 256:BitWidth256SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 192:BitWidth192SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 128:BitWidth128SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 96:BitWidth96SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 64:BitWidth64SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 48:BitWidth48SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 32:BitWidth32SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 24:BitWidth24SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 16:BitWidth16SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 8:BitWidth8SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    default:LOGERRORF("Bitwidth %i from %s not supported",
                      ImageFormat_BitWidth(image->format),
                      ImageFormat_Name(image->format));
  }

}

AL2O3_EXTERN_C void Image_GetPixelAt(Image_ImageHeader const *image, Image_PixelD *pixel, size_t index) {
  ASSERT(image);
  ASSERT(pixel);

  memset(pixel, 0, sizeof(Image_PixelD));

  // intentional fallthrough on this switch statement
  switch (ImageFormat_ChannelCount(image->format)) {
    case 4:pixel->a = Image_GetChannelAt(image, Image_Alpha, index);
    case 3:pixel->b = Image_GetChannelAt(image, Image_Blue, index);
    case 2:pixel->g = Image_GetChannelAt(image, Image_Green, index);
    case 1:pixel->r = Image_GetChannelAt(image, Image_Red, index);
      break;
    default:ASSERT(ImageFormat_ChannelCount(image->format) <= 4);
      break;
  }
}

AL2O3_EXTERN_C void Image_SetPixelAt(Image_ImageHeader const *image, Image_PixelD const *pixel, size_t index) {
  ASSERT(image);
  ASSERT(pixel);

  // intentional fallthrough on this switch statement
  switch (ImageFormat_ChannelCount(image->format)) {
    case 4: Image_SetChannelAt(image, Image_Alpha, index, pixel->a);
    case 3: Image_SetChannelAt(image, Image_Blue, index, pixel->b);
    case 2: Image_SetChannelAt(image, Image_Green, index, pixel->g);
    case 1: Image_SetChannelAt(image, Image_Red, index, pixel->r);
      break;
    default:ASSERT(ImageFormat_ChannelCount(image->format) <= 4);
      break;
  }
}

AL2O3_EXTERN_C size_t Image_BytesRequiredForMipMapsOf(Image_ImageHeader const *image) {

  int const maxMipLevels =
      Math_Log2(Math_MaxI32(image->depth,
                            Math_MaxI32(image->width, image->height)));
  uint32_t minWidth = 1;
  uint32_t minHeight = 1;
  uint32_t minDepth = 1;
  if (ImageFormat_IsCompressed(image->format)) {
    minWidth = ImageFormat_WidthOfBlock(image->format);
    minHeight = ImageFormat_HeightOfBlock(image->format);
  }

  switch (image->format) {
    case ImageFormat_PVR_4BPP_BLOCK:
    case ImageFormat_PVR_4BPPA_BLOCK:
    case ImageFormat_PVR_4BPP_SRGB_BLOCK:
    case ImageFormat_PVR_4BPPA_SRGB_BLOCK:minWidth = 8;
      minHeight = 8;
      break;
    case ImageFormat_PVR_2BPP_BLOCK:
    case ImageFormat_PVR_2BPPA_BLOCK:
    case ImageFormat_PVR_2BPP_SRGB_BLOCK:
    case ImageFormat_PVR_2BPPA_SRGB_BLOCK:minWidth = 16;
      minHeight = 8;
      break;
    default:break;
  }

  size_t size = 0;
  int level = maxMipLevels;

  Image_ImageHeader scratch;
  scratch.format = image->format;
  scratch.width = image->width;
  scratch.height = image->height;
  scratch.depth = image->depth;
  scratch.slices = image->slices;

  while (level > 0) {
    size += Image_ByteCountOf(&scratch);

    scratch.width >>= 1;
    scratch.height >>= 1;
    scratch.depth >>= 1;

    if (scratch.width + scratch.height + scratch.depth == 0) {
      break;
    }
    scratch.width = Math_MaxI32(scratch.width, minWidth);
    scratch.height = Math_MaxI32(scratch.height, minHeight);
    scratch.depth = Math_MaxI32(scratch.depth, minDepth);

    level--;
  }

  return size;

}
