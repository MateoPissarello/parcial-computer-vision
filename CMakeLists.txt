cmake_minimum_required(VERSION 3.20)
project(TALLER-TERCER-CORTE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Si hace falta:
# set(OpenCV_DIR "/usr/lib/cmake/opencv4")

# PIDE SOLO LOS MÓDULOS QUE USAS (sin dnn)
find_package(OpenCV REQUIRED
  COMPONENTS core imgproc highgui features2d xfeatures2d objdetect videoio imgcodecs
)

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

# ENLAZA EXPLÍCITO (NO uses ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} PRIVATE
  opencv_core
  opencv_imgproc
  opencv_highgui
  opencv_features2d
  opencv_xfeatures2d
  opencv_objdetect
  opencv_videoio
  opencv_imgcodecs
)