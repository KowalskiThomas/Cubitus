cmake_minimum_required(VERSION 3.5)

project(Cubitus LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -O0")

# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG_OUTPUT")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_INFO_OUTPUT")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_WARNING_OUTPUT")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_ERROR_OUTPUT")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

add_library(b2
    b2.cpp b2.h
    networkaccessmanager.cpp networkaccessmanager.h
    bucket.h bucket.cpp
    file.h file.cpp
)
target_link_libraries(b2 PRIVATE Qt5::Network)

add_executable(Cubitus
    main.cpp
    config.h
    mainwindow.cpp    mainwindow.h    mainwindow.ui
    filestreemodel.cpp filestreemodel.h
    filestreetable.cpp filestreetable.h
    filesmodel.cpp filesmodel.h
    filetreeitem.cpp filetreeitem.h
    b2test.cpp b2test.h
)
target_link_libraries(Cubitus PRIVATE Qt5::Widgets Qt5::Network)
target_link_libraries(Cubitus PRIVATE b2)
target_include_directories(Cubitus PRIVATE cmake-build-debug)
