#include "al2o3_platform/platform.h"
#include "gfx_image/image.h"
#include "tiny_imageformat/tinyimageformat_base.h"
#include "gfx_image/create.h"
#include "al2o3_catch2/catch2.hpp"

TEST_CASE("Image create/destroy 1D (C)", "[Image]") {
  Image_ImageHeader const *image0 = Image_Create1D(256, TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image0);
  CHECK(image0->format == TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image0->width == 256);
  CHECK(image0->height == 1);
  CHECK(image0->depth == 1);
  CHECK(image0->slices == 1);
  CHECK(image0->dataSize == (256 * TinyImageFormat_BitSizeOfBlock(image0->format)) / 8);
  CHECK(image0->nextImage == nullptr);
  CHECK(image0->nextType == Image_NT_None);
  CHECK(image0->flags == 0);
  Image_Destroy(image0);

  Image_ImageHeader const *image1 = Image_Create1DNoClear(64, TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image1);
  CHECK(image1->format == TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image1->width == 64);
  CHECK(image1->height == 1);
  CHECK(image1->depth == 1);
  CHECK(image1->slices == 1);
  CHECK(image1->dataSize == (64 * TinyImageFormat_BitSizeOfBlock((image1->format)) / 8));
  CHECK(image1->nextImage == nullptr);
  CHECK(image1->nextType == Image_NT_None);
  CHECK(image1->flags == 0);
  Image_Destroy(image1);

  Image_ImageHeader const *image2 = Image_Create1DArray(256, 20, TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image2);
  CHECK(image2->format == TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image2->width == 256);
  CHECK(image2->height == 1);
  CHECK(image2->depth == 1);
  CHECK(image2->slices == 20);
  CHECK(image2->dataSize == (256 * 20 * TinyImageFormat_BitSizeOfBlock((image2->format)) / 8));
  CHECK(image2->nextImage == nullptr);
  CHECK(image2->nextType == Image_NT_None);
  CHECK(image2->flags == 0);
  Image_Destroy(image2);

  Image_ImageHeader const *image3 = Image_Create1DArrayNoClear(64, 100, TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image3);
  CHECK(image3->format == TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image3->width == 64);
  CHECK(image3->height == 1);
  CHECK(image3->depth == 1);
  CHECK(image3->slices == 100);
  CHECK(image3->dataSize == (64 * 100 * TinyImageFormat_BitSizeOfBlock(image3->format)) / 8);
  CHECK(image3->nextImage == nullptr);
  CHECK(image3->nextType == Image_NT_None);
  CHECK(image3->flags == 0);
  Image_Destroy(image3);

}

TEST_CASE("Image create/destroy 2D (C)", "[Image]") {
  Image_ImageHeader const *image0 = Image_Create2D(256, 256, TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image0);
  CHECK(image0->format == TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image0->width == 256);
  CHECK(image0->height == 256);
  CHECK(image0->depth == 1);
  CHECK(image0->slices == 1);
  CHECK(image0->dataSize == (256 * 256 * TinyImageFormat_BitSizeOfBlock(image0->format)) / 8);
  CHECK(image0->nextImage == nullptr);
  CHECK(image0->nextType == Image_NT_None);
  CHECK(image0->flags == 0);
  Image_Destroy(image0);

  Image_ImageHeader const *image1 = Image_Create2DNoClear(64, 128, TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image1);
  CHECK(image1->format == TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image1->width == 64);
  CHECK(image1->height == 128);
  CHECK(image1->depth == 1);
  CHECK(image1->slices == 1);
  CHECK(image1->dataSize == (64 * 128 * TinyImageFormat_BitSizeOfBlock(image1->format)) / 8);
  CHECK(image1->nextImage == nullptr);
  CHECK(image1->nextType == Image_NT_None);
  CHECK(image1->flags == 0);
  Image_Destroy(image1);

  Image_ImageHeader const *image2 = Image_Create2DArray(256, 256, 20, TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image2);
  CHECK(image2->format == TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image2->width == 256);
  CHECK(image2->height == 256);
  CHECK(image2->depth == 1);
  CHECK(image2->slices == 20);
  CHECK(image2->dataSize == (256 * 256 * 20 * TinyImageFormat_BitSizeOfBlock(image2->format)) / 8);
  CHECK(image2->nextImage == nullptr);
  CHECK(image2->nextType == Image_NT_None);
  CHECK(image2->flags == 0);
  Image_Destroy(image2);

  Image_ImageHeader const *image3 = Image_Create2DArrayNoClear(64, 32, 100, TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image3);
  CHECK(image3->format == TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image3->width == 64);
  CHECK(image3->height == 32);
  CHECK(image3->depth == 1);
  CHECK(image3->slices == 100);
  CHECK(image3->dataSize == (64 * 32 * 100 * TinyImageFormat_BitSizeOfBlock(image3->format)) / 8);
  CHECK(image3->nextImage == nullptr);
  CHECK(image3->nextType == Image_NT_None);
  CHECK(image3->flags == 0);
  Image_Destroy(image3);
}

