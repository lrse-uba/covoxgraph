cmake_minimum_required(VERSION 2.8.0)

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

project(cblox)
find_package(CUDA REQUIRED)
add_definitions(-std=c++17)

find_package(catkin_simple REQUIRED)
catkin_simple(ALL_DEPS_REQUIRED)

# Mark these as system deps to supress warnings.
include_directories(SYSTEM ${catkin_INCLUDE_DIRS})

############
# PROTOBUF #
############
# Taken from voxblox
set(PROTO_DEFNS proto/cblox/QuatTransformation.proto
                proto/cblox/Submap.proto
                proto/cblox/SubmapCollection.proto)
set(BASE_PATH "proto")
set(PROTOBUF_COMPILE_PYTHON true)

PROTOBUF_CATKIN_GENERATE_CPP2(${BASE_PATH} PROTO_SRCS PROTO_HDRS ${PROTO_DEFNS})

set(LIB_TYPE STATIC)
cs_cuda_add_library(CUDASampLib2 ${LIB_TYPE} include/cblox/cuda/dummy.cu )

#############
# LIBRARIES #
#############
cs_add_library(${PROJECT_NAME}_proto
  ${PROTO_SRCS}
)

cs_add_library(${PROJECT_NAME}_lib
  src/core/tsdf_submap.cpp
  src/core/tsdf_esdf_submap.cpp
  src/integrator/tsdf_submap_collection_integrator.cpp
  src/utils/quat_transformation_protobuf_utils.cpp
  src/mesh/submap_mesher.cpp
  src/io/transformation_io.cpp
)

target_link_libraries(${PROJECT_NAME}_lib ${PROJECT_NAME}_proto)

add_dependencies(${PROJECT_NAME}_lib CUDASampLib2)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}_lib
${catkin_LIBRARIES}
CUDASampLib2)

##########
# EXPORT #
##########
cs_install()
cs_export(INCLUDE_DIRS ${CATKIN_DEVEL_PREFIX}/include)
