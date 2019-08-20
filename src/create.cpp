#include "al2o3_platform/platform.h"
#include "al2o3_memory/memory.h"
#include "tiny_imageformat/tinyimageformat_base.h"
#include "tiny_imageformat/tinyimageformat_query.h"
#include "gfx_image/image.h"
#include "gfx_image/create.h"

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

	// smallest sized a block texture can have is the block size
	width = Math_MaxU32(width, blockW);
	height = Math_MaxU32(height, blockH);
	depth = Math_MaxU32(depth, blockD);

	// round up to block size
	width = (width + (blockW-1)) & ~(blockW-1);
	height = (height + (blockH-1)) & ~(blockH-1);
	depth = (depth + (blockD-1)) & ~(blockD-1);

	uint64_t const pixelCount = (uint64_t)width * (uint64_t)height * (uint64_t)depth * (uint64_t)slices;
	uint64_t const blockBitSize = TinyImageFormat_BitSizeOfBlock(format);
	uint64_t const blockPixelCount = (uint64_t)blockW * (uint64_t)blockH * (uint64_t)blockD;
	header->dataSize = (pixelCount * blockBitSize) / (blockPixelCount * 8ULL);

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

AL2O3_EXTERN_C Image_ImageHeader const * Image_Create1D(uint32_t width, enum TinyImageFormat format) {
  return Image_Create(width, 1, 1, 1, format);
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_Create1DNoClear(uint32_t width, enum TinyImageFormat format) {
  return Image_CreateNoClear(width, 1, 1, 1, format);
}
AL2O3_EXTERN_C Image_ImageHeader const *Image_Create1DArray(uint32_t width, uint32_t slices, enum TinyImageFormat format) {
  return Image_Create(width, 1, 1, slices, format);
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_Create1DArrayNoClear(uint32_t width, uint32_t slices, enum TinyImageFormat format) {
  return Image_CreateNoClear(width, 1, 1, slices, format);
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_Create2D(uint32_t width, uint32_t height, enum TinyImageFormat format) {
  return Image_Create(width, height, 1, 1, format);
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_Create2DNoClear(uint32_t width, uint32_t height, enum TinyImageFormat format) {
  return Image_CreateNoClear(width, height, 1, 1, format);
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_Create2DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t slices,
                                                enum TinyImageFormat format) {
  return Image_Create(width, height, 1, slices, format);
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_Create2DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t slices,
                                                       enum TinyImageFormat format) {
  return Image_CreateNoClear(width, height, 1, slices, format);
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_Create3D(uint32_t width, uint32_t height, uint32_t depth, enum TinyImageFormat format) {
  return Image_Create(width, height, depth, 1, format);
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_Create3DNoClear(uint32_t width,
                                                  uint32_t height,
                                                  uint32_t depth,
                                                  enum TinyImageFormat format) {
  return Image_CreateNoClear(width, height, depth, 1, format);
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_Create3DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t depth,
                                                uint32_t slices,
                                                enum TinyImageFormat format) {
  return Image_Create(width, height, depth, slices, format);
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_Create3DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t depth,
                                                       uint32_t slices,
                                                       enum TinyImageFormat format) {
  return Image_CreateNoClear(width, height, depth, slices, format);
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_CreateCubemap(uint32_t width, uint32_t height, enum TinyImageFormat format) {
	auto image = (Image_ImageHeader *)Image_Create(width, height, 1, 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_CreateCubemapNoClear(uint32_t width, uint32_t height, enum TinyImageFormat format) {
  auto image = (Image_ImageHeader *)Image_CreateNoClear(width, height, 1, 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_CreateCubemapArray(uint32_t width,
                                                     uint32_t height,
                                                     uint32_t slices,
                                                     enum TinyImageFormat format) {
	if(slices == 0) slices = 1;
	auto image = (Image_ImageHeader*) Image_Create(width, height, 1, slices * 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}
AL2O3_EXTERN_C Image_ImageHeader const*Image_CreateCubemapArrayNoClear(uint32_t width,
                                                            uint32_t height,
                                                            uint32_t slices,
                                                            enum TinyImageFormat format) {
	if(slices == 0) slices = 1;
  auto image = (Image_ImageHeader*) Image_CreateNoClear(width, height, 1, slices * 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_CreateCLUT(uint32_t width, uint32_t height, TinyImageFormat format, uint32_t clutSize) {
	return Image_CreateCLUTArray(width, height, 1, format, clutSize);
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_CreateCLUTNoClear(uint32_t width, uint32_t height, TinyImageFormat format, uint32_t clutSize) {
	return Image_CreateCLUTArrayNoClear(width, height, 1, format, clutSize);
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_CreateCLUTArray(uint32_t width,
																															 uint32_t height,
																															 uint32_t slices,
																															 TinyImageFormat format,
																															 uint32_t clutSize) {
	if(!TinyImageFormat_IsCLUT(format)) return nullptr;

	// this creates the colour image and the lut image
	// currently LUT is always R8G8B8A8 format
	if(slices == 0) slices = 1;
	auto image = (Image_ImageHeader*) Image_Create(width, height, 1, slices, format);
	if(image) {
		auto clutImage = (Image_ImageHeader *) Image_Create(clutSize, 1, 1, 1, TinyImageFormat_R8G8B8A8_UNORM);
		if(clutImage) {
			image->flags = Image_Flag_CLUT;
			image->nextType = Image_NT_CLUT;
			image->nextImage  = clutImage;
		} else {
			Image_Destroy(image);
			return nullptr;
		}
	}
	return image;
}
AL2O3_EXTERN_C Image_ImageHeader const * Image_CreateCLUTArrayNoClear(uint32_t width,
																																			uint32_t height,
																																			uint32_t slices,
																																			TinyImageFormat format,
																																			uint32_t clutSize) {
	if(!TinyImageFormat_IsCLUT(format)) return nullptr;

	// this creates the colour image and the lut image
	// currently LUT is always R8G8B8A8 format
	if(slices == 0) slices = 1;
	auto image = (Image_ImageHeader*) Image_CreateNoClear(width, height, 1, slices, format);
	if(image) {
		auto clutImage = (Image_ImageHeader *) Image_CreateNoClear(clutSize, 1, 1, 1, TinyImageFormat_R8G8B8A8_UNORM);
		if(clutImage) {
			image->flags = Image_Flag_CLUT;
			image->nextType = Image_NT_CLUT;
			image->nextImage  = clutImage;
		} else {
			Image_Destroy(image);
			return nullptr;
		}
	}

	return image;
}
