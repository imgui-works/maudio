cmake_minimum_required(VERSION 3.2)

project(PackageBuilder)

INCLUDE(ExternalProject)

add_library(PackageBuilder STATIC builder.cpp)
add_dependencies(PackageBuilder sdl2)
