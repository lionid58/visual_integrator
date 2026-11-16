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

// --- СТРУКТУРЫ ИСТОЧНИКОВ СВЕТА ---
struct DirLight {
    vec3 direction;
    vec3 color;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;
    vec3 color;
};
uniform PointLight pointLight;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 color;
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


// --- ГЛАВНАЯ ФУНКЦИЯ ---
void main()
{
    vec3 norm = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
    vec3 viewDir = normalize(viewPos - FragPos);
    norm = faceforward(norm, -viewDir, norm);

    // Цветные аккумуляторы света
    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    // 1. СОЛНЦЕ (Directional Light)
    if (useSun) {
        vec3 lightDir = normalize(dirLight.direction);
        totalDiffuse += CalcDiffuse(lightDir, norm, dirLight.color);
        totalSpecular += CalcSpecular(lightDir, norm, viewDir, dirLight.color);
    }

    // 2. ЛАМПОЧКА (Point Light)
    if (useLamp) {
        vec3 lightDir = normalize(pointLight.position - FragPos);
        float dist = length(pointLight.position - FragPos);
        float attenuation = CalcAttenuation(dist);

        totalDiffuse += CalcDiffuse(lightDir, norm, pointLight.color) * attenuation;
        totalSpecular += CalcSpecular(lightDir, norm, viewDir, pointLight.color) * attenuation;
    }

    // 3. ПРОЖЕКТОР (Spotlight - светит из камеры по направлению взгляда)
    if (useSpotlight) {
        vec3 lightDir = normalize(spotLight.position - FragPos);
        float dist = length(spotLight.position - FragPos);
        float attenuation = CalcAttenuation(dist);

        float theta = dot(lightDir, normalize(-spotLight.direction));
        float epsilon = spotLight.cutOff - spotLight.outerCutOff;
        float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

        totalDiffuse += CalcDiffuse(lightDir, norm, spotLight.color) * attenuation * intensity;
        totalSpecular += CalcSpecular(lightDir, norm, viewDir, spotLight.color) * attenuation * intensity;
    }

    // Итог: (Фон + цветной рассеянный свет) умножаются на цвет геометрии + цветные блики
    vec3 result = (vec3(AmbientLight) + totalDiffuse) * Color + totalSpecular;
    FragColor = vec4(result, 1.0f);
}