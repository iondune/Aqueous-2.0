cmake_minimum_required(VERSION 2.6)
project(ionShark)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(ionEngine_SOURCE_DIR thirdparty/ionEngine)
set(SOURCES 
   src/CApplication.cpp
   src/CDataManager.cpp
   src/CDataSet.cpp
   src/CParticleSystem.cpp
   src/CViewerState.cpp
   src/CVolumeSceneObject.cpp
   src/CWaterSurfaceSceneObject.cpp
   src/CWindowWidget.cpp
   src/Main.cpp
   src/shark/Model.cpp 
   src/shark/Skeleton.cpp 
   src/shark/SharkSceneObject.cpp
   src/shark/SharkSpineOscilator.cpp
   src/shark/Transform.cpp
   src/spline/KeySpline.cpp
   src/spline/SplineSceneObject.cpp)

#Dependencies
add_subdirectory(${ionEngine_SOURCE_DIR})
include_directories("${ionEngine_SOURCE_DIR}")
include_directories("${ionEngine_SOURCE_DIR}/Include")
include_directories("${ionEngine_SOURCE_DIR}/Dependencies")
include_directories("${ionEngine_SOURCE_DIR}/Dependencies/stb")
include_directories("${ionEngine_SOURCE_DIR}/Dependencies/tinyformat")
include_directories("${ionEngine_SOURCE_DIR}/Dependencies/glm")
include_directories("${ionEngine_SOURCE_DIR}/Dependencies/imgui")

include_directories("${ionEngine_SOURCE_DIR}/Dependencies/Catch/include")
include_directories("${ionEngine_SOURCE_DIR}/Dependencies/glad/include")

IF(APPLE)
FIND_LIBRARY(APPKIT appkit)
   MARK_AS_ADVANCED (APPKIT)
SET(FRAMEWORK_LIBS ${APPKIT})
ENDIF()


#nfd
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/nfd/include")
link_directories("${PROJECT_SOURCE_DIR}/thirdparty/nfd")


include_directories("$ENV{EIGEN3_INCLUDE_DIR}")
include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(SharkProject ${SOURCES})

target_link_libraries(SharkProject ionScene)
target_link_libraries(SharkProject ionGraphicsGL)
target_link_libraries(SharkProject ionGraphics)
target_link_libraries(SharkProject ionWindow)
target_link_libraries(SharkProject ionGUI)
target_link_libraries(SharkProject ionFramework)
target_link_libraries(SharkProject ionApplication)
target_link_libraries(SharkProject assimp)
target_link_libraries(SharkProject nfd)
target_link_libraries(SharkProject ${FRAMEWORK_LIBS})

ExecutableDependencies(SharkProject)
