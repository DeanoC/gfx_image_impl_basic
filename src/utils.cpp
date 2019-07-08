#include "al2o3_platform/platform.h"
#include "gfx_imageformat/format.h"
#include "gfx_imageformat/formatcracker.h"
#include "gfx_image/image.h"
#include "gfx_image/utils.h"
#include "hq_resample.hpp"

AL2O3_EXTERN_C bool Image_GetColorRangeOf(Image_ImageHeader const *src, Image_PixelD *omin, Image_PixelD *omax) {
	ASSERT(src);
	ASSERT(omin);
	ASSERT(omax);

	double *minData = &omin->r;
	double *maxData = &omax->r;
	for (uint32_t i = 0u; i < ImageFormat_ChannelCount(src->format); ++i) {
		minData[i] = ImageFormat_Max(src->format, i);
		maxData[i] = ImageFormat_Min(src->format, i);
	};

	for (auto w = 0u; w < src->slices; ++w) {
		for (auto z = 0u; z < src->depth; ++z) {
			for (auto y = 0u; y < src->height; ++y) {
				for (auto x = 0u; x < src->width; ++x) {
					size_t const index = Image_CalculateIndex(src, x, y, z, w);
					Image_PixelD pixel;
					Image_GetPixelAt(src, &pixel, index);

					double *data = &pixel.r;
					for (uint32_t i = 0u; i < ImageFormat_ChannelCount(src->format); ++i) {
						if (data[i] < minData[i]) {
							minData[i] = data[i];
						}
						if (data[i] > maxData[i]) {
							maxData[i] = data[i];
						}
					}
				}
			}
		}
	}

	return true;
}

AL2O3_EXTERN_C bool Image_GetColorRangeOfD(Image_ImageHeader const *src, double *omin, double *omax) {

	Image_PixelD pixelMin;
	Image_PixelD pixelMax;

	ASSERT(src);
	ASSERT(omin);
	ASSERT(omax);

	if (Image_GetColorRangeOf(src, &pixelMin, &pixelMax)) {
		double *dmin = &pixelMin.r;
		double *dmax = &pixelMax.r;

		for (uint32_t i = 1u; i < ImageFormat_ChannelCount(src->format); ++i) {
			if (dmin[i] < dmin[0]) {
				dmin[0] = dmin[i];
			}
			if (dmax[i] > dmax[0]) {
				dmax[i] = dmax[i];
			}
		}

		*omin = dmin[0];
		*omax = dmax[0];
		return true;
	}

	return false;
}

AL2O3_EXTERN_C bool Image_GetColorRangeOfF(Image_ImageHeader const *src, float *omin, float *omax) {

	double dmin, dmax;
	if (Image_GetColorRangeOfD(src, &dmin, &dmax)) {
		*omin = (float) dmin;
		*omax = (float) dmax;
		return true;
	}
	return false;
}

AL2O3_EXTERN_C bool Image_NormalizeEachChannel(Image_ImageHeader const *src) {
	Image_PixelD pmin, pmax;
	if (!Image_GetColorRangeOf(src, &pmin, &pmax)) {
		return false;
	}

	Image_PixelD const s{
			1.0 / (pmax.r - pmin.r),
			1.0 / (pmax.g - pmin.g),
			1.0 / (pmax.b - pmin.b),
			1.0 / (pmax.a - pmin.a),
	};

	Image_PixelD const b{
			-pmin.r * s.r,
			-pmin.g * s.g,
			-pmin.b * s.b,
			-pmin.a * s.a,
	};

	for (auto w = 0u; w < src->slices; ++w) {
		for (auto z = 0u; z < src->depth; ++z) {
			for (auto y = 0u; y < src->height; ++y) {
				for (auto x = 0u; x < src->width; ++x) {
					size_t const index = Image_CalculateIndex(src, x, y, z, w);
					Image_PixelD pixel;
					Image_GetPixelAt(src, &pixel, index);
					pixel.r = pixel.r * s.r + b.r;
					pixel.g = pixel.g * s.g + b.g;
					pixel.b = pixel.b * s.b + b.b;
					pixel.a = pixel.a * s.a + b.a;
					Image_SetPixelAt(src, &pixel, index);
				}
			}
		}
	}
	return true;
}

