#ifndef ONCGL_LIGHT_LIGHT_H
#define  ONCGL_LIGHT_LIGHT_H

#include <glm/glm.hpp>

namespace oncgl {

struct Light {

  glm::vec3 color;
  float ambient_intensity;
  float diffuse_intensity;

  Light() {

    color = glm::vec3(0.0f);
    ambient_intensity = 0.0f;
    diffuse_intensity = 0.0f;
  }
};

struct DirectionalLight : public Light {

  glm::vec3 direction;

  DirectionalLight() {

    direction = glm::vec3(0.0f);
  }
};

struct PointLight : public Light {

  glm::vec3 position;

  struct {

    float constant;
    float linear;
    float exp;
  } attenuation;

  PointLight() {

    position = glm::vec3(0.0f);
    attenuation.constant = 0.0f;
    attenuation.linear = 0.0f;
    attenuation.exp = 0.0f;
  }

  /**
   * Calculate the bounding-box of the point-light
   * The bounding-box is calculated with the attenuation of the light
   *
   * @returns size of the bounding-box
   */
  float CalcBoundingSphere() {

    float max_channel = glm::max(glm::max(color.r, color.g), color.b);
    return (-attenuation.linear +
            glm::sqrt(attenuation.linear * attenuation.linear -
                      4 * attenuation.exp * (attenuation.exp -
                                             256 * max_channel *
                                             diffuse_intensity))) / 2 *
        attenuation.exp;
  }
};

} // namespace oncgl

#endif // ONCGL_LIGHT_LIGHT_H
