SET(BM3D_MODULE_NAME_TMP ${BM3D_MODULE_NAME})
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME}/API)
BM3D_INC_DIR_PROCESS (${BM3D_MODULE_NAME})
BM3D_SRC_SUBDIR_PROCESS(project.cpp scene.cpp)
include(${CMAKE_SOURCE_DIR}/${BM3D_MODULE_NAME}/Grids/SourceList.cmake)
include(${CMAKE_SOURCE_DIR}/${BM3D_MODULE_NAME}/ConfigSpace/SourceList.cmake)
include(${CMAKE_SOURCE_DIR}/${BM3D_MODULE_NAME}/Device/SourceList.cmake)
include(${CMAKE_SOURCE_DIR}/${BM3D_MODULE_NAME}/Roadmap/SourceList.cmake)
#include(${CMAKE_SOURCE_DIR}/${BM3D_MODULE_NAME}/Roadmap2/SourceList.cmake)
include(${CMAKE_SOURCE_DIR}/${BM3D_MODULE_NAME}/Search/SourceList.cmake)
include(${CMAKE_SOURCE_DIR}/${BM3D_MODULE_NAME}/Trajectory/SourceList.cmake)
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME_TMP})
