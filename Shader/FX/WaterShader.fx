cbuffer MatrixBuffer : register(b0)
{
    matrix _world;
    matrix _view;
    matrix _projection;

};

cbuffer Camera : register(b1)
{
    float3 _cameraPosition;
    float _paddd;
}


cbuffer LightBuffer : register(b2)
{
    matrix _lightView;
    matrix _lightProjection;
    float3 _lightDir;
    float _baseLight;
};

cbuffer ExtraBuffer : register(b3)
{
    matrix worldInverseTransposeMatrix;
    float shadowBias;

}



struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : NORMAL1;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 halfVector : TEXCOORD1;
    float3 lightDir : TEXCOORD2;
    float3 normal : NORMAL0;
    float3 viewDir : TEXCOORD3;

    float options : TEXCOORD4;
    float4 viewPosition : TEXCOORD5;
  

};




PixelInput VS(VertexInput input)
{
    PixelInput output;

    
    input.position.w = 1.0f;

    output.position = mul(input.position, _world);


    float3 viewDir = _cameraPosition - output.position.xyz;

    float3 halfVector = normalize(normalize(-_lightDir) + normalize(viewDir));




    float3 n = mul(input.normal, (float3x3) worldInverseTransposeMatrix);
    float3 t = mul(input.tangent, (float3x3) worldInverseTransposeMatrix);
    float3 b = cross(n, t);
    float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);

    output.halfVector = mul(halfVector, tbnMatrix);
    output.lightDir = normalize(mul(-_lightDir, tbnMatrix));
    output.viewDir = mul(viewDir, tbnMatrix);

   
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
   
    output.viewPosition = output.position;

    output.uv = input.uv;
    output.normal = normalize(mul(input.normal, tbnMatrix));


    output.options = shadowBias;
   

    return output;

}





Texture2D _map : register(t0);
Texture2D _normalMap : register(t1);
Texture2D _perlin : register(t2);

SamplerState samp[3];


cbuffer Material : register(b0)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 globalAmbient;
    float shininess;
    
};

cbuffer water : register(b1)
{
    float _translation;
    float _scale;
    float _angle;
    float padding;
}



float4 PS(PixelInput input) : SV_TARGET
{
    float4 perturbValue;


    //물 노말맵 크기 설정
    float2 temp_uv = input.uv * _scale;
  
   //흐르는 셋팅
   temp_uv.y += _translation * cos(_angle);
   temp_uv.x -= _translation * sin(_angle);
  
  
     //노말맵 각도 회전
   // temp_uv = frac(temp_uv);
    
    temp_uv -= _scale/2;
    float2 uv;
    
    uv.x = temp_uv.x * cos(_angle) + temp_uv.y * sin(_angle);
    uv.y = temp_uv.y * cos(_angle) - temp_uv.x * sin(_angle);
    
    uv += _scale / 2;
    
   // perturbValue = _perlin.Sample(samp[0], uv);
   // perturbValue *= _scale;
   // uv += perturbValue.xy + _translation;



    float4 diffuseMap = float4(0.2f, 0.5f, 1, 0.5f);
   // _map.Sample(samp[0], input.uv);

    float3 halfVector = normalize(input.halfVector);

    float3 normal = _normalMap.Sample(samp[0], uv).rgb;

    normal = (normal * 2.0f) - 1.0f;
 
    normal = normalize(normal);

    float3 light = normalize(input.lightDir);

    float nDotL = saturate(dot(normal, light));
    float nDotH = saturate(dot(normal, halfVector));

    float power = pow(nDotH, shininess);

    float4 intensity = ambient * globalAmbient + diffuse * nDotL + specular * power;

    float4 result = intensity * diffuseMap;
    result.a = 0.2f;
    return     result;
}
















