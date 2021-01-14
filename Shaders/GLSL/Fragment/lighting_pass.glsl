#version 450

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
layout(set = 2, binding = 0) uniform sampler2D gBuffer_AlbedoSpec;
layout(set = 3, binding = 0) uniform sampler2D gBuffer_Scene;
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

layout (location = 0) in vec3 transitPosition;

layout (location = 0) out vec4 outputColor;

vec4 calculateDirectional(DirectionalLight light);
float calculateSpecularPower(vec3 direction);

vec3 viewDirection;
vec3 position;
vec3 normal;
vec4 albedo;
float spec;

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
    vec4 fragPos = texture(gBuffer_Scene, transitPosition.xy);

    if ( fragPos.w == 0 )
    {
        outputColor = vec4( 0 );
        return;
    }

    vec4 albedoSpec = texture(gBuffer_AlbedoSpec, transitPosition.xy).rgba;

    position = texture(gBuffer_Position, transitPosition.xy).rgb;
    normal = texture(gBuffer_Normal, transitPosition.xy).rgb;
    albedo = texture(gBuffer_AlbedoSpec, transitPosition.xy).rgba;

    albedo.a = 1.0f;
    spec = albedoSpec.a;

    outputColor = vec4(0.0f);

    vec4 posInLightSpace = depthNormalizeTransform * lvpm.casters[0] * vec4(vec3(fragPos), 1.0f);

    if (environment.directionalLightCount > 0)
    {
        bias = max(0.05 * (1.0 - dot(normal, -environment.directionalLights[0].direction.xyz)), bias);
    }

    float shadow = shadowCalculationPCF(posInLightSpace);

    //    outputColor = vec4( shadow );

    for (int i = 0; i < environment.ambientLightCount; ++i) {
        outputColor += normalize(albedo + environment.ambientLights[i].diffuse) *  environment.ambientLights[i].power;
    }

    for (int i = 0; i < environment.directionalLightCount; ++i) {
        outputColor += calculateDirectional(environment.directionalLights[i]) * (shadow);
    }
}

vec4 calculateDirectional(DirectionalLight light) {
    vec3 surfaceToLight = normalize(-light.direction.xyz);

    float diffPower = max(dot(normal, surfaceToLight), 0.0f);
    vec4 diffuse = light.diffuse * (diffPower * albedo);

    float spec = calculateSpecularPower(-light.direction.xyz);
    vec4 specular = light.specular * (spec * albedo);

    return diffuse + specular;
}

float calculateSpecularPower(vec3 direction) {
    vec3 normalizedDirection = normalize(direction);
    vec3 reflectionDirection = reflect(-normalizedDirection, normal);

    return pow(max(dot(viewDirection, reflectionDirection), 0.0), 256);
}