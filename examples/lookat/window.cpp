#include "window.hpp"
#include <unordered_map>

// For debugging
#include <glm/gtx/string_cast.hpp>
#include <iostream>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onEvent(SDL_Event const &event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    // std::cout << glm::to_string(glm::vec3(1.0f))  << std::endl;
    m_trackBall.mouseMove(m_viewportSize - mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mousePress(m_viewportSize - mousePosition);
    pressed = true;
  }
  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mouseRelease(m_viewportSize - mousePosition);
    pressed = false;
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? -1.0f : 1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }

  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_truckSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_truckSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_q)
      m_panSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_e)
      m_panSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_SPACE)
      avodaco = true;
    if (event.key.keysym.sym == SDLK_LSHIFT) {
      m_camera.crouching(true);
      crouchReducer = 0.72;
    }
    if (event.key.keysym.sym == SDLK_LCTRL) {
      crouchReducer = 2.0;
    }
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_RIGHT ||
         event.key.keysym.sym == SDLK_d) &&
        m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_q && m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_e && m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_SPACE)
      avodaco = false;
    if (event.key.keysym.sym == SDLK_LSHIFT) {
      m_camera.crouching(false);
      crouchReducer = 1.5;
    }
    if (event.key.keysym.sym == SDLK_LCTRL) {
      crouchReducer = 1.5;
    }
  }
}

void Window::onCreate() {

  auto const &assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  // m_program =
  //     abcg::createOpenGLProgram({{.source = assetsPath + "blinnphong.vert",
  //                                 .stage = abcg::ShaderStage::Vertex},
  //                                {.source = assetsPath + "blinnphong.frag",
  //                                 .stage = abcg::ShaderStage::Fragment}});

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "lookat.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "lookat.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");

  // Load model
  loadModelFromFile(assetsPath + "box.obj");

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attributes{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      auto const vx{attributes.vertices.at(startIndex + 0)};
      auto const vy{attributes.vertices.at(startIndex + 1)};
      auto const vz{attributes.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = {vx, vy, vz}};

      // If map doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void Window::onPaint() {
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  abcg::glBindVertexArray(m_VAO);

  const int groundCount{15};
  for (int x = 0; x < groundCount; x++) {
    for (int y = 0; y < groundCount; y++) {
      glm::mat4 model{1.0f};
      model = glm::translate(model,
                             glm::vec3(-(groundCount / 2.0 - 1.0f) + x, -0.5f,
                                       -(groundCount / 2.0 - 1.0f) + y));
      model = glm::scale(model, glm::vec3(0.5f));

      abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE,
                               &model[0][0]);
      abcg::glUniform4f(m_colorLocation, x / (groundCount * 1.0),
                        (x + y) / (groundCount * 2.0f), y / (groundCount * 1.0),
                        1.0f);
      abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                           nullptr);
    }
  }
  abcg::glUseProgram(0);
}

void Window::onPaintUI() { abcg::OpenGLWindow::onPaintUI(); }

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
  m_trackBall.resizeViewport(size);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime * crouchReducer);
  m_camera.truck(m_truckSpeed * deltaTime * crouchReducer);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.trackball(m_trackBall, pressed);
  m_camera.jump(avodaco, deltaTime);
}













// void Window::onCreate() {

//   auto const &assetsPath{abcg::Application::getAssetsPath()};

//   abcg::glClearColor(0, 0, 0, 1);

//   // Enable depth buffering
//   abcg::glEnable(GL_DEPTH_TEST);

//   // Create program
//   m_program =
//       abcg::createOpenGLProgram({{.source = assetsPath + "lookat.vert",
//                                   .stage = abcg::ShaderStage::Vertex},
//                                  {.source = assetsPath + "lookat.frag",
//                                   .stage = abcg::ShaderStage::Fragment}});

//   m_ground.create(m_program);

//   // Get location of uniform variables
//   m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
//   m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
//   m_modelMatrixLocation = abcg::glGetUniformLocation(m_program,
//   "modelMatrix"); m_colorLocation = abcg::glGetUniformLocation(m_program,
//   "color");

//   // clang-format off
//   // Creating Cube
//   std::array positions{
//       glm::vec3{-1.0f, -1.0f, -1.0f},
//       glm::vec3{-1.0f, -1.0f, 1.0f},
//       glm::vec3{-1.0f, 1.0f, -1.0f},
//       glm::vec3{-1.0f, 1.0f, 1.0f},
//       glm::vec3{1.0f, -1.0f, -1.0f},
//       glm::vec3{1.0f, -1.0f, 1.0f},
//       glm::vec3{1.0f, 1.0f, -1.0f},
//       glm::vec3{1.0f, 1.0f, 1.0f},
//   };

//   std::array const indices{
//    0, 1, 2,
//    1, 3, 2,
//    4, 5, 6,
//    5, 7, 6,
//    0, 4, 2,
//    4, 6, 2,
//    1, 5, 3,
//    2, 7, 3,
//    2, 6, 3,
//    6, 7, 3,
//    0, 4, 1,
//    4, 5, 1
//    };
//   // clang-format on

//   // VBO
//   abcg::glGenBuffers(1, &m_VBO);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//   abcg::glBufferData(GL_ARRAY_BUFFER,
//                      sizeof(positions),
//                      positions.data(), GL_STATIC_DRAW);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   // EBO
//   abcg::glGenBuffers(1, &m_EBO);
//   abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
//   abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//                      sizeof(indices),
//                      indices.data(), GL_STATIC_DRAW);
//   abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

//   // Create VAO
//   abcg::glGenVertexArrays(1, &m_VAO);

//   // Bind vertex attributes to current VAO
//   abcg::glBindVertexArray(m_VAO);

//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//   auto const positionAttribute{
//       abcg::glGetAttribLocation(m_program, "inPosition")};
//   abcg::glEnableVertexAttribArray(positionAttribute);
//   abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
//                               sizeof(Vertex), nullptr);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

//   // End of binding to current VAO
//   abcg::glBindVertexArray(0);
// }