TEST_CASE("Image create/destroy 3D (C)", "[Image]") {
  Image_ImageHeader const *image0 = Image_Create3D(256, 256, 16, TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image0);
  CHECK(image0->format == TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image0->width == 256);
  CHECK(image0->height == 256);
  CHECK(image0->depth == 16);
  CHECK(image0->slices == 1);
  CHECK(image0->dataSize == (256ULL * 256 * 16 * TinyImageFormat_BitSizeOfBlock(image0->format)) / 8);
  CHECK(image0->nextImage == nullptr);
  CHECK(image0->nextType == Image_NT_None);
  CHECK(image0->flags == 0);
  Image_Destroy(image0);

  Image_ImageHeader const *image1 = Image_Create3DNoClear(64, 128, 256, TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image1);
  CHECK(image1->format == TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image1->width == 64);
  CHECK(image1->height == 128);
  CHECK(image1->depth == 256);
  CHECK(image1->slices == 1);
  CHECK(image1->dataSize == (64 * 128 * 256 * TinyImageFormat_BitSizeOfBlock(image1->format)) / 8);
  CHECK(image1->nextImage == nullptr);
  CHECK(image1->nextType == Image_NT_None);
  CHECK(image1->flags == 0);
  Image_Destroy(image1);

  Image_ImageHeader const *image2 = Image_Create3DArray(256, 256, 256, 20, TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image2);
  CHECK(image2->format == TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image2->width == 256);
  CHECK(image2->height == 256);
  CHECK(image2->depth == 256);
  CHECK(image2->slices == 20);
  CHECK(image2->dataSize == (256ULL * 256 * 256 * 20 * TinyImageFormat_BitSizeOfBlock(image2->format)) / 8);
  CHECK(image2->nextImage == nullptr);
  CHECK(image2->nextType == Image_NT_None);
  CHECK(image2->flags == 0);
  Image_Destroy(image2);

  Image_ImageHeader const *image3 = Image_Create3DArrayNoClear(64, 32, 16, 100, TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image3);
  CHECK(image3->format == TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image3->width == 64);
  CHECK(image3->height == 32);
  CHECK(image3->depth == 16);
  CHECK(image3->slices == 100);
  CHECK(image3->dataSize == (64 * 32 * 16 * 100 * TinyImageFormat_BitSizeOfBlock(image3->format)) / 8);
  CHECK(image3->nextImage == nullptr);
  CHECK(image3->nextType == Image_NT_None);
  CHECK(image3->flags == 0);
  Image_Destroy(image3);

  // HUGE test
  Image_ImageHeader const *image4 = Image_Create3DArrayNoClear(1024, 1024, 16, 10, TinyImageFormat_R64G64B64A64_SFLOAT);
  CHECK(image4);
  CHECK(image4->format == TinyImageFormat_R64G64B64A64_SFLOAT);
  CHECK(image4->width == 1024);
  CHECK(image4->height == 1024);
  CHECK(image4->depth == 16);
  CHECK(image4->slices == 10);
  CHECK(image4->dataSize == (1024ULL * 1024ULL * 16ULL * 10ULL * (uint64_t)TinyImageFormat_BitSizeOfBlock(image4->format)) / 8ULL);
  CHECK(image4->nextImage == nullptr);
  CHECK(image4->nextType == Image_NT_None);
  CHECK(image4->flags == 0);
  Image_Destroy(image4);

	// Out of memory test (at least for 2019!)
	// just 1Kx1kx1Kx1K succeeds on OSX but this huge 45 PB should last this year out
	Image_ImageHeader const *image5 = Image_Create3DArrayNoClear(10240, 10240, 10240, 10240, TinyImageFormat_R64G64B64A64_SFLOAT);
	CHECK(image5 == nullptr);
	Image_Destroy(image5);

}

