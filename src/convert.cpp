#include "al2o3_platform/platform.h"
#include "gfx_imageformat/format.h"
#include "gfx_imageformat/formatcracker.h"
#include "gfx_image/image.h"
#include "gfx_image/utils.h"

namespace {
constexpr size_t ImageFormatCount() {
  size_t count = 0;
#define IF_START_MACRO
#define IF_MOD_MACRO(x) count++;
#define IF_END_MACRO
#include "gfx_imageformat/format.h"
  return count;
}

typedef void (*ImageConvertFunc)(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const*dst);
typedef bool (*ImageConvertOutOfPlaceFunc)(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const**dst);

#define DefineOutOfPlaceConvert(x) \
bool x##OutOfPlace(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const**dst) {\
  *dst = Image_CloneStructure(src); \
  if (!*dst) { return false; } \
  x(src, newFormat, *dst); \
  return true; \
}

void SlowImageConvert(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const*dst) {
  ASSERT(newFormat == dst->format);
  Image_CopyImage(src, dst);
}

bool SlowImageConvertOutOfPlace(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const**dst) {
  *dst = Image_PreciseConvert(src, newFormat);
  return (*dst != nullptr);
}

void R8G8B8ToR8B8G8A8(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const * dest) {
  do {
    ASSERT(ImageFormat_ChannelCount(src->format) == 3);
    ASSERT(ImageFormat_ChannelCount(newFormat) == 4);

    // this const cast smells wrong but needed because of inplace
    auto dst = (Image_ImageHeader*)dest;
    dst->format = newFormat;

    // Fast path for RGB->RGBA8
    uint8_t const *sdata = (uint8_t *) Image_RawDataPtr(src);
    uint8_t *ddata = (uint8_t *) Image_RawDataPtr(dst);
    size_t nPixels = Image_PixelCountOf(src);
    do {
      ddata[0] = sdata[0];
      ddata[1] = sdata[1];
      ddata[2] = sdata[2];
      ddata[3] = 255;
      sdata += 3;
      ddata += 4;
    } while (--nPixels);

    if (src->nextType != Image_IT_None) {
      src = src->nextImage;
      dst = (Image_ImageHeader*)dst->nextImage;
      dst->nextType = src->nextType;
    }
  } while (src->nextType != Image_IT_None);

}
DefineOutOfPlaceConvert(R8G8B8ToR8B8G8A8)

void R8G8B8A8ToB8G8R8A8(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const * dest) {
  do {
    ASSERT(ImageFormat_ChannelCount(src->format) == 4);
    ASSERT(ImageFormat_ChannelCount(newFormat) == 4);

		// this const cast smells wrong but needed because of inplace
		auto dst = (Image_ImageHeader*)dest;
    dst->format = newFormat;

    // Fast path for RGBA8->BGRA8 (just swizzle)
    uint8_t const *sdata = (uint8_t *) Image_RawDataPtr(src);
    uint8_t *ddata = (uint8_t *) Image_RawDataPtr(dst);
    size_t nPixels = Image_PixelCountOf(src);
    do {
      uint8_t r = sdata[2];
      uint8_t g = sdata[1];
      uint8_t b = sdata[0];
      uint8_t a = sdata[3];
      ddata[0] = r;
      ddata[1] = g;
      ddata[2] = b;
      ddata[3] = a;
      sdata += 4;
      ddata += 4;
    } while (--nPixels);

    if (src->nextType != Image_IT_None) {
      src = src->nextImage;
      dst = (Image_ImageHeader*)dst->nextImage;
      dst->nextType = src->nextType;
    }
  } while (src->nextType != Image_IT_None);

}
DefineOutOfPlaceConvert(R8G8B8A8ToB8G8R8A8)

