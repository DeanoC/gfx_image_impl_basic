#include "al2o3_platform/platform.h"
#include "tiny_imageformat/tinyimageformat_base.h"
#include "tiny_imageformat/tinyimageformat_query.h"
#include "tiny_imageformat/tinyimageformat_decode.h"
#include "tiny_imageformat/tinyimageformat_encode.h"
#include "gfx_image/image.h"
#include "al2o3_memory/memory.h"


AL2O3_EXTERN_C Image_ImageHeader const *Image_Create(uint32_t width,
                                         uint32_t height,
                                         uint32_t depth,
                                         uint32_t slices,
                                         TinyImageFormat format) {
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
                                                TinyImageFormat format) {
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
                               TinyImageFormat format,
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
																																 TinyImageFormat format) {
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

	while (image && image->nextImage != nullptr) {
    count++;
    image = image->nextImage;
  }

  return count;
}

AL2O3_EXTERN_C size_t Image_MipMapCountOf(Image_ImageHeader const *image) {
	size_t count = 1;

	if(Image_HasPackedMipMaps(image)) {
		return image->packedMipMapCount;
	}

	while (image && image->nextType == Image_NT_MipMap && image->nextImage != nullptr) {
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


AL2O3_EXTERN_C bool Image_GetPixelAtF(Image_ImageHeader const *image, float *pixel, size_t index) {
	ASSERT(image);
	ASSERT(pixel);

	memset(pixel, 0, sizeof(Image_PixelD));

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);

	if(TinyImageFormat_PixelCountOfBlock(image->format) != 1) return false;

	if(!TinyImageFormat_CanDecodeLogicalPixelsF(image->format)) return false;

	TinyImageFormat_FetchInput input { pixelPtr };
	return TinyImageFormat_DecodeLogicalPixelsF(image->format, &input, 1, pixel);
}

AL2O3_EXTERN_C bool Image_GetBlockAtF(Image_ImageHeader const *image, float *pixels, size_t index) {
	ASSERT(image);
	ASSERT(pixels);

	if(!TinyImageFormat_CanDecodeLogicalPixelsF(image->format)) return false;

	uint32_t const pixelCount = TinyImageFormat_PixelCountOfBlock(image->format);

	memset(pixels, 0, pixelCount * sizeof(float) * 4);

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);


	TinyImageFormat_FetchInput input { pixelPtr };
	return TinyImageFormat_DecodeLogicalPixelsF(image->format, &input, 1, pixels);
}

AL2O3_EXTERN_C bool Image_GetRowAtF(Image_ImageHeader const *image, float *pixels, size_t index) {
	ASSERT(image);
	ASSERT(pixels);

	if(!TinyImageFormat_CanDecodeLogicalPixelsF(image->format)) return false;

	uint32_t const blockWidth = TinyImageFormat_WidthOfBlock(image->format);

	memset(pixels, 0, image->width * sizeof(float) * 4);

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);


	TinyImageFormat_FetchInput input { pixelPtr };
	return TinyImageFormat_DecodeLogicalPixelsF(image->format, &input, image->width / blockWidth, pixels);
}

AL2O3_EXTERN_C bool Image_GetPixelAtD(Image_ImageHeader const *image, Image_PixelD *pixel, size_t index) {
  ASSERT(image);
  ASSERT(pixel);

	memset(pixel, 0, sizeof(Image_PixelD));

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);

	if(TinyImageFormat_PixelCountOfBlock(image->format) != 1) return false;

	if(!TinyImageFormat_CanDecodeLogicalPixelsD(image->format)) return false;

	TinyImageFormat_FetchInput input { pixelPtr };
	return TinyImageFormat_DecodeLogicalPixelsD(image->format, &input, 1, (double*) pixel);
}

AL2O3_EXTERN_C bool Image_GetBlockAtD(Image_ImageHeader const *image, Image_PixelD *pixels, size_t index) {
	ASSERT(image);
	ASSERT(pixels);

	if(!TinyImageFormat_CanDecodeLogicalPixelsD(image->format)) return false;

	uint32_t pixelCount = TinyImageFormat_PixelCountOfBlock(image->format);

	memset(pixels, 0, pixelCount * sizeof(Image_PixelD));

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);


	TinyImageFormat_FetchInput input { pixelPtr };
	return TinyImageFormat_DecodeLogicalPixelsD(image->format, &input, 1, (double *)pixels);
}

AL2O3_EXTERN_C bool Image_SetPixelAtD(Image_ImageHeader const *image, Image_PixelD const *pixel, size_t index) {
	ASSERT(image);
	ASSERT(pixel);

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);

	if(TinyImageFormat_PixelCountOfBlock(image->format) != 1) return false;

	if(!TinyImageFormat_CanEncodeLogicalPixelsF(image->format)) return false;

	TinyImageFormat_PutOutput output { pixelPtr };
	return TinyImageFormat_EncodeLogicalPixelsD(image->format, (double*) pixel, 1, &output);
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
