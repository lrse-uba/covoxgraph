cmake_minimum_required(VERSION 2.8.3)

macro(cs_cuda_add_library _target)
  if(${_target} STREQUAL ${PROJECT_NAME}_package)
    message(WARNING "Could not create library with name '${_target}' as '${PROJECT_NAME}_package' is reserved for the top level target name for this project.")
  endif()
  cmake_parse_arguments(cs_cuda_add_library "NO_AUTO_LINK;NO_AUTO_DEP;NO_AUTO_EXPORT" "" "" ${ARGN})
  cuda_add_library(${_target} ${cs_cuda_add_library_UNPARSED_ARGUMENTS} OPTIONS --compiler-options;-std=c++17; --extended-lambda;--expt-relaxed-constexpr;--compiler-options;-fPIC)
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

project(voxgraph)
find_package(CUDA REQUIRED)
add_definitions(-std=c++17 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-deprecated-copy -fPIC -DEIGEN_INITIALIZE_MATRICES_BY_NAN -fdiagnostics-color=auto)

find_package(catkin_simple REQUIRED)
catkin_simple(ALL_DEPS_REQUIRED)

############
# PROTOBUF #
############
# General idea: first check if we have protobuf catkin, then use that.
# Otherwise use system protobuf.
set(PROTO_LIBRARIES "")
find_package(protobuf_catkin QUIET)
if (protobuf_catkin_FOUND)
  message(STATUS "Using protobuf_catkin")
  set(PROTO_LIBRARIES ${protobuf_catkin_LIBRARIES})
else()
  message(STATUS "Using system protobuf")
  find_package(Protobuf REQUIRED)
  set(PROTO_LIBRARIES ${PROTOBUF_LIBRARIES})
endif()

#############
# LIBRARIES #
#############
# NOTE: The library is static to avoid segfaults that
#       otherwise occur in voxblox multi threading
cs_add_library(${PROJECT_NAME} STATIC
    src/frontend/voxgraph_mapper.cpp
    src/frontend/pose_graph_interface/pose_graph_interface.cpp
    src/frontend/pose_graph_interface/node_templates.cpp
    src/frontend/pose_graph_interface/measurement_templates.cpp
    src/frontend/measurement_processors/pointcloud_integrator.cpp
    src/frontend/submap_collection/bounding_box.cpp
    src/frontend/submap_collection/submap_timeline.cpp
    src/frontend/submap_collection/voxgraph_submap.cpp
    src/frontend/submap_collection/voxgraph_submap_collection.cpp
    src/frontend/map_tracker/map_tracker.cpp
    src/frontend/map_tracker/transformers/tf_transformer.cpp
    src/frontend/map_tracker/transformers/odometry_transformer.cpp
    src/backend/pose_graph.cpp
    src/backend/node/node.cpp
    src/backend/node/node_collection.cpp
    src/backend/node/pose/pose_4d.cpp
    src/backend/node/pose/pose_6d.cpp
    src/backend/constraint/constraint.cpp
    src/backend/constraint/constraint_collection.cpp
    src/backend/constraint/relative_pose_constraint.cpp
    src/backend/constraint/absolute_pose_constraint.cpp
    src/backend/constraint/registration_constraint.cpp
    src/backend/constraint/cost_functions/registration_cost_function.cpp
    src/tools/rosbag_helper.cpp
    src/tools/tf_helper.cpp
    src/tools/submap_registration_helper.cpp
    src/tools/evaluation/map_evaluation.cpp
    src/tools/data_servers/projected_map_server.cpp
    src/tools/data_servers/submap_server.cpp
    src/tools/data_servers/loop_closure_edge_server.cpp
    src/tools/odometry_simulator/odometry_simulator.cpp
    src/tools/visualization/cost_function_visuals.cpp
    src/tools/visualization/pose_graph_visuals.cpp
    src/tools/visualization/submap_visuals.cpp
    src/tools/visualization/loop_closure_visuals.cpp
)

cs_cuda_add_library(cuda_voxgraph_lib ${LIB_TYPE} include/voxgraph/cuda/dummy.cu )

############
# BINARIES #
############
cs_add_executable(voxgraph_mapping_node
    src/voxgraph_mapping_node.cpp)
target_link_libraries(voxgraph_mapping_node ${PROJECT_NAME} ${PROTO_LIBRARIES})

cs_add_executable(odometry_simulator
    src/odometry_simulator_node.cpp)
target_link_libraries(odometry_simulator ${PROJECT_NAME} ${PROTO_LIBRARIES})

cs_add_executable(registration_test_bench
    src/tools/test_benches/registration_test_bench.cpp)
target_link_libraries(registration_test_bench ${PROJECT_NAME} ${PROTO_LIBRARIES})

add_dependencies(voxgraph_mapping_node cuda_voxgraph_lib)
add_dependencies(odometry_simulator cuda_voxgraph_lib)
add_dependencies(registration_test_bench cuda_voxgraph_lib)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
${catkin_LIBRARIES}
cuda_voxgraph_lib)

##########
# EXPORT #
##########
cs_export()

####################
# GIT VERSION INFO #
####################
# Make the git branch name and git commit hash available in the sources as
# a #define, such that it can easily be appended to logs
IF(EXISTS ${voxgraph_SOURCE_DIR}/../.git)
  find_package(Git)
  IF(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${voxgraph_SOURCE_DIR}"
        OUTPUT_VARIABLE "voxgraph_GIT_BRANCH"
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
        WORKING_DIRECTORY "${voxgraph_SOURCE_DIR}"
        OUTPUT_VARIABLE "voxgraph_GIT_COMMIT_HASH"
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Defining GIT_BRANCH: ${voxgraph_GIT_BRANCH}")
    message(STATUS "Defining GIT_COMMIT_HASH: ${voxgraph_GIT_COMMIT_HASH}")
  ELSE(GIT_FOUND)
    set(voxgraph_GIT_BRANCH 0)
    set(voxgraph_GIT_COMMIT_HASH 0)
  ENDIF(GIT_FOUND)
ENDIF(EXISTS ${voxgraph_SOURCE_DIR}/../.git)
add_definitions("-DGIT_BRANCH=\"${voxgraph_GIT_BRANCH}\"")
add_definitions("-DGIT_COMMIT_HASH=\"${voxgraph_GIT_COMMIT_HASH}\"")
