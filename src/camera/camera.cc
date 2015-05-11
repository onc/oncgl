#include <cmath>
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace oncgl {

//must be less than 90 to avoid gimbal lock
static const float MaxVerticalAngle = 85.0f;

Camera::Camera() :
    position_(0.0f, 0.0f, 1.0f),
    horizontal_angle_(0.0f),
    vertical_angle_(0.0f),
    field_of_view_(50.0f),
    near_plane_(0.01f),
    far_plane_(800.0f),
    viewport_aspect_ratio_(4.0f / 3.0f) {
}

const glm::vec3 &Camera::position() const {
  return position_;
}

void Camera::set_position(const glm::vec3 &position) {
  position_ = position;
}

void Camera::OffsetPosition(const glm::vec3 &offset) {
  position_ += offset;
}

float Camera::field_of_view() const {
  return field_of_view_;
}

void Camera::set_field_of_view(float field_of_view) {
  assert(field_of_view > 0.0f && field_of_view < 180.0f);
  field_of_view_ = field_of_view;
}

float Camera::near_plane() const {
  return near_plane_;
}

float Camera::far_plane() const {
  return far_plane_;
}

void Camera::set_near_and_far_planes(float near_plane, float far_plane) {
  assert(near_plane > 0.0f);
  assert(far_plane > near_plane);
  near_plane_ = near_plane;
  far_plane_ = far_plane;
}

glm::mat4 Camera::orientation() const {
  glm::mat4 orientation;
  orientation = glm::rotate(orientation, glm::radians(vertical_angle_),
                            glm::vec3(1, 0, 0));
  orientation = glm::rotate(orientation, glm::radians(horizontal_angle_),
                            glm::vec3(0, 1, 0));
  return orientation;
}

void Camera::offset_orientation(float up_angle, float right_angle) {
  horizontal_angle_ += right_angle;
  vertical_angle_ += up_angle;
  NormalizeAngles();
}

void Camera::LookAt(glm::vec3 position) {
  assert(position != position_);
  glm::vec3 direction = glm::normalize(position - position_);
  vertical_angle_ = glm::radians(asinf(-direction.y));
  horizontal_angle_ = -glm::radians(atan2f(-direction.x, -direction.z));
  NormalizeAngles();
}

float Camera::viewport_aspect_ratio() const {
  return viewport_aspect_ratio_;
}

void Camera::set_viewport_aspect_ratio(float viewport_aspect_ratio) {
  assert(viewport_aspect_ratio > 0.0);
  viewport_aspect_ratio_ = viewport_aspect_ratio;
}

glm::vec3 Camera::Forward() const {
  glm::vec4 forward = glm::inverse(orientation()) * glm::vec4(0, 0, -1, 1);
  return glm::vec3(forward);
}

glm::vec3 Camera::Right() const {
  glm::vec4 right = glm::inverse(orientation()) * glm::vec4(1, 0, 0, 1);
  return glm::vec3(right);
}

glm::vec3 Camera::Up() const {
  glm::vec4 up = glm::inverse(orientation()) * glm::vec4(0, 1, 0, 1);
  return glm::vec3(up);
}

glm::mat4 Camera::matrix() const {
  return projection() * view();
}

glm::mat4 Camera::projection() const {
  return glm::perspective(glm::radians(field_of_view_), viewport_aspect_ratio_,
                          near_plane_, far_plane_);
}

glm::mat4 Camera::view() const {
  return orientation() * glm::translate(glm::mat4(), -position_);
}

void Camera::NormalizeAngles() {
  horizontal_angle_ = fmodf(horizontal_angle_, 360.0f);
  //fmodf can return negative values, but this will make them all positive
  if (horizontal_angle_ < 0.0f) {
    horizontal_angle_ += 360.0f;
  }

  if (vertical_angle_ > MaxVerticalAngle) {
    vertical_angle_ = MaxVerticalAngle;
  } else if (vertical_angle_ < -MaxVerticalAngle) {
    vertical_angle_ = -MaxVerticalAngle;
  }
}

float Camera::horizontal_angle() {
  return horizontal_angle_;
}

float Camera::vertical_angle() {
  return vertical_angle_;
}

} // namespace oncgl