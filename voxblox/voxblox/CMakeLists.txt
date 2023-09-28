cmake_minimum_required(VERSION 2.8.12)

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

project(voxblox)
find_package(CUDA REQUIRED)
add_definitions(-std=c++17 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-deprecated-copy -fPIC -DEIGEN_INITIALIZE_MATRICES_BY_NAN -fdiagnostics-color=auto -pg)


find_package(catkin_simple REQUIRED )
catkin_simple(ALL_DEPS_REQUIRED)

set(CMAKE_MACOSX_RPATH 0)
add_definitions(-std=c++17 -Wall -Wextra)

############
# PROTOBUF #
############
# General idea: first check if we have protobuf catkin, then use that.
# Otherwise use system protobuf.
set(PROTO_DEFNS proto/voxblox/Block.proto
                proto/voxblox/Layer.proto)
set(BASE_PATH "proto")
set(PROTOBUF_COMPILE_PYTHON true)

PROTOBUF_CATKIN_GENERATE_CPP2(${BASE_PATH} PROTO_SRCS PROTO_HDRS ${PROTO_DEFNS})

####################
# SET SOURCE FILES #
####################

set("${PROJECT_NAME}_SRCS"
  src/alignment/icp.cc
  src/core/block.cc
  src/core/esdf_map.cc
  src/core/tsdf_map.cc
  src/integrator/esdf_integrator.cc
  src/integrator/esdf_occ_integrator.cc
  src/integrator/integrator_utils.cc
  src/integrator/intensity_integrator.cc
  src/integrator/tsdf_integrator.cc
  src/io/mesh_ply.cc
  src/io/sdf_ply.cc
  src/mesh/marching_cubes.cc
  src/simulation/objects.cc
  src/simulation/simulation_world.cc
  src/utils/camera_model.cc
  src/utils/evaluation_utils.cc
  src/utils/layer_utils.cc
  src/utils/neighbor_tools.cc
  src/utils/protobuf_utils.cc
  src/utils/timing.cc
  src/utils/voxel_utils.cc
)

set(LIB_TYPE STATIC)

cs_cuda_add_library(CUDASampLib2 ${LIB_TYPE} include/voxblox/cuda/dummy.cu )


#############
# LIBRARIES #
#############
cs_add_library(${PROJECT_NAME}_proto
  ${PROTO_SRCS}
)

cs_add_library(${PROJECT_NAME}
  ${${PROJECT_NAME}_SRCS}
)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_proto)

############
# BINARIES #
############

add_executable(tsdf_to_esdf
  test/tsdf_to_esdf.cc
)
target_link_libraries(tsdf_to_esdf ${PROJECT_NAME})

add_executable(test_load_esdf
  test/test_load_esdf.cc
)
target_link_libraries(test_load_esdf ${PROJECT_NAME})

#########
# TESTS #
#########
add_custom_target(test_data)
add_custom_command(TARGET test_data
                  COMMAND rm -rf test_data
                  COMMAND mkdir -p test_data
                  COMMAND cp -r ${CMAKE_SOURCE_DIR}/test/test_data/*
                                test_data/ || :)

#add_definitions(-DVISUALIZE_UNIT_TEST_RESULTS)

catkin_add_gtest(test_approx_hash_array
  test/test_approx_hash_array.cc
)
target_link_libraries(test_approx_hash_array ${PROJECT_NAME})

catkin_add_gtest(test_tsdf_map
  test/test_tsdf_map.cc
)
target_link_libraries(test_tsdf_map ${PROJECT_NAME})

catkin_add_gtest(test_protobuf
  test/test_protobuf.cc
)
target_link_libraries(test_protobuf ${PROJECT_NAME})

catkin_add_gtest(test_tsdf_interpolator
  test/test_tsdf_interpolator.cc
)
target_link_libraries(test_tsdf_interpolator ${PROJECT_NAME})

catkin_add_gtest(test_layer
  test/test_layer.cc
)
target_link_libraries(test_layer ${PROJECT_NAME})

catkin_add_gtest(test_merge_integration
  test/test_merge_integration.cc
)
target_link_libraries(test_merge_integration ${PROJECT_NAME})

catkin_add_gtest(test_layer_utils
  test/test_layer_utils.cc
)
target_link_libraries(test_layer_utils ${PROJECT_NAME})

catkin_add_gtest(test_sdf_integrators
  test/test_sdf_integrators.cc
)
target_link_libraries(test_sdf_integrators ${PROJECT_NAME})

catkin_add_gtest(test_bucket_queue
  test/test_bucket_queue.cc
)
target_link_libraries(test_bucket_queue ${PROJECT_NAME})

catkin_add_gtest(test_clear_spheres
  test/test_clear_spheres.cc
)
target_link_libraries(test_clear_spheres ${PROJECT_NAME})

add_dependencies(tsdf_to_esdf CUDASampLib2)
add_dependencies(test_load_esdf CUDASampLib2)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
${catkin_LIBRARIES}
CUDASampLib2)

##########
# EXPORT #
##########
cs_install()
cs_export(INCLUDE_DIRS ${CATKIN_DEVEL_PREFIX}/include)
