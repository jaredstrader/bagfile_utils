cmake_minimum_required(VERSION 3.0.2)
project(rosbag_utils)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  sensor_msgs
  cv_bridge
  image_transport
  rosbag
  )

find_package(OpenCV)

catkin_package(
  INCLUDE_DIRS
  CATKIN_DEPENDS roscpp std_msgs cv_bridge image_transport
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(decompress src/decompress.cpp)
target_link_libraries(decompress 
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  )
