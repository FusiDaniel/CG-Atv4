project(helloworld)
add_executable(${PROJECT_NAME} camera.cpp main.cpp window.cpp trackball.cpp)
enable_abcg(${PROJECT_NAME})
