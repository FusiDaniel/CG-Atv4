#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "trackball.hpp"

class Camera {
public:
  void computeViewMatrix();
  void computeProjectionMatrix(glm::vec2 const &size);

  void dolly(float speed);
  void truck(float speed);
  void trackball(TrackBall trackball, bool pressed);
  void jump(bool jumped, float deltaTime);
  void crouching(bool isCrouching);

  glm::mat4 const &getViewMatrix() const { return m_viewMatrix; }
  glm::mat4 const &getProjMatrix() const { return m_projMatrix; }

private:
  glm::vec3 m_eye{0.5f, 2.5f, 0.5f}; // Camera position
  glm::vec3 m_at{0.5f, 0.5f, 0.0f};  // Look-at point
  glm::vec3 m_up{0.0f, 1.0f, 0.0f};  // "up" direction

  // Matrix to change from world space to camera space
  glm::mat4 m_viewMatrix;

  // Matrix to change from camera space to clip space
  glm::mat4 m_projMatrix;

  float fallingSpeed{0.0};
  float jumpSpeed{1.5};
  float gravity{-28.0};
  float terminalSpeed{-9.0};
  float heightMultiplier{1.0};
  bool repeated{true};

  bool groundColision();
  bool cubeSideColision(glm::vec3 eye);
  bool insideBlockColision();
  TrackBall m_trackBall;
};

#endif