TEST_CASE("Image create/destroy 2D Cubemap (C)", "[Image]") {
  Image_ImageHeader const *image0 = Image_CreateCubemap(256, 256, TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image0);
  CHECK(image0->format == TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image0->width == 256);
  CHECK(image0->height == 256);
  CHECK(image0->depth == 1);
  CHECK(image0->slices == 6);
  CHECK(image0->dataSize == (256 * 256 * 6 * TinyImageFormat_BitSizeOfBlock(image0->format)) / 8);
  CHECK(image0->nextImage == nullptr);
  CHECK(image0->nextType == Image_NT_None);
  CHECK(image0->flags == Image_Flag_Cubemap);
  Image_Destroy(image0);

  Image_ImageHeader const *image1 = Image_CreateCubemapNoClear(64, 128, TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image1);
  CHECK(image1->format == TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image1->width == 64);
  CHECK(image1->height == 128);
  CHECK(image1->depth == 1);
  CHECK(image1->slices == 6);
  CHECK(image1->dataSize == (64 * 128 * 6 * TinyImageFormat_BitSizeOfBlock(image1->format)) / 8);
  CHECK(image1->nextImage == nullptr);
  CHECK(image1->nextType == Image_NT_None);
  CHECK(image1->flags == Image_Flag_Cubemap);
  Image_Destroy(image1);

  Image_ImageHeader const *image2 = Image_CreateCubemapArray(256, 256, 20, TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image2);
  CHECK(image2->format == TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image2->width == 256);
  CHECK(image2->height == 256);
  CHECK(image2->depth == 1);
  CHECK(image2->slices == 20 * 6);
  CHECK(image2->dataSize == (256 * 256 * 20 * 6 * TinyImageFormat_BitSizeOfBlock(image2->format)) / 8);
  CHECK(image2->nextImage == nullptr);
  CHECK(image2->nextType == Image_NT_None);
  CHECK(image2->flags == Image_Flag_Cubemap);
  Image_Destroy(image2);

  Image_ImageHeader const *image3 = Image_CreateCubemapArrayNoClear(64, 32, 100, TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image3);
  CHECK(image3->format == TinyImageFormat_R5G5B5A1_UNORM);
  CHECK(image3->width == 64);
  CHECK(image3->height == 32);
  CHECK(image3->depth == 1);
  CHECK(image3->slices == 100 * 6);
  CHECK(image3->dataSize == (64 * 32 * 100 * 6 * TinyImageFormat_BitSizeOfBlock(image3->format)) / 8);
  CHECK(image3->nextImage == nullptr);
  CHECK(image3->nextType == Image_NT_None);
  CHECK(image3->flags == Image_Flag_Cubemap);
  Image_Destroy(image3);
}

TEST_CASE("Calculate Index (C)", "[Image]") {
  Image_ImageHeader const *image = Image_Create3DArray(17, 17, 17, 17, TinyImageFormat_R8G8B8A8_UNORM);
  CHECK(image);
  uint64_t const pixelSize = TinyImageFormat_BitSizeOfBlock(image->format);
  CHECK(image->dataSize == (17 * 17 * 17 * 17 * pixelSize) / 8);

  CHECK(pixelSize / 8 == 4);

  // check the image is cleared

  uint8_t *ptr = (uint8_t *) Image_RawDataPtr(image);
  CHECK(ptr);
  CHECK(ptr == (uint8_t *) (image + 1));

  uint64_t accum0 = 0;
  for (auto i = 0u; i < image->dataSize; ++i) {
    accum0 += ptr[i];
  }
  CHECK(accum0 == 0);

  for (auto i = 0u; i < image->dataSize; ++i) {
    ptr[i] = (uint8_t) (i & 0xFFu);
  }

  for (uint32_t s = 0; s < image->slices; ++s) {
    for (uint32_t d = 0; d < image->depth; ++d) {
      for (uint32_t y = 0; y < image->height; ++y) {
        for (uint32_t x = 0; x < image->width; ++x) {
          size_t index = Image_CalculateIndex(image, x, y, d, s);
          CHECK(ptr[index] == (uint8_t) (index & 0xFFu));
        }
      }
    }
  }

  Image_Destroy(image);
}

