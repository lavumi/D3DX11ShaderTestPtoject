cbuffer MatrixBuffer : register(b10)
{
    matrix _world;


};
cbuffer MatrixBuffer : register(b11)
{
    matrix _view;
    matrix _projection;

};





cbuffer LightBuffer : register(b1)
{
    float3 _lightDir;
    float _padding;
};

struct VertexInput
{
    float4 position : POSITION0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 wPosition : TEXCOORD0;
    float3 lightPos : TEXCOORD1;
    float3 vertexPos : TEXCOORD2;

};

PixelInput VS(VertexInput input)
{
    PixelInput output;

    input.position.w = 1.0f;

    output.vertexPos = input.position.xyz;
    output.lightPos =- _lightDir;


    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.wPosition = input.position;

    return output;
}








float4 PS(PixelInput input) : SV_TARGET0
{
    float height = saturate(input.wPosition.y);
    float3 lightPos = normalize(input.lightPos);
    float3 vertexPos = normalize(input.vertexPos);

    float lightVertexCos = dot(lightPos, vertexPos);
    float vertexGroundCos = dot(vertexPos, float3(0, 1, 0));
    float lightGroundCos = dot(lightPos, float3(0, 1, 0));

    float3 lightRGB;

    //����Ǵ� ���� ��
    float scatteredLight = abs(vertexGroundCos);

    //r ���� ����� ���� ���� �ʰ� �� ���� ������ ���´�.
    lightRGB.r = pow(lightVertexCos, 5); // * lightVertexCos * lightVertexCos * lightVertexCos * lightVertexCos;
    //�Ķ� ���� ��ǻ� ���� ����� ����
    //���� �Ķ� ���� ����ũ�� ����. ������ ����� �ҵ�
    //����Ǵ� ���� ������Ű�� ���� ������ ���� �Ķ����� �߰���Ŵ
    lightRGB.b = scatteredLight + lightGroundCos;
    lightRGB.g = (lightRGB.r + lightRGB.b) / 2;

       
      
    lightRGB = saturate(lightRGB);

    float4 result = float4(lightRGB, 1); //lerp(_center, _apex, height);
      
    
    return result;
}