void HalfNToFloatN(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const*dest) {
  do {
    // Fast path for half->float
		auto dst = (Image_ImageHeader*)dest;
    uint16_t const *sdata = (uint16_t *) Image_RawDataPtr(src);
    float *ddata = (float *) Image_RawDataPtr(dst);
    size_t nPixels = Image_PixelCountOf(src);
    do {
      uint32_t i = 0;
      for (i = 0; i < ImageFormat_ChannelCount(src->format); i++) {
        ddata[i] = Math_Half2Float(sdata[i]);
      }

      for (i = 0; i < ImageFormat_ChannelCount(newFormat); i++) {
        ddata[i] = 0.0f;
      }

      if (ImageFormat_ChannelCount(newFormat) == 4 &&
          ImageFormat_ChannelCount(src->format) < 4) {
        ddata[i] = 1.0f;
      }

      sdata += sizeof(uint16_t) * ImageFormat_ChannelCount(src->format);
      ddata += sizeof(float) * ImageFormat_ChannelCount(newFormat);
    } while (--nPixels);

    if (src->nextType != Image_IT_None) {
      src = src->nextImage;
      dst = (Image_ImageHeader*)dst->nextImage;
      dst->nextType = src->nextType;
    }
  } while (src->nextType != Image_IT_None);

}
DefineOutOfPlaceConvert(HalfNToFloatN)

void FloatNToHalfN(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const*dest) {
  do {
    // Fast path for float->half
		auto dst = (Image_ImageHeader*)dest;
		float const *sdata = (float const *) Image_RawDataPtr(src);
    uint16_t *ddata = (uint16_t *) Image_RawDataPtr(dst);
    size_t nPixels = Image_PixelCountOf(src);
    do {
      uint32_t i = 0;
      for (i = 0; i < ImageFormat_ChannelCount(src->format); i++) {
        ddata[i] = Math_Float2Half(sdata[i]);
      }

      for (i = 0; i < ImageFormat_ChannelCount(newFormat); i++) {
        ddata[i] = Math_Float2Half(0.0f);
      }
      if (ImageFormat_ChannelCount(newFormat) == 4 &&
          ImageFormat_ChannelCount(src->format) < 4) {
        ddata[i] = Math_Float2Half(1.0f);
      }

      sdata += sizeof(uint16_t) * ImageFormat_ChannelCount(src->format);
      ddata += sizeof(float) * ImageFormat_ChannelCount(newFormat);
    } while (--nPixels);

    if (src->nextType != Image_IT_None) {
      src = src->nextImage;
      dst = (Image_ImageHeader*)dst->nextImage;
      dst->nextType = src->nextType;
    }
  } while (src->nextType != Image_IT_None);
}
DefineOutOfPlaceConvert(FloatNToHalfN)