/*

float4 PS2(PixelInput input) : SV_TARGET
{


    float4 diffuseMap = lerp(_map.Sample(samp[0], input.uv), float4(0.4, 0.2, 1, 1), step(drawTexture, 0.5));




    float4 bumpMap = _normalMap.Sample(samp[0], input.uv);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
 
  // float3 bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.bitangent;
 
  // bumpNormal = normalize(bumpNormal);
 
 
    float3 light = normalize(input.lightDir);
  
    float lightIntensity = saturate(dot(bumpMap.rgb, light)) * 2;
 
 
 
    float4 color = saturate(diffuseMap * lightIntensity);
 





    float2 projectTexCoord;
    projectTexCoord.x = input.lightWorldPosition.x / input.lightWorldPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightWorldPosition.y / input.lightWorldPosition.w / 2.0f + 0.5f;

    float depthValue;
    float lightDepthValue;
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = _lightMap.Sample(samp[0], projectTexCoord).r;

        lightDepthValue = input.lightWorldPosition.z / input.lightWorldPosition.w;

        lightDepthValue = lightDepthValue - 0.0001;

        if (lightDepthValue > depthValue)
        {
            color = 0;
		   // lightIntensity = 0.2f;
           //
           // if (lightIntensity > 0.0f)
           // {
			//    color += (color * lightIntensity);
			//    color = saturate(color);
           // }
        }
    }




    return color;


}

float4 BlinPhong(PixelInput input) : SV_TARGET
{
    float4 diffuseMap = lerp(_map.Sample(samp[0], input.uv), float4(0.4, 0.2, 1, 1), step(drawTexture, 0.5));


    float3 halfVector = normalize(input.halfVector);





    float3 normal = _normalMap.Sample(samp[0], input.uv).rgb;

    normal = (normal * 2.0f) - 1.0f;
 
    normal = normalize(normal);

    float3 light = normalize(input.lightDir);

    float nDotL = saturate(dot(normal, light));
    float nDotH = saturate(dot(normal, halfVector));

    float power = pow(nDotH, shininess);

    float4 intensity = ambient * globalAmbient + diffuse * nDotL + specular * power;

    return    intensity * diffuseMap;
}


float4 ParallexMapping(PixelInput input) : SV_TARGET
{
  


    float2 uv;

    float height = _heightMap.Sample(samp[0], input.uv).r;
        

    float3 viewDir = normalize(input.viewDir);
    uv = input.uv - (height * viewDir.xy * scale) / viewDir.z;




    float3 normal = _normalMap.Sample(samp[0], uv).rgb;
    normal = (normal * 2.0f) - 1.0f;
 
    normal = normalize(normal);

    float3 light = normalize(input.lightDir);
    float3 halfVector = normalize(input.halfVector);

    float nDotL = saturate(dot(normal, light));
    float nDotH = saturate(dot(normal, halfVector));

    float power = pow(nDotH, shininess);

    float4 intensity = ambient * globalAmbient + diffuse * nDotL + specular * power;


    float4 diffuseMap = lerp(_map.Sample(samp[0], uv), float4(0.4, 0.2, 1, 1), step(drawTexture, 0.5));
    return intensity * diffuseMap;
}


float4 ParallexShadow(PixelInput input) : SV_TARGET
{
    float2 uv;
  

    float3 viewDir = normalize(input.viewDir);
    float layer_depth = 1.0 / layer;
    float cur_layer_depth = 0.0;
    float2 delta_uv = viewDir.xy * scale / (viewDir.z * layer);
    float2 cur_uv = input.uv;

    float depth_from_tex = _heightMap.Sample(samp[0], cur_uv).r;

    //layerstep;
    for (int layerstep = 0; layerstep < 32; layerstep++)
    {
        cur_layer_depth += layer_depth;
        cur_uv -= delta_uv;
        depth_from_tex = _heightMap.Sample(samp[0], cur_uv).r;
        if (depth_from_tex < cur_layer_depth)
        {
            break;
        }
    }


    uv = cur_uv;
           
    float2 prev_uv = cur_uv + delta_uv;
    float next = depth_from_tex - cur_layer_depth;
    float prev = _heightMap.Sample(samp[0], prev_uv).r - cur_layer_depth + layer_depth;
    float weight = next / (next - prev);
    uv = lerp(cur_uv, prev_uv, weight);
    

    float3 light = normalize(input.lightDir);




    //그림자 만들기mo
    float shadowIntensity = 0;
    float2 shadowDelta_uv = light.xy * scale / (light.z * layer);
    cur_layer_depth -= layer_depth;
    int totalStep = layerstep;
    for (; layerstep > 0; layerstep--)
    {
        cur_layer_depth -= layer_depth;
        cur_uv = shadowDelta_uv * layerstep;
        depth_from_tex = _heightMap.Sample(samp[0], cur_uv).r;
        if (depth_from_tex < cur_layer_depth)
        {
            shadowIntensity += (cur_layer_depth - depth_from_tex) * (1 - (float) layerstep / (float) totalStep);
        }
    }
   
    shadowIntensity = saturate(1 - shadowIntensity);




    float4 diffuseMap = lerp(_map.Sample(samp[0], uv), float4(0.4, 0.2, 1, 1), step(drawTexture, 0.5));
    float3 normal = _normalMap.Sample(samp[0], uv).rgb;
    normal = (normal * 2.0f) - 1.0f;
 
    normal = normalize(normal);

    
    float3 halfVector = normalize(input.halfVector);

    float nDotL = saturate(dot(normal, light));
    float nDotH = saturate(dot(normal, halfVector));

    float power = pow(nDotH, shininess);

    float4 intensity = ambient * globalAmbient + diffuse * shadowIntensity * nDotL + specular * power;

    return intensity * diffuseMap;
}



*/