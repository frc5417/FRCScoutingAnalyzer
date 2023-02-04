cmake_minimum_required(VERSION 3.5)
set(QT_MINIMUM_VERSION 5.1.0)

set(PROJECT_NAME ScoutingAnalyzer)

project(${PROJECT_NAME})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(QT_USE_QTMAIN TRUE)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
      "Default build type: RelWithDebInfo" FORCE)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME}
  main.cpp
  datasetpage.cpp
  datasetpage.h
  mainwindow.cpp
  mainwindow.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets )
