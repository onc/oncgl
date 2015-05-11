#version 330

/**
 * Structs
 */
struct Light {
  vec3 color;
  float ambient_intensity;
  float diffuse_intensity;
};

struct Attenuation {

  float constant;
  float linear;
  float exponent;
};

struct DirectionalLight {

  Light light;
  vec3 direction;
};

struct PointLight {

  Light light;
  vec3 position;
  Attenuation atten;
};

struct SpotLight {

  PointLight light;
  vec3 direction;
  float cutoff;
};

/**
 * Uniforms
 */
uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;

uniform DirectionalLight dirLight;

uniform vec3 screenSize;

uniform vec3 eyePos;

/**
 * OUT
 */
out vec4 FragColor;

vec2 calcTexCoord() {

  return gl_FragCoord.xy / screenSize.xy;
}

vec4 calcLightInternal(Light light, vec3 lightDirection, vec3 worldPos, vec3 normal) {

  vec4 ambientColor = vec4(light.color, 1.0) * light.ambient_intensity;
  float diffuseFactor = dot(normal, -lightDirection);

  vec4 diffuseColor  = vec4(0, 0, 0, 0);
  vec4 specularColor = vec4(0, 0, 0, 0);

  if (diffuseFactor > 0.0) {
    diffuseColor = vec4(light.color, 1.0) * light.diffuse_intensity * diffuseFactor;

    vec3 viewDir = normalize(eyePos - worldPos);
    vec3 halfwayDir = normalize(-lightDirection + viewDir);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 1024);

    if (specularFactor > 0.0) {
      specularColor = vec4(light.color, 1.0) * 0.2 * specularFactor;
    }
  }

  return (ambientColor + diffuseColor + specularColor);
}

vec4 calcDirectionalLight(vec3 worldPos, vec3 normal) {

  vec4 lightDir = vec4(dirLight.direction, 1.0f);

  return calcLightInternal(dirLight.light, lightDir.xyz, worldPos, normal);
}

void main() {

  vec2 TexCoord = calcTexCoord();
  vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
  vec3 Color = texture(gColorMap, TexCoord).xyz;
  vec3 Normal = texture(gNormalMap, TexCoord).xyz;
  Normal = normalize(Normal);

  FragColor = vec4(Color, 1.0) * calcDirectionalLight(WorldPos, Normal);
}
