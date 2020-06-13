#version 460

out vec4 FragColor;

in VS_OUT
{
    vec4 position;
    vec2 texCoord;
} fs_in;

layout(std140, binding = 0) uniform Matrices
{
    mat4 WVP;
    mat4 shadowWVP;
    vec4 cameraPos;
};

uniform sampler2D positions; //0
uniform sampler2D normals; //1
uniform sampler2D colors; //2
uniform sampler2D depth; //3
uniform sampler2D noise; //4
uniform sampler2D shadowDepth; //5

float SampleShadowMap(sampler2D shadowMap, vec2 coords, float compare) {
	return step(compare, texture2D(shadowMap, coords.xy).r);
}

float SampleShadowMapLinear(sampler2D sampler, vec2 texcoords, float compare) {
    vec2 texelSize = vec2(1.0f) / textureSize(sampler, 0).xy;
    vec2 coords = texcoords / texelSize + vec2(0.5f);

    vec2 fracPart = fract(coords);
	vec2 startTexel = (coords - fracPart) * texelSize;

    float blTexel = SampleShadowMap(sampler, startTexel, compare);
	float brTexel = SampleShadowMap(sampler, startTexel + vec2(texelSize.x, 0.0), compare);
	float tlTexel = SampleShadowMap(sampler, startTexel + vec2(0.0, texelSize.y), compare);
	float trTexel = SampleShadowMap(sampler, startTexel + texelSize, compare);
	
	float mixA = mix(blTexel, tlTexel, fracPart.y);
	float mixB = mix(brTexel, trTexel, fracPart.y);
	
	return mix(mixA, mixB, fracPart.x);
}

float SampleShadowMapPCF(sampler2D sampler, vec2 texcoords, float compare) {
    const vec2 texelSize = vec2(1.0f) / textureSize(sampler, 0).xy;

    const float numSamples = 6.0f;
    const float start = -(numSamples - 1.0f) / 2.0f;
    const float end = (numSamples - 1.0f) / 2.0f;
    const float totalSamples = numSamples * numSamples;

    float accum = 0.0f;
    for (float y = start; y < end; y++) {
        for (float x = start; x < end; x++) {
            accum += SampleShadowMapLinear(sampler, texcoords + vec2(x, y) * texelSize, compare);
        }
    }
    return accum / totalSamples;
}

void main() {
    vec3 position = texture(positions, fs_in.texCoord).rgb;
    vec3 normal = texture(normals, fs_in.texCoord).rgb;
    vec4 noiseValue = texture(noise, fs_in.texCoord);
    
    normal = normalize(normal);

    vec4 mainColor = texture(colors, fs_in.texCoord);
    vec4 color = mainColor;
    color.rgb -= vec3(0.3f);

    // Point light
    const vec3 lightPosition = vec3(1.0f, 5.0f, 1.0f);
    const vec4 lightColor = vec4(0.7f, 0.7f, 0.7f, 1.0f);

    vec3 lightDir = normalize(lightPosition - position);
    float dist = length(lightPosition - position);

    if (dist < 13.0f) {
        float intensity  = max(dot(normalize(normal), lightDir), 0.0);
        vec3 spotLightDirection = vec3(0.0f, 1.0f, 0.0f);

        if (dot(spotLightDirection, lightDir) > 0.77f) {
            float attenuation = 1.0 / (1.0 + 0.014 * dist + 0.03 * (dist * dist));
            //color = color * lightColor * attenuation * intensity;
        }
    }

    // Directional light
    vec3 directonalLightDirection = vec3(0.0f, 1.0f, 0.0f);
    vec4 directionalLightColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
    float lightFactor = dot(normal, normalize(directonalLightDirection));
    if (lightFactor > 0.0f) {
        color += mainColor * lightFactor * directionalLightColor;
    }


    // Shadow stuff
	vec4 shadowPos = vec4(position, 1.0f) * shadowWVP;
	vec3 projCoords = (shadowPos.xyz / shadowPos.w) / 2.0f + 0.5f;
	float pixelDepthInLightPos = projCoords.z;

	//if (clamp(projCoords.x, 0.0f, 1.0f) == projCoords.x && projCoords.y == clamp(projCoords.y, 0.0f, 1.0f) && projCoords.z < 1.0) {
        float bias = 0.0f;//max(0.05 * (1.0 - dot(normal, cameraPos.xyz - position.xyz)), 0.00005);
        float shadow = step(projCoords.z - bias, texture(shadowDepth, projCoords.xy).r);
        //float shadow = textureProj(shadowDepth, (shadowPos / shadowPos.w) / 2.0f + 0.5f);
        color -= 1.0f- shadow;
	//}

    FragColor = color;
}

