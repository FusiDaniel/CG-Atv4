#include "trackball.hpp"

#include <algorithm>
#include <limits>

// For debugging
#include <glm/gtx/string_cast.hpp>
#include <iostream>

void TrackBall::mouseMove(glm::ivec2 const &position) {
  m_lastXY = updateXY(position);
}

void TrackBall::resizeViewport(glm::ivec2 const &size) {
  m_viewportSize = size;
}

glm::vec3 TrackBall::updateXY(glm::vec2 const &position) const {
  return glm::vec3(
      2.0f * position.x / gsl::narrow<float>(m_viewportSize.x) - 1.0f,
      1.0f - 2.0f * position.y / gsl::narrow<float>(m_viewportSize.y), 0.0f);
}

glm::vec3 TrackBall::getXY() const {
  return m_lastXY;
}