void ImageTester(uint32_t w_, uint32_t h_, uint32_t d_, uint32_t s_, TinyImageFormat fmt_, bool doLog_) {
  using namespace Catch::literals;
  if (fmt_ == TinyImageFormat_UNDEFINED) { return; }

  auto img = Image_Create(w_, h_, d_, s_, fmt_);
  if (doLog_) {
    LOGINFOF("Testing %s", TinyImageFormat_Name(fmt_));
  }

  CHECK(img);
  CHECK(img->width == w_);
  CHECK(img->height == h_);
  CHECK(img->depth == d_);
  CHECK(img->slices == s_);
  CHECK(img->format == fmt_);
  uint64_t const pixelSize = TinyImageFormat_BitSizeOfBlock(img->format);
  CHECK(img->dataSize == (w_ * h_ * d_ * s_ * pixelSize) / 8);

  // check the image is cleared
  uint8_t *ptr = (uint8_t *) Image_RawDataPtr(img);
  CHECK(ptr);
  CHECK(ptr == (uint8_t *) (img + 1));
  for (auto i = 0u; i < img->dataSize; ++i) {
    CHECK(ptr[i] == 0);
  }

  double const mins[4] = {
      TinyImageFormat_Min(img->format, TinyImageFormat_LC_Red),
      TinyImageFormat_Min(img->format, TinyImageFormat_LC_Green),
      TinyImageFormat_Min(img->format, TinyImageFormat_LC_Blue),
      TinyImageFormat_Min(img->format, TinyImageFormat_LC_Alpha)
  };
  double const maxs[4] = {
      TinyImageFormat_Max(img->format, TinyImageFormat_LC_Red),
      TinyImageFormat_Max(img->format, TinyImageFormat_LC_Green),
      TinyImageFormat_Max(img->format, TinyImageFormat_LC_Blue),
      TinyImageFormat_Max(img->format, TinyImageFormat_LC_Alpha)
  };

  for (auto s = 0u; s < img->slices; ++s) {
    for (auto z = 0u; z < img->depth; ++z) {
      for (auto y = 0u; y < img->height; ++y) {
        for (auto x = 0u; x < img->width; ++x) {
          Image_PixelD pixel = {double(x), double(y), double(z), double(s)};
          if (TinyImageFormat_IsSigned(img->format)) {
            pixel.r = mins[0] + pixel.r;
            pixel.g = mins[1] + pixel.g;
            pixel.b = mins[2] + pixel.b;
            pixel.a = mins[3] + pixel.a;
          }

          Image_PixelClampD(&pixel, mins, maxs);

          if (TinyImageFormat_IsNormalised(img->format)) {
            pixel.r = pixel.r / maxs[0];
            pixel.g = pixel.g / maxs[1];
            pixel.b = pixel.b / maxs[2];
            pixel.a = pixel.a / maxs[3];
          }
          if (doLog_) {
            LOGINFOF("set<RGBA> %d %d %d %d = %d %d %d %d ",
                     x, y, z, s,
                     pixel.r, pixel.g, pixel.b, pixel.a);
          }

          size_t const index = Image_CalculateIndex(img, x, y, z, s);
          Image_SetPixelAtD(img, &pixel, index);
        }
      }
    }
  }
  for (auto s = 0u; s < img->slices; ++s) {
    for (auto z = 0u; z < img->depth; ++z) {
      for (auto y = 0u; y < img->height; ++y) {
        for (auto x = 0u; x < img->width; ++x) {
          Image_PixelD pixel;

          size_t const index = Image_CalculateIndex(img, x, y, z, s);
          Image_GetPixelAtD(img, &pixel, index);
          if (doLog_) {
            LOGINFOF("get<RGBA> %d %d %d %d = %d %d %d %d ",
                     x, y, z, s,
                     pixel.r, pixel.g, pixel.b, pixel.a);
          }
          Image_PixelD expected = {double(x), double(y), double(z), double(s)};

          if (TinyImageFormat_IsSigned(img->format)) {
            expected.r = mins[0] + expected.r;
            expected.g = mins[1] + expected.g;
            expected.b = mins[2] + expected.b;
            expected.a = mins[3] + expected.a;
          }
          Image_PixelClampD(&expected, mins, maxs);

          if (TinyImageFormat_IsNormalised(img->format)) {
            expected.r = expected.r / maxs[0];
            expected.g = expected.g / maxs[1];
            expected.b = expected.b / maxs[2];
            expected.a = expected.a / maxs[3];
          }

          int8_t swizz[4];
					swizz[0] = TinyImageFormat_LogicalChannelToPhysical(img->format, TinyImageFormat_LC_Red);
					swizz[1] = TinyImageFormat_LogicalChannelToPhysical(img->format, TinyImageFormat_LC_Green);
					swizz[2] = TinyImageFormat_LogicalChannelToPhysical(img->format, TinyImageFormat_LC_Blue);
          swizz[3] = TinyImageFormat_LogicalChannelToPhysical(img->format, TinyImageFormat_LC_Alpha);

					if(swizz[0] == -1) expected.r = 0.0;
					if(swizz[0] == -2) expected.r = 1.0;
					if(swizz[1] == -1) expected.g = 0.0;
					if(swizz[1] == -2) expected.g = 1.0;
					if(swizz[2] == -1) expected.b = 0.0;
					if(swizz[2] == -2) expected.b = 1.0;
					if(swizz[3] == -1) expected.a = 0.0;
					if(swizz[3] == -2) expected.a = 1.0;

          // delibrate fallthrough
          switch (TinyImageFormat_ChannelCount(fmt_)) {
            case 4: CHECK(expected.a == Approx(pixel.a));
            case 3: CHECK(expected.b == Approx(pixel.b));
            case 2: CHECK(expected.g == Approx(pixel.g));
            case 1: CHECK(expected.r == Approx(pixel.r));
              break;
            default:CHECK(TinyImageFormat_ChannelCount(fmt_) <= 4);
          }
        }
      }
    }
  }
  Image_Destroy(img);
}

