cmake_minimum_required(VERSION 3.4.0 FATAL_ERROR)

add_executable(visualizer visualizer.cpp)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Python3 COMPONENTS Development NumPy)

target_link_libraries(visualizer Eigen3::Eigen)

target_include_directories(visualizer PRIVATE ${Python2_INCLUDE_DIRS} ${Python2_NumPy_INCLUDE_DIRS})
target_link_libraries(visualizer Python3::Python Python3::NumPy)
find_package(glfw3 3.3 REQUIRED)

include_directories(/usr/include/GL/)
include_directories(../Utils)
include_directories(../Sockets)

find_library(GLEW_LIB NAMES glew)
find_library(GL_LIB NAMES GL)
target_link_libraries(visualizer glfw)
target_link_libraries(visualizer ${GLEW_LIB})
target_link_libraries(visualizer ${GL_LIB})

configure_file("./Sailing_Boat.obj" ${PROJECT_BINARY_DIR}/bin COPYONLY)
configure_file("./Sail_Boat_Without_Sails.obj" ${PROJECT_BINARY_DIR}/bin COPYONLY)
configure_file("./Sail.obj" ${PROJECT_BINARY_DIR}/bin COPYONLY)
configure_file("./Colline.obj" ${PROJECT_BINARY_DIR}/bin COPYONLY)
configure_file("./Wind_Arrow.obj" ${PROJECT_BINARY_DIR}/bin COPYONLY)
configure_file("./Current_Arrow.obj" ${PROJECT_BINARY_DIR}/bin COPYONLY)
configure_file("./Force_Arrow.obj" ${PROJECT_BINARY_DIR}/bin COPYONLY)
configure_file("./vertShader.glsl" ${PROJECT_BINARY_DIR}/bin COPYONLY)
configure_file("./fragShader.glsl" ${PROJECT_BINARY_DIR}/bin COPYONLY)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


