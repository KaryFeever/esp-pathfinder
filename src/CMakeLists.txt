cmake_minimum_required(VERSION 3.5)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../components")
include($ENV{IDF_PATH}/tools/cmake/project.cmake $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
project(esp-pathfinder)