#ifndef ONCGL_CAMERA_CAMERA_H
#define ONCGL_CAMERA_CAMERA_H

#include <glm/glm.hpp>

namespace oncgl {

class Camera {
 public:
  Camera();

  const glm::vec3 &position() const;

  void set_position(const glm::vec3 &position);

  void OffsetPosition(const glm::vec3 &offset);

  /**
   * Vertical viewing angle of the camera in degrees
   * The value must be between 0 and 180
   *
   * @return vertical field of view
   */
  float field_of_view() const;

  void set_field_of_view(float field_of_view);

  float near_plane() const;

  float far_plane() const;

  /**
   * Sets the near and far planes
   *
   * @param nearPlane  Near plane of the camera. must be > 0
   * @param farPlane   Far plane of the camera. must be > near plane
   */
  void set_near_and_far_planes(float near_plane, float far_plane);

  /**
   * Rotation matrix that determinates the direction the camera is looking
   * Does not include camera position
   */
  glm::mat4 orientation() const;

  /**
   * Offsets the cameras orientation
   * The verticle angle is constrained between 85 and -85deg to avoid gimbal lock
   *
   * @param upAngle     the angle (in degrees) to offset upwards
   * @param rightAngle  the angle (in degrees) to offset rightwards
   */
  void offset_orientation(float up_angle, float right_angle);

  /**
   * Orients the camera so that is it directly facing `position`
   *
   * @param position  the position to look at
   */
  void LookAt(glm::vec3 position);

  /**
   * The width divided by the height of the screen/window/viewport
   * Incorrect values will make the 3D scene look stretched.
   */
  float viewport_aspect_ratio() const;

  void set_viewport_aspect_ratio(float viewport_aspect_ratio);

  /**
   * A unit vector representing the direction the camera is facing
   */
  glm::vec3 Forward() const;

  /**
   * A unit vector representing the direction to the right of the camera
   */
  glm::vec3 Right() const;

  /**
   * A unit vector representing the direction out of the top of the camera
   */
  glm::vec3 Up() const;

  /**
   * The combined camera transformation matrix, including perspective projection.
   * This is the complete matrix to use in the vertex shader.
   */
  glm::mat4 matrix() const;

  /**
   * The perspective projection transformation matrix
   */
  glm::mat4 projection() const;

  /**
   * The translation and rotation matrix of the camera.
   * Same as the `matrix` method, except the return value does not include the projection
   * transformation.
   */
  glm::mat4 view() const;

  float horizontal_angle();

  float vertical_angle();

 private:
  glm::vec3 position_;
  float horizontal_angle_;
  float vertical_angle_;
  float field_of_view_;
  float near_plane_;
  float far_plane_;
  float viewport_aspect_ratio_;

  void NormalizeAngles();
};

} // namespace oncgl

#endif // ONCGL_CAMERA_CAMERA_H
