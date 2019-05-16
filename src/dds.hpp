#pragma once
#ifndef GFX_IMAGEIO_DDS_HPP
#define GFX_IMAGEIO_DDS_HPP

namespace Image {
/************************************************************************************/
// Define some useful macros
#define MCHAR2(a, b) (a | (b << 8))
#define MAKE_CHAR4(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))

// --- IMAGE HEADERS ---
#pragma pack(push, 1)

#define DDPF_ALPHAPIXELS 0x00000001
#define DDPF_FOURCC 0x00000004
#define DDPF_RGB 0x00000040

#define DDSD_CAPS 0x00000001
#define DDSD_HEIGHT 0x00000002
#define DDSD_WIDTH 0x00000004
#define DDSD_PITCH 0x00000008
#define DDSD_PIXELFORMAT 0x00001000
#define DDSD_MIPMAPCOUNT 0x00020000
#define DDSD_LINEARSIZE 0x00080000
#define DDSD_DEPTH 0x00800000

#define DDSCAPS_COMPLEX 0x00000008
#define DDSCAPS_TEXTURE 0x00001000
#define DDSCAPS_MIPMAP 0x00400000

#define DDSCAPS2_CUBEMAP 0x00000200
#define DDSCAPS2_VOLUME 0x00200000

#define DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000
#define DDSCAPS2_CUBEMAP_ALL_FACES                                                                                       \
    (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | \
     DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)

#define D3D10_RESOURCE_MISC_TEXTURECUBE 0x4
#define D3D10_RESOURCE_DIMENSION_BUFFER 1
#define D3D10_RESOURCE_DIMENSION_TEXTURE1D 2
#define D3D10_RESOURCE_DIMENSION_TEXTURE2D 3
#define D3D10_RESOURCE_DIMENSION_TEXTURE3D 4

struct DDSHeader {
  uint32_t mDWMagic;
  uint32_t mDWSize;
  uint32_t mDWFlags;
  uint32_t mDWHeight;
  uint32_t mDWWidth;
  uint32_t mDWPitchOrLinearSize;
  uint32_t mDWDepth;
  uint32_t mDWMipMapCount;
  uint32_t mReserved[11];

  struct {
    uint32_t mDWSize;
    uint32_t mDWFlags;
    uint32_t mDWFourCC;
    uint32_t mDWRGBBitCount;
    uint32_t mDWRBitMask;
    uint32_t mDWGBitMask;
    uint32_t mDWBBitMask;
    uint32_t mDWRGBAlphaBitMask;
  } mPixelFormat;

  struct {
    uint32_t mDWCaps1;
    uint32_t mDWCaps2;
    uint32_t mReserved[2];    //caps3 and caps4
  } mCaps;