AL2O3_EXTERN_C bool Image_NormalizeAcrossChannels(Image_ImageHeader const *src) {
	double dmin, dmax;
	if (!Image_GetColorRangeOfD(src, &dmin, &dmax)) {
		return false;
	}

	double const s = 1.0 / (dmax - dmin);
	double const b = -dmin * s;

	for (auto w = 0u; w < src->slices; ++w) {
		for (auto z = 0u; z < src->depth; ++z) {
			for (auto y = 0u; y < src->height; ++y) {
				for (auto x = 0u; x < src->width; ++x) {
					size_t const index = Image_CalculateIndex(src, x, y, z, w);
					Image_PixelD pixel;
					Image_GetPixelAt(src, &pixel, index);
					pixel.r = pixel.r * s + b;
					pixel.g = pixel.g * s + b;
					pixel.b = pixel.b * s + b;
					pixel.a = pixel.a * s + b;
					Image_SetPixelAt(src, &pixel, index);
				}
			}
		}
	}
	return true;
}
// TODO optimise or have option for faster mipmap chain generation
AL2O3_EXTERN_C void Image_CreateMipMapChain(Image_ImageHeader const * image, bool generateFromImage) {
	// start from the image provided and create successive mip images
	ASSERT(image->nextType == Image_NT_None);
	ASSERT(Math_IsPowerOf2U32(image->width));
	ASSERT(Math_IsPowerOf2U32(image->height));

	// need to think about mip mapped volume textures...
	ASSERT(image->depth == 1);

	using namespace Image;

	Image_ImageHeader * curImage = (Image_ImageHeader*)image;
	uint32_t curWidth = image->width;
	uint32_t curHeight = image->height;
	if (curWidth <= 1 || curHeight <= 1) {
		return;
	}

	Image_ImageHeader const * scratchImage = nullptr;

	uint32_t const numChans = ImageFormat_ChannelCount(image->format);
	if (generateFromImage) {
		ImageFormat dblFmt = ImageFormat_R32G32B32A32_SFLOAT;
		switch (numChans) {
		case 1:dblFmt = ImageFormat_R32_SFLOAT;
			break;
		case 2:dblFmt = ImageFormat_R32G32_SFLOAT;
			break;
		case 3:dblFmt = ImageFormat_R32G32B32_SFLOAT;
			break;
		case 4:dblFmt = ImageFormat_R32G32B32A32_SFLOAT;
			break;
		default:
		case 0: {
			ASSERT(false);
		}
		}
		scratchImage = Image_Create(image->width / 2, image->height / 2, 1, 1, dblFmt);
	}

	do {
		curWidth = curWidth / 2;
		curHeight = curHeight / 2;

		auto newImage = Image_Create(curWidth, curHeight, 1, image->slices, image->format);

		if (generateFromImage) {
			float *const scratch = (float *const) Image_RawDataPtr(scratchImage);

			for (auto w = 0u; w < image->slices; ++w) {
				float const *origSlice = (float const *)
						(((uint8_t *) Image_RawDataPtr(image)) + w * Image_ByteCountPerSliceOf(image));

				hq_resample<float>(numChans,
													 origSlice, image->width, image->height,
													 scratch, curWidth, curHeight);

				Image_CopySlice(scratchImage, w, newImage, 0);
			}
		}

		curImage->nextImage = (Image_ImageHeader*)newImage;
		curImage->nextType = Image_NT_MipMaps;
		curImage = (Image_ImageHeader*)curImage->nextImage;
	} while (curWidth > 1 || curHeight > 1);

	if (scratchImage) {
		Image_Destroy(scratchImage);
	}
}

AL2O3_EXTERN_C void Image_CopyImageChain(Image_ImageHeader const *src, Image_ImageHeader const * dst) {
	Image_CopyImage(src, dst);
	if (src->nextType == dst->nextType && src->nextImage && dst->nextImage) {
		Image_CopyImageChain(src->nextImage, dst->nextImage);
	}
}

AL2O3_EXTERN_C void Image_CopyImage(Image_ImageHeader const * src, Image_ImageHeader const * dst) {
	if (src == dst) {
		return;
	}

	ASSERT(dst->slices == src->slices);
	ASSERT(dst->depth == src->depth);
	ASSERT(dst->height == src->height);
	ASSERT(dst->width == src->width);

	for (auto w = 0u; w < src->slices; ++w) {
		for (auto z = 0u; z < src->depth; ++z) {
			for (auto y = 0u; y < src->height; ++y) {
				for (auto x = 0u; x < src->width; ++x) {
					size_t const index = Image_CalculateIndex(src, x, y, z, w);
					Image_PixelD pixel;
					Image_GetPixelAt(src, &pixel, index);
					Image_SetPixelAt(dst, &pixel, index);
				}
			}
		}
	}
}

