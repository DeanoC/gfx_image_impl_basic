#include "al2o3_platform/platform.h"
#include "tiny_imageformat/format.h"
#include "tiny_imageformat/formatcracker.h"
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