void ImageTesterFiltered(uint32_t w_, uint32_t h_, uint32_t d_, uint32_t s_, enum TinyImageFormat fmt_, bool doLog_) {
  // Filter the known failures for the generic test runs
  // this formats will emit failures due to 'lossy'ness over the range

  // skip block compressed testing the pixel read/write tests
  if (TinyImageFormat_IsCompressed(fmt_)) { return; }

  // sRGB is a form of lossy compression need to rethink tests in this case
  if (TinyImageFormat_IsSRGB(fmt_)) { return; }

  // small floats are also lossy over the ranges we test against
  if (TinyImageFormat_IsFloat(fmt_) &&
      TinyImageFormat_ChannelBitWidth(fmt_, TinyImageFormat_LC_Red) <= 16) {
    return;
  }

  // mixed normalised / unnormalised format which the tester doesn't handle yet
  if (fmt_ == TinyImageFormat_D24_UNORM_S8_UINT) { return; }
  if (fmt_ == TinyImageFormat_D16_UNORM_S8_UINT) { return; }

  ImageTester(w_, h_, d_, s_, fmt_, doLog_);
}

/*

#define IMAGE_TEST_CASE(FMT, W, H, D, S, DOLOG) \
TEST_CASE( "Image 2D "#FMT" "#W"_"#H"_"#D"_"#S, "[Image]" ) \
{ \
    ImageTester((W), (H), (D), (S), TinyImageFormat_##FMT, DOLOG); \
}
#define IMAGE_TEST_CASE_FILTERED(FMT, W, H, D, S, DOLOG) \
TEST_CASE( "Image 2D "#FMT" "#W"_"#H"_"#D"_"#S, "[Image]" ) \
{ \
    ImageTesterFiltered((W), (H), (D), (S), TinyImageFormat_##FMT, DOLOG); \
}
#define DO_BASIC_TESTS 1

#if DO_BASIC_TESTS == 1

// basic suffix tests
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 1, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_SINT, 10, 1, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_UNORM, 10, 1, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_SNORM, 10, 1, 1, 1, false)

// basic dimension tests
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 10, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 10, 10, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 10, 10, 10, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 1, 10, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 1, 1, 10, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 10, 1, 10, false)

#endif

// format tests
#define TEST_ALL_FORMATS 1
#if TEST_ALL_FORMATS == 0
IMAGE_TEST_CASE(D24_UNORM_S8_UINT, 16, 1, 1, 1, true)
#else

//IMAGE_TEST_CASE(D24_UNORM_S8_UINT, 16, 1, 1, 1, true)
//IMAGE_TEST_CASE(D16_UNORM_S8_UINT, 16, 1, 1, 1, true)
//IMAGE_TEST_CASE(A1R5G5B5_UNORM_PACK16, 16, 1, 1, 1, true)
//IMAGE_TEST_CASE(A2R10G10B10_UNORM_PACK32, 16, 1, 1, 1, true)
//IMAGE_TEST_CASE(A2B10G10R10_UNORM_PACK32, 16, 1, 1, 1, true)

#define IF_START_MACRO
#define IF_MOD_MACRO(x) IMAGE_TEST_CASE_FILTERED(x, 256, 4, 2, 2, false)
#define IF_END_MACRO
//#include "tiny_imageformat/tinyimageformat_base.h"

#endif

 */