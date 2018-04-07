CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(mocap_player)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PACKAGE(assimp REQUIRED)
FIND_PACKAGE(glm REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)

SET(CMAKE_CXX_COMPILER "/usr/bin/g++")
SET(CMAKE_CXX_FLAGS "-std=c++11 -O0")
SET(CMAKE_BUILD_TYPE Debug)


SET(M_INCLUDE_DIRS "./include" "/usr/local/include")
SET(M_LINK_DIRS "/usr/local/lib")


INCLUDE_DIRECTORIES(${M_INCLUDE_DIRS})
LINK_DIRECTORIES(${M_LINK_DIRS})

ADD_SUBDIRECTORY(src/bvh_parser)
ADD_SUBDIRECTORY(src/scene_utils)
ADD_SUBDIRECTORY(src/gl_utils)
ADD_SUBDIRECTORY(src/pose_utils)
ADD_SUBDIRECTORY(src/cal_utils)

SET(M_LINK_LIBS ${M_LINK_LIBS} bvh_parser scene_utils gl_utils pose_utils cal_utils) # Set link libraries of mine
SET(M_LINK_LIBS ${M_LINK_LIBS} ${OPENGL_gl_LIBRARY} glfw ${GLEW_LIBRARIES} ${ASSIMP_LIBRARIES}) # Set the link libraries of gl
SET(M_LINK_LIBS ${M_LINK_LIBS} opencv_core opencv_highgui opencv_imgcodecs opencv_imgproc opencv_videoio opencv_calib3d)

ADD_EXECUTABLE(main src/main/main.cpp)
TARGET_LINK_LIBRARIES(main ${M_LINK_LIBS})
