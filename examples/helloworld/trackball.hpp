#ifndef TRACKBALL_HPP_
#define TRACKBALL_HPP_

#include "abcg.hpp"

class TrackBall {
public:
  void mouseMove(glm::ivec2 const &position);
  void resizeViewport(glm::ivec2 const &size);
  [[nodiscard]] glm::vec3 getXY() const;

private:
  glm::vec3 m_lastXY{};
  glm::ivec2 m_viewportSize{};

  [[nodiscard]] glm::vec3 updateXY(glm::vec2 const &mousePosition) const;
};

#endif
