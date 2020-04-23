#pragma pack_matrix(row_major)

cbuffer ModelViewProjectionConstantBuffer : register(b0) {
  matrix world;
  matrix view;
  matrix projection;
};

struct VS_INPUT {
  float3 position : POSITION;
  float3 color : COLOR0;
};

struct VS_OUTPUT  {
  float4 position : SV_POSITION;
  float4 color : COLOR0;
};

VS_OUTPUT main(VS_INPUT input) {
  VS_OUTPUT output;

  float4 position = float4(input.position, 1.0f);
  position = mul(position, world);
  position = mul(position, view);
  position = mul(position, projection);
  output.position = position;

  float4 color = float4(input.color, 1.0f);
  output.color = color;

  return output;
}
