#include "ShaderPreset.hlsli"


struct VertexInput
{
    float4 position : POSITION0;
    float2 world0 : INSTMATRIX0;
};
struct GeoInput
{
    float4 position : SV_POSITION;
    float clip : SV_ClipDistance0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 lightWorldPosition : TEXCOORD0;
};

GeoInput VS(VertexInput input)
{
    GeoInput output;
    input.position.w = 1;

    output.position = input.position;
    output.position.xz += input.world0.xy;
    output.clip = 0;
    return output;
}


Texture2D _map : register(t0);
SamplerState samp[3];

cbuffer GsData : register(b0)
{
    
    float _grassHeight;
    float _grassWidth;
    float _timer;
    float4 _characterPos;
    float2 _wind;
}

[maxvertexcount(6)]
void GS(point GeoInput input[1], inout TriangleStream<PixelInput> triStream)
{
  

    float grassheight = _grassHeight;
    float grasswidth = _grassWidth;
    
    PixelInput base;
    base.position = input[0].position;

    int rndx, rndz;
    rndx = base.position.x * 399;
    rndz = base.position.z * 928;
    int rnd = rndx + rndz;
    rnd %= 100;


    float2 heightuv = base.position.xz / ImageSize;
    heightuv.y *= -1;
    heightuv.y += 1;
    base.position.y = _map.SampleLevel(samp[0], heightuv, 0).r * 64.0f - 26.2f;


    //풀을 표현할 벡터
    float3 heightVector, widthVector;
    heightVector.x = (float) (rndx % 3) - 1;
    heightVector.y = (float) (rndx % 7) + 3;
    heightVector.z = (float) (rndz % 3) - 1;
    heightVector = normalize(heightVector);
   
  //  float2 wind = normalize(_wind);
  //  heightVector.xz += _wind * (sin(_timer * rndx * 0.00001f * 3.141592f) + 1) * 0.1f;
    heightVector = normalize(heightVector);

    widthVector.z = 1;
    widthVector.y = (float) (rndx % 5);
    widthVector.x = (-heightVector.z - widthVector.y * heightVector.y) / heightVector.x;
    widthVector = normalize(widthVector);





    heightVector *= _grassHeight / 2;
    widthVector *= _grassWidth / 2;






    PixelInput root, left, right, top;

    root = left = right = top = base;

    float3 leftVector, rightVector, topVector;

    leftVector = heightVector - widthVector;
    rightVector = heightVector + widthVector;
    topVector = heightVector * 2;



     //캐릭터 위치에 따른 기울어짐
    float3 characterToRoot = root.position.xyz - _characterPos.xyz;
    float3 rotationAxisVector = cross(characterToRoot, float3(0, 1, 0));
    rotationAxisVector = normalize(rotationAxisVector);
    float rotationDegree = (1 - saturate((characterToRoot.x * characterToRoot.x + characterToRoot.z * characterToRoot.z) * 0.2f)) * 1.570796f * 0.4f;
    
    float3x3 rotationMatrix = RotationMatrix(rotationAxisVector, rotationDegree);
    float3x3 rotationMatrixHalf = RotationMatrix(rotationAxisVector, rotationDegree * 0.8f);

    leftVector = mul(leftVector, rotationMatrix);
    rightVector = mul(rightVector, rotationMatrix);
    topVector = mul(topVector, rotationMatrixHalf);






    left.position.xyz += leftVector;
    right.position.xyz += rightVector;
    top.position.xyz += topVector;
    




    float4 lightWorldPosition;

    lightWorldPosition = MulLightVP(root.position);
    root.lightWorldPosition = mul(lightWorldPosition, cropMatrix[0]);


    lightWorldPosition = MulLightVP(left.position);
    left.lightWorldPosition = mul(lightWorldPosition, cropMatrix[0]);


    lightWorldPosition = MulLightVP(right.position);
    right.lightWorldPosition = mul(lightWorldPosition, cropMatrix[0]);


    lightWorldPosition = MulLightVP(top.position);
    top.lightWorldPosition = mul(lightWorldPosition, cropMatrix[0]);
    

    root.position = MulVP(root.position);

    left.position = MulVP(left.position);

    right.position = MulVP(right.position);

    top.position = MulVP(top.position);




    triStream.Append(root);
    triStream.Append(left);
    triStream.Append(right);
    
    triStream.RestartStrip();
    

    triStream.Append(right);
    triStream.Append(left);
    triStream.Append(top);
    
    triStream.RestartStrip();
    
}

Texture2DArray _lightMap : register(t13);
SamplerComparisonState _samp[3];

float4 PS(PixelInput input) : SV_Target
{
    float shadow = 1;
    float3 projectTexCoord;
    float lightDepthValue;

    float4 lWorldPos = input.lightWorldPosition;
          
    projectTexCoord.x = lWorldPos.x / lWorldPos.w / 2.0f + 0.5f;
    projectTexCoord.y = -lWorldPos.y / lWorldPos.w / 2.0f + 0.5f;
    projectTexCoord.z = 0;
    lightDepthValue = lWorldPos.z / lWorldPos.w;

    float bias = 0.000001f;
 
    //라이트 텍스쳐가 존재하는 부분만 그림자 처리 연산
    if ((saturate(projectTexCoord.x) == projectTexCoord.x)
        && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
       
              //marginBias = acos(saturate(dot(normal, lightDir)));
              //epsilon = 0.001 / marginBias;
              //epsilon = clamp(epsilon, 0, 0.1f);
        shadow = _lightMap.SampleCmpLevelZero(_samp[2], projectTexCoord, lightDepthValue + bias) + 0.3f;
        shadow = saturate(shadow);
 
    }
  
    return shadow;

}