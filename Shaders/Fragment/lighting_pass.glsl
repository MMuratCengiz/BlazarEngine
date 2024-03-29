#version 450

#include "frag_utilities.glsl"

#define MAX_ALLOWED_SHADOW_CASTERS 3
#define ALLOWED_LIGHTS 16

struct AmbientLight {
    float power;
    vec4 diffuse;
    vec4 specular;
};

struct DirectionalLight {
    float power;
    vec4 diffuse;
    vec4 specular;
    vec4 direction;
};

struct PointLight {
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;

    vec4 position;
    vec4 diffuse;
    vec4 specular;
};

struct SpotLight {
    float power;
    float radius;
    vec4 position;
    vec4 direction;
    vec4 diffuse;
    vec4 specular;
};

layout(push_constant) uniform PushConstants {
    mat4 ModelMatrix;
} pushConstants;

layout(set = 0, binding = 0) uniform sampler2D gBuffer_Position;
layout(set = 1, binding = 0) uniform sampler2D gBuffer_Normal;
layout(set = 2, binding = 0) uniform sampler2D gBuffer_Albedo;
layout(set = 3, binding = 0) uniform sampler2D gBuffer_Material;
layout(set = 4, binding = 0) uniform sampler2D shadowMap;

layout(set = 5, binding = 0) uniform EnvironmentLights {
    int ambientLightCount;
    int directionalLightCount;
    int pointLightCount;
    int spotLightCount;

    AmbientLight ambientLights[ALLOWED_LIGHTS];
    DirectionalLight directionalLights[ALLOWED_LIGHTS];
    PointLight pointLights[ALLOWED_LIGHTS];
    SpotLight spotLights[ALLOWED_LIGHTS];
} environment;

layout(set = 6, binding = 0) uniform LightViewProjectionMatrix {
    mat4[MAX_ALLOWED_SHADOW_CASTERS] casters;
    int arraySize;
} lvpm;

layout(set = 7, binding = 0) uniform WorldContext
{
    vec4 cameraPosition;
} worldContext;

layout (location = 0) in vec4 inPosition;

layout (location = 0) out vec4 outputColor;

vec4 calculateDirectional(DirectionalLight light);
float calculateSpecularPower(vec3 direction);
int getShininess( );

vec3 viewDirection;
vec3 position;
vec3 normal;
vec4 albedo;
float shininess;

const mat4 depthNormalizeTransform = mat4(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.5, 0.5, 0.0, 1.0);

float bias = 0.0f;

float shadowCalculation(vec4 fragPosLightSpace)
{
    float shadow = 1.0f;

    vec4 shadow_coords = fragPosLightSpace / fragPosLightSpace.w;

    if (texture(shadowMap, shadow_coords.xy).r < shadow_coords.z - bias) {
        shadow = 0.0;
    }

    return shadow;
}

float shadowCalculationPCF(vec4 fragPosLightSpace)
{
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    vec4 shadow_coords = fragPosLightSpace / fragPosLightSpace.w;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, shadow_coords.xy + vec2(x, y) * texelSize).r;
            shadow += shadow_coords.z - bias > pcfDepth ? 0.75 : 0.0;
        }
    }

    shadow /= 9.0;

    return 1.0 - shadow;
}

void main() {
    vec4 fragPos = texture(gBuffer_Position, inPosition.xy);

    outputColor = vec4( 0 );

    if (fragPos.w == 0)
    {
        discard;
    }

    normal = texture(gBuffer_Normal, inPosition.xy).rgb;
    albedo = texture(gBuffer_Albedo, inPosition.xy);
    shininess = texture(gBuffer_Material, inPosition.xy).r;

    viewDirection = normalize( worldContext.cameraPosition.xyz - fragPos.xyz );

    vec4 posInLightSpace = depthNormalizeTransform * lvpm.casters[0] * vec4(vec3(fragPos), 1.0f);

    if (environment.directionalLightCount > 0)
    {
        bias = max(0.05 * (1.0 - dot(normal, -environment.directionalLights[0].direction.xyz)), bias);
    }

    float shadow = shadowCalculationPCF(posInLightSpace);

    for (int i = 0; i < environment.ambientLightCount; ++i) {
        outputColor += normalize(albedo + environment.ambientLights[i].diffuse) *  environment.ambientLights[i].power;
    }

    for (int i = 0; i < environment.directionalLightCount; ++i) {
        outputColor += calculateDirectional(environment.directionalLights[i]) * (shadow);
    }

    outputColor = gammaCorrectColor(outputColor);
}

vec4 calculateDirectional(DirectionalLight light) {
    vec3 surfaceToLight = normalize(-light.direction.xyz);

    float diffPower = max(dot(normal, surfaceToLight), 0.0f);
    vec4 diffuse = light.diffuse * (diffPower * albedo);

    float spec = calculateSpecularPower(surfaceToLight);
    vec4 specular = light.diffuse * (spec * albedo);

    return diffuse * light.power + specular;
}

float calculateSpecularPower(vec3 direction) {
    vec3 halfwayVector = normalize( direction + viewDirection );
    return pow(max(dot(normal, halfwayVector), 0.0), getShininess( ) );
}

int getShininess( )
{
    if ( shininess >= 0.8 )
    {
        return 2;
    }

    if ( shininess >= 0.65 )
    {
        return 4;
    }

    if ( shininess >= 0.5 )
    {
        return 8;
    }

    if ( shininess >= 0.35 )
    {
        return 16;
    }

    if ( shininess >= 0.20 )
    {
        return 32;
    }

    if ( shininess >= 0.10 )
    {
        return 64;
    }

    return 128;
}