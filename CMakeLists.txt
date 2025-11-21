cmake_minimum_required(VERSION 3.25)

project(test-cef VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 COMPONENTS Core Widgets)
find_package(ThirdpartyOpenSSL CONFIG REQUIRED)
find_package(libcef CONFIG REQUIRED)

add_subdirectory(browser-app)
