#include "al2o3_platform/platform.h"
#include "tiny_imageformat/tinyimageformat_base.h"
#include "tiny_imageformat/tinyimageformat_query.h"
#include "tiny_imageformat/tinyimageformat_decode.h"
#include "tiny_imageformat/tinyimageformat_encode.h"
#include "gfx_image/image.h"

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

AL2O3_EXTERN_C bool Image_GetBlocksAtF(Image_ImageHeader const *image, float *pixels, size_t blockCount, size_t index) {
	ASSERT(image);
	ASSERT(pixels);

	if(!TinyImageFormat_CanDecodeLogicalPixelsF(image->format)) return false;

	uint32_t const pixelCount = TinyImageFormat_PixelCountOfBlock(image->format);

	memset(pixels, 0, pixelCount * sizeof(float) * 4);

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);


	TinyImageFormat_FetchInput input { pixelPtr };
	return TinyImageFormat_DecodeLogicalPixelsF(image->format, &input, (uint32_t)blockCount, pixels);
}

AL2O3_EXTERN_C bool Image_SetBlocksAtF(Image_ImageHeader const *image, float const *pixels, size_t blockCount, size_t index) {
	ASSERT(image);
	ASSERT(pixels);

	if(!TinyImageFormat_CanEncodeLogicalPixelsF(image->format)) return false;

	uint32_t const pixelCount = TinyImageFormat_PixelCountOfBlock(image->format);

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);


	TinyImageFormat_EncodeOutput output { pixelPtr };
	return TinyImageFormat_EncodeLogicalPixelsF(image->format, pixels, (uint32_t)blockCount, &output);
}


AL2O3_EXTERN_C bool Image_GetPixelAtF(Image_ImageHeader const *image, float *pixel, size_t index) {
	ASSERT(image);
	if(TinyImageFormat_PixelCountOfBlock(image->format) != 1) return false;
	return Image_GetBlocksAtF(image, pixel, 1, index);
}

AL2O3_EXTERN_C bool Image_SetPixelAtF(Image_ImageHeader const *image, float const *pixel, size_t index) {
	ASSERT(image);
	if(TinyImageFormat_PixelCountOfBlock(image->format) != 1) return false;
	return Image_SetBlocksAtF(image, pixel, 1, index);
}

AL2O3_EXTERN_C bool Image_GetRowAtF(Image_ImageHeader const *image, float *pixels, size_t index) {
	ASSERT(image);
	uint32_t const blockWidth = TinyImageFormat_WidthOfBlock(image->format);

	return Image_GetBlocksAtF(image, pixels, image->width / blockWidth, index);
}

AL2O3_EXTERN_C bool Image_SetRowAtF(Image_ImageHeader const *image, float const *pixels, size_t index) {
	ASSERT(image);

	uint32_t const blockWidth = TinyImageFormat_WidthOfBlock(image->format);
	return Image_SetBlocksAtF(image, pixels, image->width / blockWidth, index);
}


AL2O3_EXTERN_C bool Image_GetBlocksAtD(Image_ImageHeader const *image, double *pixels, size_t blockCount, size_t index) {
	ASSERT(image);
	ASSERT(pixels);

	if(!TinyImageFormat_CanDecodeLogicalPixelsD(image->format)) return false;

	uint32_t const pixelCount = TinyImageFormat_PixelCountOfBlock(image->format);

	memset(pixels, 0, pixelCount * sizeof(float) * 4);

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);


	TinyImageFormat_FetchInput input { pixelPtr };
	return TinyImageFormat_DecodeLogicalPixelsD(image->format, &input, (uint32_t)blockCount, pixels);
}

AL2O3_EXTERN_C bool Image_SetBlocksAtD(Image_ImageHeader const *image, double const *pixels, size_t blockCount, size_t index) {
	ASSERT(image);
	ASSERT(pixels);

	if(!TinyImageFormat_CanEncodeLogicalPixelsD(image->format)) return false;

	uint32_t const pixelCount = TinyImageFormat_PixelCountOfBlock(image->format);

	uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
			index * (TinyImageFormat_BitSizeOfBlock(image->format) / 8);


	TinyImageFormat_EncodeOutput output { pixelPtr };
	return TinyImageFormat_EncodeLogicalPixelsD(image->format, pixels, (uint32_t)blockCount, &output);
}

AL2O3_EXTERN_C bool Image_GetPixelAtD(Image_ImageHeader const *image, double *pixel, size_t index) {
	ASSERT(image);
	if(TinyImageFormat_PixelCountOfBlock(image->format) != 1) return false;
	return Image_GetBlocksAtD(image, pixel, 1, index);
}

AL2O3_EXTERN_C bool Image_SetPixelAtD(Image_ImageHeader const *image, double const *pixel, size_t index) {
	ASSERT(image);
	if(TinyImageFormat_PixelCountOfBlock(image->format) != 1) return false;
	return Image_SetBlocksAtD(image, pixel, 1, index);
}

AL2O3_EXTERN_C bool Image_GetRowAtD(Image_ImageHeader const *image, double *pixels, size_t index) {
	ASSERT(image);
	uint32_t const blockWidth = TinyImageFormat_WidthOfBlock(image->format);

	return Image_GetBlocksAtD(image, pixels, image->width / blockWidth, index);
}

AL2O3_EXTERN_C bool Image_SetRowAtD(Image_ImageHeader const *image, double const *pixels, size_t index) {
	ASSERT(image);

	uint32_t const blockWidth = TinyImageFormat_WidthOfBlock(image->format);
	return Image_SetBlocksAtD(image, pixels, image->width / blockWidth, index);
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
