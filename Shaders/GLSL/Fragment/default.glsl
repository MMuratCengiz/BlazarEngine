#version 450
#extension GL_ARB_separate_shader_objects : enable
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

layout(set = 1, binding = 0) uniform sampler2D Texture1;
layout(set = 2, binding = 0) uniform Material {
    vec4 diffuseColor;
    vec4 specularColor;
    vec4 textureScale;

    float shininess;
} mat;

layout(set = 3, binding = 0) uniform EnvironmentLights {
    int ambientLightCount;
    int directionalLightCount;
    int pointLightCount;
    int spotLightCount;

    AmbientLight ambientLights[ ALLOWED_LIGHTS ];
    DirectionalLight directionalLights[ ALLOWED_LIGHTS ];
    PointLight pointLights[ ALLOWED_LIGHTS ];
    SpotLight spotLights[ ALLOWED_LIGHTS ];
} environment;

layout (location = 0) in vec2 transitTexture1Coor;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec3 worldPos;

layout (location = 0) out vec4 outputColor;

vec2 texture1Coor;
vec3 viewDirection;
vec4 texturedDiffuse;
vec4 texturedSpecular;

PointLight pointLightFromPower(float power);
float calculateSpecularPower(vec3 direction);
vec4 calculateDirectional(DirectionalLight light);
vec4 calculatePointLight(PointLight light);
vec4 calculateSpotLight(SpotLight light);

void main() {
    texture1Coor = transitTexture1Coor * mat.textureScale.xz;

    texturedSpecular = vec4( 0.5f );
    texturedDiffuse = texture(Texture1, texture1Coor);
    outputColor = vec4( 0 );

    for ( int i = 0; i < environment.ambientLightCount; ++i ) {
        outputColor += normalize( texturedDiffuse + environment.ambientLights[ i ].diffuse ) *  environment.ambientLights[ i ].power;
    }

    for ( int i = 0; i < environment.directionalLightCount; ++i ) {
        outputColor += calculateDirectional( environment.directionalLights[ i ] );
    }

    for ( int i = 0; i < environment.pointLightCount; ++i ) {
        outputColor += calculatePointLight( environment.pointLights[ i ] );
    }

    for ( int i = 0; i < environment.spotLightCount; ++i ) {
        outputColor += calculateSpotLight( environment.spotLights[ i ] );
    }
}

vec4 calculateDirectional(DirectionalLight light) {
    vec3 surfaceToLight = normalize(-light.direction.xyz);

    float diffPower = max(dot(norm, surfaceToLight), 0.0f);
    vec4 diffuse = light.diffuse * (diffPower * texturedDiffuse);

    float spec = calculateSpecularPower(-light.direction.xyz);
    vec4 specular = light.specular * (spec * texturedSpecular);

    return diffuse + specular;
}

vec4 calculatePointLight(PointLight light) {
    vec3 surfaceToLight = normalize(light.position.xyz - worldPos);

    float distance = length(light.position.xyz - worldPos);

    float k = light.attenuationConstant;
    float l = light.attenuationLinear * distance;
    float q = light.attenuationQuadratic * distance * distance;

    float attenuation = 1.0 / (k + l + q);

    float diffPower = max(dot(norm, surfaceToLight), 0.0f);

    vec4 specularResult = light.specular * texturedSpecular * attenuation * calculateSpecularPower(worldPos - light.position.xyz);
    vec4 diffuseResult = light.diffuse * texturedDiffuse * attenuation * diffPower;

    return diffuseResult + specularResult;
}

vec4 calculateSpotLight(SpotLight light) {
    float theta = dot(normalize(light.position.xyz - worldPos), normalize(-light.direction.xyz));

    if (theta > light.radius) {
        PointLight pointLight = pointLightFromPower(light.power);
        pointLight.diffuse = light.diffuse;
        pointLight.specular = light.specular;
        pointLight.position = light.position;

        return calculatePointLight(pointLight);
    }

    return vec4(0);
}

float calculateSpecularPower(vec3 direction) {
    vec3 normalizedDirection = normalize(direction);
    vec3 reflectionDirection = reflect(-normalizedDirection, norm);

    return pow(max(dot(viewDirection, reflectionDirection), 0.0), 256);
}

PointLight pointLightFromPower(float power) {
    float attenuationLinear;
    float attenuationQuadratic;

    if (power >= 7) {
        attenuationLinear = 0.7;
        attenuationQuadratic = 1.8;
    }

    if (power >= 13) {
        attenuationLinear = 0.35;
        attenuationQuadratic = 0.44;
    }

    if (power >= 20) {
        attenuationLinear = 0.22;
        attenuationQuadratic = 0.20;
    }

    if (power >= 32) {
        attenuationLinear = 0.14;
        attenuationQuadratic = 0.07;
    }

    if (power >= 50) {
        attenuationLinear = 0.09;
        attenuationQuadratic = 0.032;
    }

    if (power >= 65) {
        attenuationLinear = 0.07;
        attenuationQuadratic = 0.017;
    }

    if (power >= 100) {
        attenuationLinear = 0.045;
        attenuationQuadratic = 0.0075;
    }

    if (power >= 160) {
        attenuationLinear = 0.027;
        attenuationQuadratic = 0.0028;
    }

    if (power >= 200) {
        attenuationLinear = 0.022;
        attenuationQuadratic = 0.0019;
    }

    if (power >= 325) {
        attenuationLinear = 0.014;
        attenuationQuadratic = 0.0007;
    }

    if (power >= 600) {
        attenuationLinear = 0.007;
        attenuationQuadratic = 0.0002;
    }

    if (power >= 3250) {
        attenuationLinear = 0.0014;
        attenuationQuadratic = 0.000007;
    }

    PointLight pointLight;
    pointLight.attenuationConstant = 1.0f;
    pointLight.attenuationLinear = attenuationLinear;
    pointLight.attenuationQuadratic = attenuationQuadratic;
    return pointLight;
}