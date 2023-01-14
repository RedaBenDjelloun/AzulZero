cmake_minimum_required(VERSION 2.6)
file(TO_CMAKE_PATH "$ENV{IMAGINEPP_ROOT}/CMake" p)
list(APPEND CMAKE_MODULE_PATH "${p}") #For old Imagine++
list(APPEND CMAKE_SYSTEM_FRAMEWORK_PATH /Library/Frameworks) #Mac, why not auto?
find_package(Imagine REQUIRED)

project(AzulZero)
add_executable(AzulZero
               main.cpp
               board.cpp board.h
               controller.cpp controller.h
               GUI.cpp GUI.h
               Tree.h
               game.h game.cpp)
ImagineUseModules(AzulZero Graphics)