#define IntegerTypeNToFloatN(t, n1, n2) \
void t##ToFloat_##n1##_##n2(Image_ImageHeader const*src, ImageFormat newFormat, Image_ImageHeader const * dest) { \
  do { \
  	auto dst = (Image_ImageHeader*)dest;\
    t const *sdata = (t*) Image_RawDataPtr(src); \
    float* ddata = (float*) Image_RawDataPtr(dst); \
    size_t nPixels = Image_PixelCountOf(src); \
    do { \
      int i = 0; \
      for (i = 0; i < n1; i++) { \
        ddata[i] = (float)(sdata[i]); \
      } \
      for (i = 0; i < n2; i++) { \
        ddata[i] = 0.0f; \
      } \
      if(n1 == 4 && n2 < 4) { \
        ddata[i] = 1.0f; \
      } \
      sdata += sizeof(t) * n1; \
      ddata += sizeof(float) * n2; \
    } \
    while (--nPixels); \
    if(src->nextType != Image_IT_None) { \
      src = src->nextImage; \
      dst = (Image_ImageHeader*)dst->nextImage; \
      dst->nextType = src->nextType; \
    } \
  } while(src->nextType != Image_IT_None); \
} \
DefineOutOfPlaceConvert(t##ToFloat_##n1##_##n2)

#define IntegerTypeToFloat(t) \
  IntegerTypeNToFloatN(t, 1, 1) \
  IntegerTypeNToFloatN(t, 1, 2) \
  IntegerTypeNToFloatN(t, 1, 3) \
  IntegerTypeNToFloatN(t, 1, 4) \
  IntegerTypeNToFloatN(t, 2, 1) \
  IntegerTypeNToFloatN(t, 2, 2) \
  IntegerTypeNToFloatN(t, 2, 3) \
  IntegerTypeNToFloatN(t, 2, 4) \
  IntegerTypeNToFloatN(t, 3, 1) \
  IntegerTypeNToFloatN(t, 3, 2) \
  IntegerTypeNToFloatN(t, 3, 3) \
  IntegerTypeNToFloatN(t, 3, 4) \
  IntegerTypeNToFloatN(t, 4, 1) \
  IntegerTypeNToFloatN(t, 4, 2) \
  IntegerTypeNToFloatN(t, 4, 3) \
  IntegerTypeNToFloatN(t, 4, 4)

IntegerTypeToFloat(uint8_t);
IntegerTypeToFloat(uint16_t);
IntegerTypeToFloat(uint32_t);
IntegerTypeToFloat(int8_t);
IntegerTypeToFloat(int16_t);
IntegerTypeToFloat(int32_t);

#undef DefineOutOfPlaceConvert
#undef IntegerTypeNToFloatN
#undef IntegerTypeToFloat

bool g_imageConvertTablesBuild = false;
bool g_imageConvertCanInPlace[ImageFormatCount()][ImageFormatCount()];
ImageConvertFunc g_imageConvertDDTable[ImageFormatCount()][ImageFormatCount()];
ImageConvertOutOfPlaceFunc g_imageConvertOutOfPlaceDDTable[ImageFormatCount()][ImageFormatCount()];

void BuildImageConvertTables() {
  for (auto i = 0u; i < ImageFormatCount(); ++i) {
    for (auto j = 0u; j < ImageFormatCount(); ++j) {
      g_imageConvertCanInPlace[i][j] = false;
      g_imageConvertDDTable[i][j] = &SlowImageConvert;
      g_imageConvertOutOfPlaceDDTable[i][j] = &SlowImageConvertOutOfPlace;
    }
  }

#define FDT(s, d, f, i) \
  g_imageConvertDDTable[ImageFormat_##s][ImageFormat_##d] = f; \
  g_imageConvertOutOfPlaceDDTable[ImageFormat_##s][ImageFormat_##d] = f##OutOfPlace; \
  g_imageConvertCanInPlace[ImageFormat_##s][ImageFormat_##d] = i;

#define FDT_SET(s, d, f, i) \
  FDT(s##_UNORM, d##_UNORM, f, i); \
  FDT(s##_SNORM, d##_SNORM, f, i); \
  FDT(s##_UINT, d##_UINT, f, i); \
  FDT(s##_SINT, d##_SINT, f, i); \
  FDT(s##_USCALED, d##_USCALED, f, i); \
  FDT(s##_SSCALED, d##_SSCALED, f, i); \
  FDT(s##_SRGB, d##_SRGB, f, i);

#define FDT_ITOF(t, s, d, n1, n2, inplace) \
  g_imageConvertDDTable[ImageFormat_##s][ImageFormat_##d] = &t##ToFloat_##n1##_##n2; \
  g_imageConvertOutOfPlaceDDTable[ImageFormat_##s][ImageFormat_##d] = &t##ToFloat_##n1##_##n2##OutOfPlace; \
  g_imageConvertCanInPlace[ImageFormat_##s][ImageFormat_##d] = (inplace);

#define FDT_ITOF_SET(t, s, d, n1, n2, w) \
  FDT_ITOF(t, s##_UNORM, d##_SFLOAT, n1, n2, (w*n1) <= (32*n2)) \
  FDT_ITOF(t, s##_SNORM, d##_SFLOAT, n1, n2, (w*n1) <= (32*n2)) \
  FDT_ITOF(t, s##_UINT, d##_SFLOAT, n1, n2, (w*n1) <= (32*n2)) \
  FDT_ITOF(t, s##_SINT, d##_SFLOAT, n1, n2, (w*n1) <= (32*n2)) \
  FDT_ITOF(t, s##_USCALED, d##_SFLOAT, n1, n2, (w*n1) <= (32*n2)) \
  FDT_ITOF(t, s##_SSCALED, d##_SFLOAT, n1, n2, (w*n1) <= (32*n2))

#define FDT_ITOF_WIDTH_SET(w) \
  FDT_ITOF_SET(uint##w##_t, R##w, R32, 1, 1, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w, R32, 2, 1, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w##B##w, R32, 3, 1, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w##B##w##A##w, R32, 4, 1, w) \
  FDT_ITOF_SET(uint##w##_t, R##w, R32G32, 1, 2, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w, R32G32, 2, 2, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w##B##w, R32G32 , 3, 2, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w##B##w##A##w, R32G32, 4, 2, w) \
  FDT_ITOF_SET(uint##w##_t, R##w, R32G32B32, 1, 3, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w, R32G32B32, 2, 3, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w##B##w, R32G32B32, 3, 3, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w##B##w##A##w, R32G32B32, 4, 3, w) \
  FDT_ITOF_SET(uint##w##_t, R##w, R32G32B32A32, 1, 4, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w, R32G32B32A32, 2, 4, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w##B##w, R32G32B32A32, 3, 4, w) \
  FDT_ITOF_SET(uint##w##_t, R##w##G##w##B##w##A##w, R32G32B32A32, 4, 4, w) \
  FDT_ITOF_SET(int##w##_t, R##w, R32, 1, 1, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w, R32, 2, 1, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w##B##w, R32, 3, 1, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w##B##w##A##w, R32, 4, 1, w) \
  FDT_ITOF_SET(int##w##_t, R##w, R32G32, 1, 2, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w, R32G32, 2, 2, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w##B##w, R32G32 , 3, 2, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w##B##w##A##w, R32G32, 4, 2, w) \
  FDT_ITOF_SET(int##w##_t, R##w, R32G32B32, 1, 3, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w, R32G32B32, 2, 3, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w##B##w, R32G32B32, 3, 3, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w##B##w##A##w, R32G32B32, 4, 3, w) \
  FDT_ITOF_SET(int##w##_t, R##w, R32G32B32A32, 1, 4, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w, R32G32B32A32, 2, 4, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w##B##w, R32G32B32A32, 3, 4, w) \
  FDT_ITOF_SET(int##w##_t, R##w##G##w##B##w##A##w, R32G32B32A32, 4, 4, w)

  FDT_ITOF_WIDTH_SET(8);
  FDT_ITOF_WIDTH_SET(16);


/* TODO
*(uint32_t *)dest = Math_FloatRGBToRGBE8(rgba[0], rgba[1], rgba[2]);
*(uint32_t *) dest = Math_FloatRGBToRGB9E5(rgba[0], rgba[1], rgba[2]);
} else if (newFormat >=I16&& newFormat<= RGBA16) {
((uint16_t *) dest)[i] = (uint16_t) (65535 * Math_SaturateF(rgba[i])+ 0.5f);
} else if (newFormat == RGB10A2) {
*(uint32_t *) dest =
(uint32_t(1023.0f * Math_SaturateF(rgba[0]) + 0.5f) << 22) |
    (uint32_t(1023.0f * Math_SaturateF(rgba[1]) + 0.5f) << 12) |
    (uint32_t(1023.0f * Math_SaturateF(rgba[2]) + 0.5f) << 2) |
    (uint32_t(3.0f * Math_SaturateF(rgba[3]) + 0.5f));
} else {
for (
int i = 0;
i<nDestChannels;
i++) {
dest[i] = (unsigned char) (255 *
Math_SaturateF(rgba[i])
+ 0.5f);
}
}

src +=
srcSize;
dest +=
destSize;
} while (--nPixels);
}*/

#undef FDT_ITOF_WIDTH_SET
#undef FDT_ITOF_SET
#undef FDT_ITOF
#undef FDT_SET
#undef FDT

  g_imageConvertTablesBuild = true;
}

} // end anon namespace

AL2O3_EXTERN_C Image_ImageHeader const * Image_FastConvert(Image_ImageHeader const * src, ImageFormat const newFormat, bool allowInPlace) {
  ASSERT(src);

  if (allowInPlace && src->format == newFormat) {
    return src;
  }

  if (g_imageConvertTablesBuild == false) {
    BuildImageConvertTables();
  }

  if (allowInPlace && g_imageConvertCanInPlace[src->format][newFormat]) {
    g_imageConvertDDTable[src->format][newFormat](src, newFormat, src);
    return src;
  } else {
    Image_ImageHeader const *dst;
    bool ret = g_imageConvertOutOfPlaceDDTable[src->format][newFormat](src, newFormat, &dst);
    if (ret) { return dst; }
    else { return nullptr; }
  }
}


