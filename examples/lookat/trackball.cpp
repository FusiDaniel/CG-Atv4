#include "trackball.hpp"

#include <algorithm>
#include <limits>

// For debugging
#include <glm/gtx/string_cast.hpp>
#include <iostream>

void TrackBall::mouseMove(glm::ivec2 const &position) {
  m_lastXY = setXY(position);
  if (!m_mouseTracking)
    return;
  
  auto const currentPosition{project(position)};
  
  // std::cout << glm::to_string(currentPosition)  << std::endl;
  if (m_lastPosition == currentPosition) {
    // Scale velocity to zero if time since last event > 10ms
    m_velocity *= m_lastTime.elapsed() > 0.01 ? 0.0 : 1.0;
    return;
  }

  // Rotation axis
  m_axis = glm::cross(m_lastPosition, currentPosition);
  // Rotation angle
  auto const angle{glm::length(m_axis)};

  m_axis = glm::normalize(m_axis);
  
  // std::cout << glm::to_string(m_axis)  << std::endl;
  // Compute an angle velocity that will be used as a constant rotation angle
  // when the mouse is not being tracked.
  m_velocity = angle / (gsl::narrow_cast<float>(m_lastTime.restart()) +
                        std::numeric_limits<float>::epsilon());
  m_velocity = glm::clamp(m_velocity, 0.0f, m_maxVelocity);

  // Concatenate rotation: R_old = R_new * R_old
  m_rotation = glm::rotate(glm::mat4(1.0f), angle, m_axis) * m_rotation;

  m_lastPosition = currentPosition;
}

void TrackBall::mousePress(glm::ivec2 const &position) {
  m_rotation = getRotation();
  m_mouseTracking = true;
  m_lastTime.restart();
  m_lastPosition = project(position);
  m_velocity = 0.0f;
}

void TrackBall::mouseRelease(glm::ivec2 const &position) {
  mouseMove(position);
  m_mouseTracking = false;
  m_velocity = 0.0f;
}

void TrackBall::resizeViewport(glm::ivec2 const &size) {
  m_viewportSize = size;
}

glm::vec3 TrackBall::setXY(glm::vec2 const &position) const {
  return glm::vec3(
      2.0f * position.x / gsl::narrow<float>(m_viewportSize.x) - 1.0f,
      1.0f - 2.0f * position.y / gsl::narrow<float>(m_viewportSize.y), 0.0f);
}

glm::vec3 TrackBall::getXY() const {
  return m_lastXY;
}


glm::mat4 TrackBall::getRotation() const {
  if (m_mouseTracking)
    return m_rotation;

  // Rotate by velocity when not tracking to simulate an inertia-free rotation
  auto const angle{m_velocity * gsl::narrow_cast<float>(m_lastTime.elapsed())};

  return glm::rotate(glm::mat4(1.0f), angle, m_axis) * m_rotation;
}

glm::vec3 TrackBall::project(glm::vec2 const &position) const {

  // std::cout << glm::to_string(m_viewportSize)  << std::endl;

  // Convert from window coordinates to NDC
  auto projected{glm::vec3(
      2.0f * position.x / gsl::narrow<float>(m_viewportSize.x) - 1.0f,
      1.0f - 2.0f * position.y / gsl::narrow<float>(m_viewportSize.y), 0.0f)};

  // std::cout << glm::to_string(projected)  << std::endl;
  // Project to centered unit hemisphere
  if (auto const squaredLength{glm::length2(projected)};
      squaredLength >= 1.0f) {
    // Outside the sphere
    projected = glm::normalize(projected);
  } else {
    // Inside the sphere
    projected.z = std::sqrt(1.0f - squaredLength);
  }

  return projected;
}