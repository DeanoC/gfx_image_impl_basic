#include "al2o3_platform/platform.h"
#include "tiny_imageformat/tinyimageformat.h"
#include "gfx_image/image.h"
#include "al2o3_memory/memory.h"


AL2O3_EXTERN_C Image_ImageHeader const *Image_Create(uint32_t width,
                                         uint32_t height,
                                         uint32_t depth,
                                         uint32_t slices,
                                         enum TinyImageFormat format) {
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
                                                enum TinyImageFormat format) {
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
                               enum TinyImageFormat format,
                               Image_ImageHeader *header) {


	uint32_t const blockW = TinyImageFormat_WidthOfBlock(format);
	uint32_t const blockH = TinyImageFormat_HeightOfBlock(format);
	uint32_t const blockD = TinyImageFormat_DepthOfBlock(format);

	// smallest sized a block compressed texture can have is hte block size
	width = Math_MaxU32(width, blockW);
	height = Math_MaxU32(height, blockH);
	depth = Math_MaxU32(depth, blockD);

	// round up to block size
	width = (width + (blockW-1)) & ~(blockW-1);
	height = (height + (blockH-1)) & ~(blockH-1);
	depth = (depth + (blockD-1)) & ~(blockD-1);

	uint64_t const pixelCount = (uint64_t)width * (uint64_t)height * (uint64_t)depth * (uint64_t)slices;
	uint32_t const blockBitSize = TinyImageFormat_BitSizeOfBlock(format);
	header->dataSize = (pixelCount * (uint64_t)blockBitSize) / ((uint64_t)blockW * (uint64_t)blockH * (uint64_t)blockD * 8ULL);

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
																																 enum TinyImageFormat format) {
	auto *image = (Image_ImageHeader *) MEMORY_MALLOC(sizeof(Image_ImageHeader));
	if (!image) { return nullptr; }
	Image_FillHeader(width, height, depth, slices, format, image);

	return image;
}

AL2O3_EXTERN_C void Image_Destroy(Image_ImageHeader const *image) {
	if(!image) return;

	// recursively free next in image chain
  if(image->nextType != Image_NT_None) {
		Image_Destroy(image->nextImage);
	}

  MEMORY_FREE((Image_ImageHeader*)image);
}

//#include "fetch.hpp"
#include "put.hpp"

AL2O3_EXTERN_C size_t Image_ByteCountOfImageChainOf(Image_ImageHeader const *image) {

	size_t total = Image_ByteCountOf(image);

	if(Image_HasPackedMipMaps(image)) {
		return total;
	}

	if(image->nextType != Image_NT_None && image->nextImage != nullptr) {
		total += Image_ByteCountOfImageChainOf(image->nextImage);
	}

	return total;
}
AL2O3_EXTERN_C size_t Image_LinkedImageCountOf(Image_ImageHeader const *image) {
  size_t count = 1;

	if(Image_HasPackedMipMaps(image)) {
		return image->packedMipMapCount;
	}

	while (image && image->nextImage != nullptr) {
    count++;
    image = image->nextImage;
  }

  return count;
}

AL2O3_EXTERN_C Image_ImageHeader const *Image_LinkedImageOf(Image_ImageHeader const *image, size_t const index) {
  size_t count = 0;

  if(index > 0) {
		ASSERT(Image_HasPackedMipMaps(image) == false)
	}

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
                                 TinyImageFormat_LogicalChannel channel,
                                 size_t index,
                                 double value) {
  using namespace Image;

  // block compressed not handled ye
  ASSERT(!TinyImageFormat_IsCompressed(image->format));

	if(channel < 0) {
		LOGERRORF("Channel must be RED, GREEN, BLUE or ALPHA");
		return;
	}


		// split into bit width grouped formats
  uint32_t const pixelSize = TinyImageFormat_BitSizeOfBlock(image->format);
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
											TinyImageFormat_BitSizeOfBlock(image->format),
                      TinyImageFormat_Name(image->format));
  }

}

AL2O3_EXTERN_C void Image_GetPixelAt(Image_ImageHeader const *image, Image_PixelD *pixel, size_t index) {
  ASSERT(image);
  ASSERT(pixel);

	memset(pixel, 0, sizeof(Image_PixelD));

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);

	if(TinyImageFormat_PixelCountOfBlock(image->format) != 1) return;

	if(!TinyImageFormat_CanFetchLogicalPixels(image->format)) return;

	TinyImageFormat_FetchInput input { pixelPtr };
	TinyImageFormat_FetchLogicalPixels(image->format, &input, &pixel->r);
}

AL2O3_EXTERN_C void Image_GetBlockAt(Image_ImageHeader const *image, Image_PixelD *pixel, size_t index) {
	ASSERT(image);
	ASSERT(pixel);

	if(!TinyImageFormat_CanFetchLogicalPixels(image->format)) return;

	uint32_t pixelCount = TinyImageFormat_PixelCountOfBlock(image->format);

	memset(pixel, 0, pixelCount * sizeof(Image_PixelD));

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);


	TinyImageFormat_FetchInput input { pixelPtr };
	TinyImageFormat_FetchLogicalPixels(image->format, &input, (double *)pixel);
}

AL2O3_EXTERN_C void Image_SetPixelAt(Image_ImageHeader const *image, Image_PixelD const *pixel, size_t index) {
  ASSERT(image);
  ASSERT(pixel);

  // intentional fallthrough on this switch statement
  switch (TinyImageFormat_ChannelCount(image->format)) {
    case 4: Image_SetChannelAt(image, TinyImageFormat_LC_Alpha, index, pixel->a);
    case 3: Image_SetChannelAt(image, TinyImageFormat_LC_Blue, index, pixel->b);
    case 2: Image_SetChannelAt(image, TinyImageFormat_LC_Green, index, pixel->g);
    case 1: Image_SetChannelAt(image, TinyImageFormat_LC_Red, index, pixel->r);
      break;
    default:ASSERT(TinyImageFormat_ChannelCount(image->format) <= 4);
      break;
  }
}

AL2O3_EXTERN_C size_t Image_BytesRequiredForMipMapsOf(Image_ImageHeader const *image) {

  int const maxMipLevels =
      Math_Log2(Math_MaxI32(image->depth,
                            Math_MaxI32(image->width, image->height)));
  uint32_t minWidth = TinyImageFormat_WidthOfBlock(image->format);
  uint32_t minHeight = TinyImageFormat_HeightOfBlock(image->format);
  uint32_t minDepth = TinyImageFormat_DepthOfBlock(image->format);

  switch (image->format) {
    case TinyImageFormat_PVRTC1_4BPP_UNORM:
    case TinyImageFormat_PVRTC1_4BPP_SRGB:
		case TinyImageFormat_PVRTC2_4BPP_UNORM:
		case TinyImageFormat_PVRTC2_4BPP_SRGB:
    	minWidth = 8;
      minHeight = 8;
      break;
    case TinyImageFormat_PVRTC1_2BPP_UNORM:
    case TinyImageFormat_PVRTC1_2BPP_SRGB:
		case TinyImageFormat_PVRTC2_2BPP_UNORM:
		case TinyImageFormat_PVRTC2_2BPP_SRGB:
    	minWidth = 16;
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
