#include "camera.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <tgmath.h>

void Camera::crouching(bool isCrouching) {
  if (isCrouching)
    heightMultiplier = 0.833f;
  else
    heightMultiplier = 1.0f;
  computeViewMatrix();
}

void Camera::computeProjectionMatrix(glm::vec2 const &size) {
  m_projMatrix = glm::mat4(1.0f);
  auto const aspect{size.x / size.y};
  m_projMatrix = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 5.0f);
}

void Camera::computeViewMatrix() {
  if (groundColision())
    m_eye.y = std::max(0.5f, m_eye.y);

  auto aux_m_at = m_at;
  auto aux_m_eye = m_eye;
  aux_m_at.y *= heightMultiplier;
  aux_m_eye.y *= heightMultiplier;
  m_viewMatrix = glm::lookAt(aux_m_eye, aux_m_at, m_up);
}

void Camera::trackball(TrackBall trackBall, bool left_click) {
  if (!left_click) {
    m_trackBall = trackBall;
    return;
  }
  glm::mat4 transform{1.0f};
  auto const forward{glm::normalize(m_at - m_eye)};
  auto const left{glm::cross(m_up, forward)};

  // Rotate camera around its local x and y axis
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform,
                          m_trackBall.getXY()[0] - trackBall.getXY()[0], m_up);
  transform = glm::rotate(transform,
                          m_trackBall.getXY()[1] - trackBall.getXY()[1], left);
  transform = glm::translate(transform, -m_eye);

  m_at = transform * glm::vec4(m_at, 1.0f);

  m_trackBall = trackBall;
  computeViewMatrix();
}

void Camera::dolly(float speed) {
  // Compute forward vector (view direction)
  auto aux_at = m_at;
  aux_at[1] = 0.0f;
  auto forward{glm::normalize(aux_at - m_eye)};

  if (cubeSideColision(m_eye + forward * speed))
    return;

  // Move eye and center forward (speed > 0) or backward (speed < 0)
  m_eye += forward * speed;
  m_at += forward * speed;

  computeViewMatrix();
}

void Camera::truck(float speed) {
  // Compute forward vector (view direction)
  auto aux_at = m_at;
  aux_at[1] = 0.0f;
  auto forward{glm::normalize(aux_at - m_eye)};
  // Compute vector to the left
  auto const left{glm::cross(m_up, forward)};

  if (cubeSideColision(m_eye - left * speed))
    return;
  // Move eye and center to the left (speed < 0) or to the right (speed > 0)
  m_eye -= left * speed;
  m_at -= left * speed;

  computeViewMatrix();
}

bool Camera::cubeSideColision(glm::vec3 eye) {
  if ((eye[1] > -1.5 && eye[1] < 0.25) &&
      ((static_cast<int>(abs(eye[0]) / 0.5 + 0.5)) % 2 == 1)) {
    return true;
  }
  return false;
}

bool Camera::groundColision() {
  if ((m_eye[1] > 0.5 - 0.01 && m_eye[1] < 0.5 + 0.01) &&
      ((static_cast<int>(abs(m_eye[0]) / 0.5 + 0.5)) % 2 == 1)) {
    return true;
  }
  return false;
}

bool Camera::insideBlockColision() {
  if ((m_eye[1] > -1.5 && m_eye[1] < 0.5) &&
      ((static_cast<int>(abs(m_eye[0]) / 0.5 + 0.5)) % 2 == 1)) {
    return true;
  }
  return false;
}

void Camera::jump(bool jump, float deltaTime) {
  if (m_eye[1] < -4.0f) {
    m_eye = {0.5f, 2.5f, 0.5f};
    computeViewMatrix();
    return;
  }

  if (insideBlockColision()) {
    m_eye[1] = 0.5f;
  }

  if (groundColision()) {
    if (jump)
      fallingSpeed = jumpSpeed;
    else
      fallingSpeed = 0.0;
  } else
    fallingSpeed += deltaTime * gravity * 0.1;

  fallingSpeed = std::max(fallingSpeed, terminalSpeed);

  m_at += m_up * fallingSpeed * deltaTime;
  m_eye += m_up * fallingSpeed * deltaTime;

  computeViewMatrix();
}