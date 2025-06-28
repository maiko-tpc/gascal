# Use the code in this section for standalone projects
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(gascal)
if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()

# ---Build executables----------------------------------------------------------
add_executable(mag mag.cxx)
target_link_libraries(mag Garfield::Garfield)

add_executable(plot plot.cxx)
target_link_libraries(plot Garfield::Garfield)

