#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <algorithm>


void Camera::computeProjectionMatrix(glm::vec2 const &size) {
  m_projMatrix = glm::mat4(1.0f);
  auto const aspect{size.x / size.y};
  m_projMatrix = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 5.0f);
}

void Camera::computeViewMatrix() {
  std::cout << glm::to_string(m_eye)  << std::endl;
  m_at.y = std::max(0.5f, m_at.y);
  m_eye.y = std::max(0.5f, m_eye.y);
  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}

void Camera::trackball(TrackBall trackBall) {
  m_trackBall = trackBall;
  // std::cout << glm::to_string(rotMatrix.getRotation()) << std::endl;
  computeViewMatrix();
}

void Camera::dolly(float speed) {
  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_at - m_eye)};

  // Move eye and center forward (speed > 0) or backward (speed < 0)
  m_eye += forward * speed;
  m_at += forward * speed;

  computeViewMatrix();
}

void Camera::truck(float speed) {
  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_at - m_eye)};
  // Compute vector to the left
  auto const left{glm::cross(m_up, forward)};

  // Move eye and center to the left (speed < 0) or to the right (speed > 0)
  m_at -= left * speed;
  m_eye -= left * speed;

  computeViewMatrix();
}

void Camera::pan(float speed, float speedX) {
  glm::mat4 transform{1.0f};

  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_at - m_eye)};
  // Compute vector to the left
  auto const left{glm::cross(m_up, forward)};

  // Rotate camera around its local y axis
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, -speed, m_up);
  // transform = glm::rotate(transform, -speedX, left);
  transform = glm::translate(transform, -m_eye);

  m_at -= m_up * speedX;
  m_eye -= m_up * speedX;
  m_at = transform * glm::vec4(m_at, 1.0f);

  computeViewMatrix();
}