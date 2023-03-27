set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(QT_USE_QTMAIN TRUE)
if(NOT CMAKE_BUILD_TYPE)
  # set(CMAKE_BUILD_TYPE "Release" CACHE STRING
  #     "Default build type: Release" FORCE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Default build type: Debug" FORCE)
endif()

add_executable(ScoutingAnalyzer
  main.cpp
  datasetpage.cpp
  datasetpage.h
  mainwindow.cpp
  mainwindow.h
  teamdata.h
  qwidgetwithclick.h
  qcustomplot/qcustomplot.cpp
  qcustomplot/qcustomplot.h
)

target_link_libraries(ScoutingAnalyzer PRIVATE Qt5::Widgets Qt5::PrintSupport )

install(
  TARGETS ScoutingAnalyzer
  RUNTIME DESTINATION bin
)