project(lookat)
add_executable(${PROJECT_NAME} camera.cpp main.cpp window.cpp trackball.cpp model.cpp)
enable_abcg(${PROJECT_NAME})
