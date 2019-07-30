#include <string>
#include <vector>

#include "Common/MsgHandler.h"
#include "Common/Logging/Log.h"
#include "VideoCommon/AbstractFramebuffer.h"
#include "VideoCommon/AbstractTexture.h"
#include "VideoCommon/RasterFont.h"
#include "VideoCommon/RenderBase.h"
#include "VideoCommon/VertexManagerBase.h"
#include "VideoCommon/VideoConfig.h"

static const int CHARACTER_WIDTH = 8;
static const int CHARACTER_HEIGHT = 13;
static const int CHARACTER_OFFSET = 32;
static const int CHARACTER_COUNT = 95;

static const u8 rasters[CHARACTER_COUNT][CHARACTER_HEIGHT] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36},
    {0x00, 0x00, 0x00, 0x66, 0x66, 0xff, 0x66, 0x66, 0xff, 0x66, 0x66, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x7e, 0xff, 0x1b, 0x1f, 0x7e, 0xf8, 0xd8, 0xff, 0x7e, 0x18},
    {0x00, 0x00, 0x0e, 0x1b, 0xdb, 0x6e, 0x30, 0x18, 0x0c, 0x76, 0xdb, 0xd8, 0x70},
    {0x00, 0x00, 0x7f, 0xc6, 0xcf, 0xd8, 0x70, 0x70, 0xd8, 0xcc, 0xcc, 0x6c, 0x38},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1c, 0x0c, 0x0e},
    {0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c},
    {0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x30},
    {0x00, 0x00, 0x00, 0x00, 0x99, 0x5a, 0x3c, 0xff, 0x3c, 0x5a, 0x99, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18, 0x00, 0x00},
    {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x03, 0x03},
    {0x00, 0x00, 0x3c, 0x66, 0xc3, 0xe3, 0xf3, 0xdb, 0xcf, 0xc7, 0xc3, 0x66, 0x3c},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x38, 0x18},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0xe7, 0x7e},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0x07, 0x03, 0x03, 0xe7, 0x7e},
    {0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xff, 0xcc, 0x6c, 0x3c, 0x1c, 0x0c},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x03, 0x03, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x03, 0x7f, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e},
    {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x1c, 0x1c, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06},
    {0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60},
    {0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x18, 0x0c, 0x06, 0x03, 0xc3, 0xc3, 0x7e},
    {0x00, 0x00, 0x3f, 0x60, 0xcf, 0xdb, 0xd3, 0xdd, 0xc3, 0x7e, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18},
    {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e},
    {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3},
    {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff},
    {0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c},
    {0x00, 0x03, 0x03, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60},
    {0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18},
    {0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x30, 0x70},
    {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0x7f, 0x03, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0x7e, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x03, 0x03, 0x03, 0x03, 0x03},
    {0x00, 0x00, 0x7f, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x33, 0x1e},
    {0x7e, 0xc3, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00},
    {0x38, 0x6c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x0c, 0x00},
    {0x00, 0x00, 0xc6, 0xcc, 0xf8, 0xf0, 0xd8, 0xcc, 0xc6, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78},
    {0x00, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xfe, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xfc, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00},
    {0xc0, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0x00, 0x00, 0x00, 0x00},
    {0x03, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xfe, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xfe, 0x03, 0x03, 0x7e, 0xc0, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x1c, 0x36, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x00},
    {0x00, 0x00, 0x7e, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xdb, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00},
    {0xc0, 0x60, 0x60, 0x30, 0x18, 0x3c, 0x66, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xff, 0x60, 0x30, 0x18, 0x0c, 0x06, 0xff, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x0f, 0x18, 0x18, 0x18, 0x38, 0xf0, 0x38, 0x18, 0x18, 0x18, 0x0f},
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
    {0x00, 0x00, 0xf0, 0x18, 0x18, 0x18, 0x1c, 0x0f, 0x1c, 0x18, 0x18, 0x18, 0xf0},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x8f, 0xf1, 0x60, 0x00, 0x00, 0x00}};

