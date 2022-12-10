#include "camera.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

void Camera::crouching(bool isCrouching){
  if (isCrouching) crouch = 0.833f;
  else crouch = 1.0f;
  computeViewMatrix();
}

void Camera::computeProjectionMatrix(glm::vec2 const &size) {
  m_projMatrix = glm::mat4(1.0f);
  auto const aspect{size.x / size.y};
  m_projMatrix = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 5.0f);
}

void Camera::computeViewMatrix() {
  // std::cout << glm::to_string(m_eye)  << std::endl;
  // std::cout << glm::to_string(m_at)  << std::endl;
  // std::cout << glm::to_string(m_up)  << std::endl;

  // m_at.y = std::max(0.1f, m_at.y);
  m_eye.y = std::max(0.5f, m_eye.y);

  auto aux_m_at = m_at;
  auto aux_m_eye = m_eye;
  aux_m_at.y *= crouch;
  aux_m_eye.y *= crouch;
  m_viewMatrix = glm::lookAt(aux_m_eye, aux_m_at, m_up);
}

void Camera::trackball(TrackBall trackBall, bool pressed) {
  if (!pressed) {
    m_trackBall = trackBall;
    return;
  }
  glm::mat4 transform{1.0f};
  auto const forward{glm::normalize(m_at - m_eye)};
  auto const left{glm::cross(m_up, forward)};
  // Rotate camera around its local y axis
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, m_trackBall.getXY()[0] - trackBall.getXY()[0], m_up);
  transform = glm::rotate(transform, m_trackBall.getXY()[1] - trackBall.getXY()[1], left);
  transform = glm::translate(transform, -m_eye);

  m_at = transform * glm::vec4(m_at, 1.0f);

  m_trackBall = trackBall;
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

void Camera::pan(float speed) {
  glm::mat4 transform{1.0f};
  
   std::cout << speed << std::endl;
  // Rotate camera around its local y axis
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, -speed, m_up);
  transform = glm::translate(transform, -m_eye);

  m_at = transform * glm::vec4(m_at, 1.0f);

  computeViewMatrix();
}

void Camera::jump(bool jump, float deltaTime) {
  if (m_eye[1] > 0.5 - 0.1 && m_eye[1] < 0.5 + 0.1) {
    if (jump)
      upSpeed = jumpSpeed;
    else
      upSpeed = 0.0;
  } else
    upSpeed += deltaTime * gravity * 0.1;

  upSpeed = std::max(upSpeed, -9.0f);
  // std::cout << upSpeed << std::endl;

  m_at += m_up * upSpeed * deltaTime;
  m_eye += m_up * upSpeed * deltaTime;

  computeViewMatrix();
}