  uint32_t mDWReserved2;
};

struct DDSHeaderDX10 {
  uint32_t mDXGIFormat;
  uint32_t mResourceDimension;
  uint32_t mMiscFlag;
  uint32_t mArraySize;
  uint32_t mReserved;
};
#pragma pack(pop)

enum DDS_DXGI_FORMAT {
  DDS_DXGI_FORMAT_UNKNOWN,
  DDS_DXGI_FORMAT_R32G32B32A32_TYPELESS,
  DDS_DXGI_FORMAT_R32G32B32A32_FLOAT,
  DDS_DXGI_FORMAT_R32G32B32A32_UINT,
  DDS_DXGI_FORMAT_R32G32B32A32_SINT,
  DDS_DXGI_FORMAT_R32G32B32_TYPELESS,
  DDS_DXGI_FORMAT_R32G32B32_FLOAT,
  DDS_DXGI_FORMAT_R32G32B32_UINT,
  DDS_DXGI_FORMAT_R32G32B32_SINT,
  DDS_DXGI_FORMAT_R16G16B16A16_TYPELESS,
  DDS_DXGI_FORMAT_R16G16B16A16_FLOAT,
  DDS_DXGI_FORMAT_R16G16B16A16_UNORM,
  DDS_DXGI_FORMAT_R16G16B16A16_UINT,
  DDS_DXGI_FORMAT_R16G16B16A16_SNORM,
  DDS_DXGI_FORMAT_R16G16B16A16_SINT,
  DDS_DXGI_FORMAT_R32G32_TYPELESS,
  DDS_DXGI_FORMAT_R32G32_FLOAT,
  DDS_DXGI_FORMAT_R32G32_UINT,
  DDS_DXGI_FORMAT_R32G32_SINT,
  DDS_DXGI_FORMAT_R32G8X24_TYPELESS,
  DDS_DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
  DDS_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
  DDS_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
  DDS_DXGI_FORMAT_R10G10B10A2_TYPELESS,
  DDS_DXGI_FORMAT_R10G10B10A2_UNORM,
  DDS_DXGI_FORMAT_R10G10B10A2_UINT,
  DDS_DXGI_FORMAT_R11G11B10_FLOAT,
  DDS_DXGI_FORMAT_R8G8B8A8_TYPELESS,
  DDS_DXGI_FORMAT_R8G8B8A8_UNORM,
  DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
  DDS_DXGI_FORMAT_R8G8B8A8_UINT,
  DDS_DXGI_FORMAT_R8G8B8A8_SNORM,
  DDS_DXGI_FORMAT_R8G8B8A8_SINT,
  DDS_DXGI_FORMAT_R16G16_TYPELESS,
  DDS_DXGI_FORMAT_R16G16_FLOAT,
  DDS_DXGI_FORMAT_R16G16_UNORM,
  DDS_DXGI_FORMAT_R16G16_UINT,
  DDS_DXGI_FORMAT_R16G16_SNORM,
  DDS_DXGI_FORMAT_R16G16_SINT,
  DDS_DXGI_FORMAT_R32_TYPELESS,
  DDS_DXGI_FORMAT_D32_FLOAT,
  DDS_DXGI_FORMAT_R32_FLOAT,
  DDS_DXGI_FORMAT_R32_UINT,
  DDS_DXGI_FORMAT_R32_SINT,
  DDS_DXGI_FORMAT_R24G8_TYPELESS,
  DDS_DXGI_FORMAT_D24_UNORM_S8_UINT,
  DDS_DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
  DDS_DXGI_FORMAT_X24_TYPELESS_G8_UINT,
  DDS_DXGI_FORMAT_R8G8_TYPELESS,
  DDS_DXGI_FORMAT_R8G8_UNORM,
  DDS_DXGI_FORMAT_R8G8_UINT,
  DDS_DXGI_FORMAT_R8G8_SNORM,
  DDS_DXGI_FORMAT_R8G8_SINT,
  DDS_DXGI_FORMAT_R16_TYPELESS,
  DDS_DXGI_FORMAT_R16_FLOAT,
  DDS_DXGI_FORMAT_D16_UNORM,
  DDS_DXGI_FORMAT_R16_UNORM,
  DDS_DXGI_FORMAT_R16_UINT,
  DDS_DXGI_FORMAT_R16_SNORM,
  DDS_DXGI_FORMAT_R16_SINT,
  DDS_DXGI_FORMAT_R8_TYPELESS,
  DDS_DXGI_FORMAT_R8_UNORM,
  DDS_DXGI_FORMAT_R8_UINT,
  DDS_DXGI_FORMAT_R8_SNORM,
  DDS_DXGI_FORMAT_R8_SINT,
  DDS_DXGI_FORMAT_A8_UNORM,
  DDS_DXGI_FORMAT_R1_UNORM,
  DDS_DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
  DDS_DXGI_FORMAT_R8G8_B8G8_UNORM,
  DDS_DXGI_FORMAT_G8R8_G8B8_UNORM,
  DDS_DXGI_FORMAT_BC1_TYPELESS,
  DDS_DXGI_FORMAT_BC1_UNORM,
  DDS_DXGI_FORMAT_BC1_UNORM_SRGB,
  DDS_DXGI_FORMAT_BC2_TYPELESS,
  DDS_DXGI_FORMAT_BC2_UNORM,
  DDS_DXGI_FORMAT_BC2_UNORM_SRGB,
  DDS_DXGI_FORMAT_BC3_TYPELESS,
  DDS_DXGI_FORMAT_BC3_UNORM,
  DDS_DXGI_FORMAT_BC3_UNORM_SRGB,
  DDS_DXGI_FORMAT_BC4_TYPELESS,
  DDS_DXGI_FORMAT_BC4_UNORM,
  DDS_DXGI_FORMAT_BC4_SNORM,
  DDS_DXGI_FORMAT_BC5_TYPELESS,
  DDS_DXGI_FORMAT_BC5_UNORM,
  DDS_DXGI_FORMAT_BC5_SNORM,
  DDS_DXGI_FORMAT_B5G6R5_UNORM,
  DDS_DXGI_FORMAT_B5G5R5A1_UNORM,
  DDS_DXGI_FORMAT_B8G8R8A8_UNORM,
  DDS_DXGI_FORMAT_B8G8R8X8_UNORM,
  DDS_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
  DDS_DXGI_FORMAT_B8G8R8A8_TYPELESS,
  DDS_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
  DDS_DXGI_FORMAT_B8G8R8X8_TYPELESS,
  DDS_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
  DDS_DXGI_FORMAT_BC6H_TYPELESS,
  DDS_DXGI_FORMAT_BC6H_UF16,
  DDS_DXGI_FORMAT_BC6H_SF16,
  DDS_DXGI_FORMAT_BC7_TYPELESS,
  DDS_DXGI_FORMAT_BC7_UNORM,
  DDS_DXGI_FORMAT_BC7_UNORM_SRGB,
  DDS_DXGI_FORMAT_AYUV,
  DDS_DXGI_FORMAT_Y410,
  DDS_DXGI_FORMAT_Y416,
  DDS_DXGI_FORMAT_NV12,
  DDS_DXGI_FORMAT_P010,
  DDS_DXGI_FORMAT_P016,
  DDS_DXGI_FORMAT_420_OPAQUE,
  DDS_DXGI_FORMAT_YUY2,
  DDS_DXGI_FORMAT_Y210,
  DDS_DXGI_FORMAT_Y216,
  DDS_DXGI_FORMAT_NV11,
  DDS_DXGI_FORMAT_AI44,
  DDS_DXGI_FORMAT_IA44,
  DDS_DXGI_FORMAT_P8,
  DDS_DXGI_FORMAT_A8P8,
  DDS_DXGI_FORMAT_B4G4R4A4_UNORM,
  DDS_DXGI_FORMAT_P208,
  DDS_DXGI_FORMAT_V208,
  DDS_DXGI_FORMAT_V408,
  DDS_DXGI_FORMAT_FORCE_UINT
};


} // end namespace Image

#endif //WYRD_IMAGE_DDS_HPP
