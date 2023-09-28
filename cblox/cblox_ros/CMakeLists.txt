cmake_minimum_required(VERSION 2.8.3)
macro(cs_cuda_add_library _target)
  if(${_target} STREQUAL ${PROJECT_NAME}_package)
    message(WARNING "Could not create library with name '${_target}' as '${PROJECT_NAME}_package' is reserved for the top level target name for this project.")
  endif()
  cmake_parse_arguments(cs_cuda_add_library "NO_AUTO_LINK;NO_AUTO_DEP;NO_AUTO_EXPORT" "" "" ${ARGN})
  cuda_add_library(${_target} ${cs_cuda_add_library_UNPARSED_ARGUMENTS} OPTIONS --compiler-options;-std=c++17; --extended-lambda;--compiler-options;-fPIC)
  if(NOT cs_cuda_add_library_NO_AUTO_LINK)
    target_link_libraries(${_target} ${CUDA_LIBRARIES} ${catkin_LIBRARIES})
  endif()
  if(NOT cs_cuda_add_library_NO_AUTO_DEP)
    if(NOT "${${PROJECT_NAME}_CATKIN_BUILD_DEPENDS_EXPORTED_TARGETS}" STREQUAL "")
      add_dependencies(${_target} ${${PROJECT_NAME}_CATKIN_BUILD_DEPENDS_EXPORTED_TARGETS})
    endif()
  endif()
  if(NOT cs_cuda_add_library_NO_AUTO_EXPORT)
    list(APPEND ${PROJECT_NAME}_LIBRARIES ${_target})
  endif()
  cs_add_targets_to_package(${_target})
endmacro()

project(cblox_ros)
find_package(CUDA REQUIRED)


find_package(catkin_simple REQUIRED)
catkin_simple(ALL_DEPS_REQUIRED)

add_definitions(-std=c++17 -Wall -Wextra)

set(LIB_TYPE STATIC)
cs_cuda_add_library(CUDASampLib2 ${LIB_TYPE} include/cblox_ros/dummy.cu )
#############
# LIBRARIES #
#############
cs_add_library(${PROJECT_NAME}
    src/submap_server.cc
  src/active_submap_visualizer.cc
  src/trajectory_visualizer.cc
    src/submap_conversions.cc
)
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})

############
# BINARIES #
############

cs_add_executable(tsdf_submap_server
  src/tsdf_submap_server_node.cc
)
cs_add_executable(esdf_submap_server
  src/esdf_submap_server_node.cc
)
target_link_libraries(tsdf_submap_server ${PROJECT_NAME})
target_link_libraries(esdf_submap_server ${PROJECT_NAME})


TARGET_LINK_LIBRARIES(${PROJECT_NAME}
${catkin_LIBRARIES}
CUDASampLib2)

##########
# EXPORT #
##########
cs_install()
cs_export()
