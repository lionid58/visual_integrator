#version 330 core

in vec3 Color;
in vec3 FragPos;
out vec4 FragColor;

// --- БАЗОВЫЕ ПАРАМЕТРЫ ---
uniform float AmbientLight;
uniform vec3 viewPos;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

// --- ФЛАГИ АКТИВНОСТИ ---
uniform bool useSun;
uniform bool useLamp;
uniform bool useSpotlight;

// --- КАРТЫ ТЕНЕЙ ---
uniform sampler2D shadowMapSun;
uniform sampler2D shadowMapLamp;
uniform sampler2D shadowMapSpot;

// --- СТРУКТУРЫ ИСТОЧНИКОВ СВЕТА ---
struct DirLight {
    vec3 direction;
    vec3 color;
    mat4 lightSpaceMatrix;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;
    vec3 color;
    mat4 lightSpaceMatrix;
};
uniform PointLight pointLight;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 color;
    mat4 lightSpaceMatrix;
};
uniform SpotLight spotLight;


// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ---

vec3 CalcDiffuse(vec3 lightDir, vec3 normal, vec3 lightColor) {
    return diffuseStrength * max(dot(normal, lightDir), 0.0) * lightColor;
}

vec3 CalcSpecular(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 lightColor) {
    vec3 reflectDir = reflect(-lightDir, normal);
    return specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), shininess) * lightColor;
}

float CalcAttenuation(float dist) {
    return 1.0 / (1.0 + 0.09 * dist + 0.032 * (dist * dist));
}

// Возвращает 1.0, если точка в тени, и 0.0, если освещена
float CalcShadow(mat4 lightSpaceMatrix, sampler2D shadowMap, vec3 normal, vec3 lightDir) {
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    // Перспективное деление (нужно для перспективных матриц лампы и прожектора)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Перевод из диапазона [-1, 1] в текстурные координаты [0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    // Если точка за пределами видимости источника, тени нет
    if (projCoords.z > 1.0 || projCoords.z < 0.0)
        return 0.0;

    float currentDepth = projCoords.z;
    // Смещение (bias) зависит от угла падения луча и защищает от полос самозатенения
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    // Мягкие тени (PCF 3x3)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}


// --- ГЛАВНАЯ ФУНКЦИЯ ---
void main()
{
    vec3 norm = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
    vec3 viewDir = normalize(viewPos - FragPos);
    norm = faceforward(norm, -viewDir, norm);

    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    // 1. СОЛНЦЕ
    if (useSun) {
        vec3 lightDir = normalize(dirLight.direction);
        float shadow = CalcShadow(dirLight.lightSpaceMatrix, shadowMapSun, norm, lightDir);

        totalDiffuse += (1.0 - shadow) * CalcDiffuse(lightDir, norm, dirLight.color);
        totalSpecular += (1.0 - shadow) * CalcSpecular(lightDir, norm, viewDir, dirLight.color);
    }

    // 2. ЛАМПОЧКА
    if (useLamp) {
        vec3 lightDir = normalize(pointLight.position - FragPos);
        float dist = length(pointLight.position - FragPos);
        float attenuation = CalcAttenuation(dist);
        float shadow = CalcShadow(pointLight.lightSpaceMatrix, shadowMapLamp, norm, lightDir);

        totalDiffuse += (1.0 - shadow) * CalcDiffuse(lightDir, norm, pointLight.color) * attenuation;
        totalSpecular += (1.0 - shadow) * CalcSpecular(lightDir, norm, viewDir, pointLight.color) * attenuation;
    }

    // 3. ПРОЖЕКТОР
    if (useSpotlight) {
        vec3 lightDir = normalize(spotLight.position - FragPos);
        float dist = length(spotLight.position - FragPos);
        float attenuation = CalcAttenuation(dist);

        float theta = dot(lightDir, normalize(-spotLight.direction));
        float epsilon = spotLight.cutOff - spotLight.outerCutOff;
        float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

        float shadow = CalcShadow(spotLight.lightSpaceMatrix, shadowMapSpot, norm, lightDir);

        totalDiffuse += (1.0 - shadow) * CalcDiffuse(lightDir, norm, spotLight.color) * attenuation * intensity;
        totalSpecular += (1.0 - shadow) * CalcSpecular(lightDir, norm, viewDir, spotLight.color) * attenuation * intensity;
    }

    // Тень гасит только прямой и отраженный свет, фоновый AmbientLight остаётся всегда
    vec3 result = (vec3(AmbientLight) + totalDiffuse) * Color + totalSpecular;
    FragColor = vec4(result, 1.0f);
}