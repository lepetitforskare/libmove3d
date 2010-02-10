SET(BM3D_MODULE_NAME_TMP2 ${BM3D_MODULE_NAME})
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME}/Grids)
BM3D_INC_DIR_PROCESS (${BM3D_MODULE_NAME})
BM3D_SRC_SUBDIR_PROCESS(
ThreeDCell.cpp
ThreeDGrid.cpp 
ThreeDPoints.cpp
TwoDCell.cpp
TwoDGrid.cpp
BaseGrid.cpp
BaseCell.cpp
)
include(${CMAKE_SOURCE_DIR}/${BM3D_MODULE_NAME}/GridToGraph/SourceList.cmake)
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME_TMP2})
