#include "ShaderPreset.hlsli"





struct VertexInput
{
    float4 position : POSITION0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 wPosition : TEXCOORD0;
    float3 vertexPos : TEXCOORD2;

};

PixelInput VS(VertexInput input)
{
    PixelInput output;

    input.position.w = 1.0f;

    output.vertexPos = input.position.xyz;


    output.position = mul(input.position, _world);
    output.position = mul(output.position, _viewXprojection);
   // output.position = mul(output.position, _projection);

    output.wPosition = input.position;

    return output;
}



//float2 getUVbyCircle(float3 pixelVector, float3 uVector, float3 vVector)
//{
//    float3 uvPlaneNormal = cross(uVector, vVector);
//    uvPlaneNormal = normalize(uvPlaneNormal);

//    float3 projectedVector = pixelVector - dot(pixelVector, uvPlaneNormal) * uvPlaneNormal;
    
//   // float length = length(projectedVector);
//    float u = dot(projectedVector, uVector);
//    float v = dot(projectedVector, vVector);

//    if (u > 1)
//        u = 0;
//    if (v > 1)
//        v = 0;


//    return float2(u, v);
//}




Texture2D map;

float4 PS(PixelInput input) : SV_TARGET0
{
    float height = saturate(input.wPosition.y);
    float3 lightPos = _lightDir;
    float3 vertexPos = normalize(input.vertexPos);

    float lightVertexCos = dot(lightPos, vertexPos);
    float vertexGroundCos = dot(vertexPos, float3(0, 1, 0));
    float lightGroundCos = dot(float3(0, 1, 0), lightPos);

    float3 lightRGB;

    //����Ǵ� ���� ��
    float scatteredLight = 0.2f;// abs(vertexGroundCos) + 0.2f;

    lightVertexCos = saturate(lightVertexCos);

    //r ���� ����� ���� ���� �ʰ� �� ���� ������ ���´�.
    lightRGB.r = pow(lightVertexCos, 15) ; 

    //�Ķ� ���� ��ǻ� ���� ����� ����
     //����Ǵ� ���� ������Ű�� ���� ������ ���� �Ķ����� �߰���Ŵ
    //TODO : ���� �Ķ� ���� ����ũ�� ����. �� ���� ����� ������?
    lightRGB.b = scatteredLight + lightGroundCos;

    lightRGB.g = (lightRGB.r + lightRGB.b) /2;
    
      
    lightRGB = saturate(lightRGB);


    float4 result = float4(lightRGB, 1); //lerp(_center, _apex, height);
      


    //�� �׸���
    lightVertexCos = dot(-lightPos, vertexPos);

    float moonColor = rndTable[vertexGroundCos*100] + 0.4f;
    result = lerp(0.4f, result, step(lightVertexCos, 0.999f));

 //
 //   float3 moonPos2Vertex = vertexPos - lightPos;
 //
 // //  moonPos2Vertex =  normalize(moonPos2Vertex);
 //   float2 uv = getUVbyCircle(moonPos2Vertex * 1000, float3(0, 0, 1), float3(0, 1, 0));
 //
 //  //  lightVertexCos = dot(lightPos, vertexPos);
 //   result = lerp(0.5f, float4(uv, 0, 1), step(lightVertexCos, 0.999f));

    
    return result;
}