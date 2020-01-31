cmake_minimum_required(VERSION 3.14)
project (io-pdal VERSION 0.0.1)

find_package(geoflow REQUIRED)

#find_package(PDAL REQUIRED)
include_directories( ${PDAL_INCLUDE_DIRS} )
link_directories( ${PDAL_LIBRARY_DIRS} )

# ghc filesystem
if(EXISTS "${PROJECT_SOURCE_DIR}/.gitmodules")
  execute_process(
    COMMAND             git submodule update --init filesystem
    WORKING_DIRECTORY   ${PROJECT_SOURCE_DIR}/thirdparty
    )
endif() 

set(GF_PLUGIN_NAME ${PROJECT_NAME})
set(GF_PLUGIN_TARGET_NAME "gfp_pdal")
set(GF_PLUGIN_REGISTER ${PROJECT_SOURCE_DIR}/register.hpp)
geoflow_create_plugin(
  nodes.cpp
)
target_include_directories(gfp_pdal PRIVATE thirdparty/filesystem/include)
target_link_libraries( gfp_pdal PRIVATE
  geoflow-core
  pdalcpp
)
target_compile_options( gfp_pdal PUBLIC
  "$<$<CONFIG:RELEASE>:-O3>"
)