namespace VideoCommon
{

bool RasterFont::Initialize(AbstractTextureFormat format)
{
  m_framebuffer_format = format;
  return CreateTexture() && CreatePipeline();
}

bool RasterFont::CreateTexture()
{
  std::vector<u32> texture_data;
  texture_data.reserve(CHARACTER_WIDTH * CHARACTER_COUNT * CHARACTER_HEIGHT);
  for (int y = 0; y < CHARACTER_HEIGHT; y++)
  {
    for (int c = 0; c < CHARACTER_COUNT; c++)
    {
      for (int x = 0; x < CHARACTER_WIDTH; x++)
      {
        bool pixel = (0 != (rasters[c][y] & (1 << (CHARACTER_WIDTH - x - 1))));
        texture_data[CHARACTER_WIDTH * CHARACTER_COUNT * y + CHARACTER_WIDTH * c + x] =
            pixel ? -1 : 0;
      }
    }
  }

  u32 width = CHARACTER_WIDTH * CHARACTER_COUNT;
  u32 height = CHARACTER_HEIGHT;

  TextureConfig tex_config(width, height, 1, 1, 1, AbstractTextureFormat::RGBA8, 0);
  m_texture = g_renderer->CreateTexture(tex_config);
  if (!m_texture)
  {
    PanicAlert("Failed to create texture");
    return false;
  }

  m_texture->Load(0, width, height, width, (const u8*)texture_data.data(),
                 sizeof(u32) * width * height);

  return true;
}

bool RasterFont::CreatePipeline()
{
  const APIType api_type = g_ActiveConfig.backend_info.api_type;
  std::stringstream ss;

  // Vertex uniform buffer
  if (api_type == APIType::D3D)
    ss << "cbuffer PSBlock : register(b0)\n";
  else
    ss << "UBO_BINDING(std140, 1) uniform PSBlock\n";

  ss << "{\n";
  ss << "  float2 char_size;\n";
  ss << "  float2 offset;\n";
  ss << "  float4 color;\n";
  ss << "};\n";

  if (api_type == APIType::D3D)
  {
    ss << "void main(in float2 rawpos : POSITION,\n"
       << "          in float2 rawtex0 : TEXCOORD,\n"
       << "          out float3 frag_uv : TEXCOORD,\n"
       << "          out float4 out_pos : SV_Position) {\n";
  }
  else
  {
    ss << "ATTRIBUTE_LOCATION(" << SHADER_POSITION_ATTRIB << ") in float2 rawpos;\n"
       << "ATTRIBUTE_LOCATION(" << SHADER_TEXTURE0_ATTRIB << ") in float2 rawtex0;\n"
       << "VARYING_LOCATION(0) out float3 frag_uv;\n"
       << "#define out_pos gl_Position\n"
       << "void main() {\n";
  }

  ss << "  frag_uv = float3(rawtex0 * char_size, 0.0);\n"
     << "  out_pos = float4(rawpos + offset, 0.0, 1.0);\n";
  // Clip-space is flipped in Vulkan
  if (api_type == APIType::Vulkan)
    ss << "  out_pos.y = -out_pos.y;\n";
  ss << "}\n";

  std::unique_ptr<AbstractShader> vertex_shader =
      g_renderer->CreateShaderFromSource(ShaderStage::Vertex, ss.str());
  if (!vertex_shader)
  {
    PanicAlert("Failed to compile vertex shader");
    return false;
  }

  // Pixel uniform buffer
  ss.str("");
  ss.clear();
  if (api_type == APIType::D3D)
    ss << "cbuffer PSBlock : register(b0)\n";
  else
    ss << "UBO_BINDING(std140, 1) uniform PSBlock\n";

  ss << "{\n";
  ss << "  float2 char_size;\n";
  ss << "  float2 offset;\n";
  ss << "  float4 color;\n";
  ss << "};\n";

  if (api_type == APIType::D3D)
  {
    ss << "Texture2DArray tex0 : register(t0);\n"
       << "SamplerState samp0 : register(s0);\n"
       << "void main(in float3 frag_uv : TEXCOORD,\n"
       << "          out float4 ocol0 : SV_Target) {\n";
  }
  else
  {
    ss << "SAMPLER_BINDING(0) uniform sampler2DArray samp0;\n"
       << "VARYING_LOCATION(0) in float3 frag_uv; \n"
       << "FRAGMENT_OUTPUT_LOCATION(0) out float4 ocol0;\n"
       << "void main() {\n";
  }

  if (api_type == APIType::D3D)
    ss << "  ocol0 = tex0.Sample(samp0, frag_uv) * color;\n";
  else
    ss << "  ocol0 = texture(samp0, frag_uv) * color;\n";
  ss << "}\n";

  std::unique_ptr<AbstractShader> pixel_shader =
      g_renderer->CreateShaderFromSource(ShaderStage::Pixel, ss.str());
  if (!pixel_shader)
  {
    PanicAlert("Failed to compile pixel shader");
    return false;
  }


  PortableVertexDeclaration vdecl = {};
  vdecl.position = {VAR_FLOAT, 2, 0, true, false};
  vdecl.texcoords[0] = {VAR_FLOAT, 2, sizeof(float) * 2, true, false};
  vdecl.stride = sizeof(float) * 4;
  m_vertex_format = g_renderer->CreateNativeVertexFormat(vdecl);
  if (!m_vertex_format)
  {
    PanicAlert("Failed to create vertex format");
    return false;
  }

  AbstractPipelineConfig pconfig = {};
  pconfig.vertex_format = m_vertex_format.get();
  pconfig.vertex_shader = vertex_shader.get();
  pconfig.pixel_shader = pixel_shader.get();
  pconfig.rasterization_state =
      RenderState::GetCullBackFaceRasterizationState(PrimitiveType::Triangles);
  pconfig.depth_state = RenderState::GetNoDepthTestingDepthState();
  pconfig.blending_state = RenderState::GetNoBlendingBlendState();
  pconfig.blending_state.blendenable = true;
  pconfig.blending_state.srcfactor = BlendMode::SRCALPHA;
  pconfig.blending_state.dstfactor = BlendMode::INVSRCALPHA;
  pconfig.blending_state.srcfactoralpha = BlendMode::ZERO;
  pconfig.blending_state.dstfactoralpha = BlendMode::ONE;
  pconfig.framebuffer_state = RenderState::GetColorFramebufferState(m_framebuffer_format);
  pconfig.usage = AbstractPipelineUsage::Utility;
  m_pipeline = g_renderer->CreatePipeline(pconfig);
  if (!m_pipeline)
  {
    PanicAlert("Failed to create pipeline");
    return false;
  }

  return true;
}

void RasterFont::Prepare()
{
  if (g_renderer->GetCurrentFramebuffer()->GetColorFormat() != m_framebuffer_format)
  {
    WARN_LOG(VIDEO, "RasterFont Prepare color format mismatch.");
  }
  // Set up common state for drawing.
  int bbWidth = g_renderer->GetBackbufferWidth();
  int bbHeight = g_renderer->GetBackbufferHeight();
  g_renderer->SetViewportAndScissor(MathUtil::Rectangle<int>(0, 0, bbWidth, bbHeight), 0.0f, 1.0f);
  g_renderer->SetPipeline(m_pipeline.get());
  g_renderer->SetTexture(0, m_texture.get());
  g_renderer->SetSamplerState(0, RenderState::GetPointSamplerState());
}

void RasterFont::Draw(const std::string& text, float start_x, float start_y, u32 color)
{
  float screen_width = g_renderer->GetBackbufferWidth();
  float screen_height = g_renderer->GetBackbufferHeight();
  float scale = g_renderer->GetBackbufferScale();
#ifndef ANDROID
  scale *= 2.0f;
#endif

  std::vector<float> vertices(text.length() * 6 * 4);
  u32 usage = 0;
  float delta_x = scale * CHARACTER_WIDTH / screen_width;
  float delta_y = scale * CHARACTER_HEIGHT / screen_height;
  float border_x = scale * 2.0f / screen_width;
  float border_y = scale * 4.0f / screen_height;
  float x = scale * start_x * 2.0f / screen_width - 1.0f;
  float y = 1.0f - scale * start_y * 2.0f / screen_height;

  for (const char& c : text)
  {
    if (c == '\n')
    {
      x = start_x;
      y -= delta_y + border_y;
      continue;
    }

    // do not print spaces, they can be skipped easily
    if (c == ' ')
    {
      x += delta_x + border_x;
      continue;
    }

    if (c < CHARACTER_OFFSET || c >= CHARACTER_COUNT + CHARACTER_OFFSET)
      continue;

    vertices[usage++] = x;
    vertices[usage++] = y;
    vertices[usage++] = float(c - CHARACTER_OFFSET);
    vertices[usage++] = 0.0f;

    vertices[usage++] = x + delta_x;
    vertices[usage++] = y + delta_y;
    vertices[usage++] = float(c - CHARACTER_OFFSET + 1);
    vertices[usage++] = 1.0f;

    vertices[usage++] = x + delta_x;
    vertices[usage++] = y;
    vertices[usage++] = float(c - CHARACTER_OFFSET + 1);
    vertices[usage++] = 0.0f;

    vertices[usage++] = x;
    vertices[usage++] = y;
    vertices[usage++] = float(c - CHARACTER_OFFSET);
    vertices[usage++] = 0.0f;

    vertices[usage++] = x;
    vertices[usage++] = y + delta_y;
    vertices[usage++] = float(c - CHARACTER_OFFSET);
    vertices[usage++] = 1.0f;

    vertices[usage++] = x + delta_x;
    vertices[usage++] = y + delta_y;
    vertices[usage++] = float(c - CHARACTER_OFFSET + 1);
    vertices[usage++] = 1.0f;

    x += delta_x + border_x;
  }

  u32 vertex_stride = sizeof(float) * 4;
  u32 num_vertices = usage / 4;
  const u16* indices = nullptr;
  u32 num_indices = 0;
  u32 out_base_vertex = 0;
  u32 out_base_index = 0;
  g_vertex_manager->UploadUtilityVertices(vertices.data(), vertex_stride, num_vertices, indices,
                                          num_indices, &out_base_vertex, &out_base_index);

  struct PCBlock
  {
    float char_size[2];
    float offset[2];
    float color[4];
  } pc_block = {};

  pc_block.char_size[0] = 1.0f / static_cast<float>(CHARACTER_COUNT);
  pc_block.char_size[1] = 1.0f;

  // shadows
  pc_block.offset[0] = 2.0f / screen_width;
  pc_block.offset[1] = -2.0f / screen_height;
  pc_block.color[3] = (color >> 24) / 255.0f;
  g_vertex_manager->UploadUtilityUniforms(&pc_block, sizeof(pc_block));
  g_renderer->Draw(out_base_vertex, num_vertices);

  // non-shadowed part
  pc_block.offset[0] = 0.0f;
  pc_block.offset[1] = 0.0f;
  pc_block.color[0] = ((color >> 16) & 0xFF) / 255.0f;
  pc_block.color[1] = ((color >> 8) & 0xFF) / 255.0f;
  pc_block.color[2] = (color & 0xFF) / 255.0f;
  g_vertex_manager->UploadUtilityUniforms(&pc_block, sizeof(pc_block));
  g_renderer->Draw(out_base_vertex, num_vertices);
}
} // namespace VideoCommon