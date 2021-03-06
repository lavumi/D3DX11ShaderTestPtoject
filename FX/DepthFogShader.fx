#include "ShaderPreset.hlsli"

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;

};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;

};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    input.position.w = 1;
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _viewXprojection);


    output.uv = input.uv;


    return output;
}



Texture2D _map : register(t10);
Texture2D _depthmap : register(t0);

SamplerState samp[3];
float4 PS(PixelInput input) : SV_Target
{


    float4 main = _map.Sample(samp[0], input.uv);
    float depth = _depthmap.Sample(samp[0], input.uv).r;
                        

    depth = ConvertToLinearDepth(depth) ;
    if (depth > 0.999f)
    {
        depth = 0;
    }

    main += depth * globalAmbient*0.5f;

    return main;
}