AL2O3_EXTERN_C void Image_CopySlice(
		Image_ImageHeader const * src,
		uint32_t sw,
		Image_ImageHeader const * dst,
		uint32_t dw) {
	ASSERT(dst->depth == src->depth);
	ASSERT(dst->height == src->height);
	ASSERT(dst->width == src->width);
	if (dst == src) {
		ASSERT(dw != sw);
	}

	for (auto z = 0u; z < src->depth; ++z) {
		for (auto y = 0u; y < src->height; ++y) {
			for (auto x = 0u; x < src->width; ++x) {
				size_t const srcIndex = Image_CalculateIndex(src, x, y, z, sw);
				size_t const dstIndex = Image_CalculateIndex(src, x, y, z, dw);
				Image_PixelD pixel;
				Image_GetPixelAt(src, &pixel, srcIndex);
				Image_SetPixelAt(dst, &pixel, dstIndex);
			}
		}
	}
}

AL2O3_EXTERN_C void Image_CopyPage(
		Image_ImageHeader const * src,
		uint32_t sz, uint32_t sw,
		Image_ImageHeader const * dst,
		uint32_t dz, uint32_t dw) {
	ASSERT(dst->height == src->height);
	ASSERT(dst->width == src->width);
	if (dst == src) {
		ASSERT(dz != sz || dw != sw);
	}

	for (auto y = 0u; y < src->height; ++y) {
		for (auto x = 0u; x < src->width; ++x) {
			size_t const srcIndex = Image_CalculateIndex(src, x, y, sz, sw);
			size_t const dstIndex = Image_CalculateIndex(src, x, y, dz, dw);
			Image_PixelD pixel;
			Image_GetPixelAt(src, &pixel, srcIndex);
			Image_SetPixelAt(dst, &pixel, dstIndex);
		}
	}
}

AL2O3_EXTERN_C void Image_CopyRow(Image_ImageHeader const * src,
																	uint32_t sy, uint32_t sz, uint32_t sw,
																	Image_ImageHeader const * dst,
																	uint32_t dy, uint32_t dz, uint32_t dw) {
	ASSERT(dst->width == src->width);
	if (dst == src) {
		ASSERT(dy != sy || dz != sz || dw != sw);
	}

	for (auto x = 0u; x < src->width; ++x) {
		size_t const srcIndex = Image_CalculateIndex(src, x, sy, sz, sw);
		size_t const dstIndex = Image_CalculateIndex(src, x, dy, dz, dw);
		Image_PixelD pixel;
		Image_GetPixelAt(src, &pixel, srcIndex);
		Image_SetPixelAt(dst, &pixel, dstIndex);
	}
}

AL2O3_EXTERN_C void Image_CopyPixel(Image_ImageHeader const * src,
																		uint32_t sx, uint32_t sy, uint32_t sz, uint32_t sw,
																		Image_ImageHeader const * dst,
																		uint32_t dx, uint32_t dy, uint32_t dz, uint32_t dw) {
	size_t const srcIndex = Image_CalculateIndex(src, sx, sy, sz, sw);
	size_t const dstIndex = Image_CalculateIndex(src, dx, dy, dz, dw);
	Image_PixelD pixel;
	Image_GetPixelAt(src, &pixel, srcIndex);
	Image_SetPixelAt(dst, &pixel, dstIndex);
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_Clone(Image_ImageHeader const *image) {
	auto dst = (Image_ImageHeader *) Image_Create(image->width, image->height, image->depth, image->slices, image->format);
	if (dst == nullptr) {
		return nullptr;
	}
	Image_CopyImage(image, dst);
	if (image->nextType != Image_NT_None) {
		dst->nextImage = Image_Clone(image->nextImage);
		dst->nextType = image->nextType;
	}
	return dst;
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_CloneStructure(Image_ImageHeader const * image) {
	auto dst = (Image_ImageHeader *) Image_Create(image->width, image->height, image->depth, image->slices, image->format);
	if (dst == nullptr) {
		return nullptr;
	}
	if (image->nextType != Image_NT_None) {
		dst->nextImage = Image_CloneStructure(image->nextImage);
		dst->nextType = image->nextType;
	}
	return dst;
}

AL2O3_EXTERN_C Image_ImageHeader const * Image_PreciseConvert(Image_ImageHeader const * image, ImageFormat const newFormat) {
	auto dst = (Image_ImageHeader *)Image_Create(image->width, image->height, image->depth, image->slices, image->format);
	if (dst == nullptr) {
		return nullptr;
	}
	Image_CopyImage(image, dst);
	if (image->nextType != Image_NT_None) {
		dst->nextImage = Image_PreciseConvert(image->nextImage, newFormat);
		dst->nextType = image->nextType;
	}
	return dst;
}

