#include "ShaderPreset.hlsli"


struct VertexInput
{
    float4 position : POSITION0;
//   float4 world0 : INSTMATRIX0;
//   float4 world1 : INSTMATRIX1;
//   float4 world2 : INSTMATRIX2;
//   float4 world3 : INSTMATRIX3;
};

struct PixelInput
{
    float4 position : SV_POSITION;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    input.position.w = 1;

//    float4x4 world = float4x4(input.world0, input.world1, input.world2, input.world3);
   // output.position = mul(input.position, world);
    output.position = input.position;
    return output;
}


Texture2D _map : register(t0);
SamplerState samp[3];

cbuffer GsData : register(b0)
{
    float _grassHeight;
    float _grassWidth;
    float _timer;
    float _wind;
}



[maxvertexcount(256)]
void GS(point PixelInput input[1], inout TriangleStream<PixelInput> triStream)
{
  

    float grassheight = _grassHeight;
    float grasswidth = _grassWidth;
    
    PixelInput base = input[0];

  // int rndSeed = base.position.x * 4 + base.position.z * 62;
  // rndSeed %= 36;


   // float2 uv = input[0].position.xz / 100;
   // float height = _map.SampleLevel(samp[0], uv, 0);
   //
   // grassheight += height * 0.3f;


    float2 heightuv = base.position.xz / 256;
    //�̹����� ���� uv ��ǥ ó��
    heightuv.y *= -1;
    heightuv.y += 1;
    
    base.position.y = _map.SampleLevel(samp[0], heightuv, 0).r * 34.0f - 18.0f;


    PixelInput output;
    float4 lightWorldPosition;

    //for (int i = 0; i < 6; i++)
    //{
     //  base.position.x += 1.0 / 6;
     //  base.position.z -= 1;
       //for (int j = 0; j < 6; j++)
       //{
      //      base.position.z += 1.0 / 6;
            output = base;

           //int rndX, rndZ;
           //rndX = (i * 6 + j + rndSeed) % 36;
           //rndZ = (i + j * 6 + rndSeed) % 36;

          //  output.position.xz += float2(rndTable[rndX], rndTable[rndZ]);
    output.position = mul(output.position, _lightView);
    output.position = mul(output.position, _lightProjection);
    output.position = mul(output.position, cropMatrix[0]);

            triStream.Append(output);
    
            output = base;
         //   output.position.xz += float2(rndTable[rndX], rndTable[rndZ]);
            output.position.y += grassheight / 2;
            output.position.x -= grasswidth / 2 + sin(_timer * 3.141592f) * _wind / 2;
            output.position.z += grasswidth / 2 + sin(_timer * 3.141592f) * _wind / 2;
    output.position = mul(output.position, _lightView);
    output.position = mul(output.position, _lightProjection);
    output.position = mul(output.position, cropMatrix[0]);

            triStream.Append(output);
    
    
            output = base;
         //   output.position.xz += float2(rndTable[rndX], rndTable[rndZ]);
            output.position.y += grassheight / 2;
            output.position.x += grasswidth / 2 - sin(_timer * 3.141592f) * _wind / 2;
            output.position.z -= grasswidth / 2 - sin(_timer * 3.141592f) * _wind / 2;
    output.position = mul(output.position, _lightView);
    output.position = mul(output.position, _lightProjection);
    output.position = mul(output.position, cropMatrix[0]);

            triStream.Append(output);
    
            triStream.RestartStrip();
    
            triStream.Append(output);
    
            output = base;
      //      output.position.xz += float2(rndTable[rndX], rndTable[rndZ]);
            output.position.y += grassheight / 2;
            output.position.x -= grasswidth / 2 + sin(_timer * 3.141592f) * _wind / 2;
            output.position.z += grasswidth / 2 + sin(_timer * 3.141592f) * _wind / 2;
    output.position = mul(output.position, _lightView);
    output.position = mul(output.position, _lightProjection);
    output.position = mul(output.position, cropMatrix[0]);
            triStream.Append(output);
    
            output = base;
      //      output.position.xz += float2(rndTable[rndX], rndTable[rndZ]);
            output.position.y += grassheight;
            output.position.x -= sin(_timer * 3.141592f) * _wind;
            output.position.z += sin(_timer * 3.141592f) * _wind;
    output.position = mul(output.position, _lightView);
    output.position = mul(output.position, _lightProjection);
    output.position = mul(output.position, cropMatrix[0]);
            triStream.Append(output);


    
    
            triStream.RestartStrip();
     //   }
   // }

}