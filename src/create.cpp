#include "al2o3_platform/platform.h"
#include "tiny_imageformat/tinyimageformat.h"
#include "gfx_image/image.h"
#include "gfx_image/create.h"

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
