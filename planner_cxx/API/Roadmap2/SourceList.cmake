SET(BM3D_MODULE_NAME_TMP2 ${BM3D_MODULE_NAME})
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME}/Roadmap2)
BM3D_INC_DIR_PROCESS (${BM3D_MODULE_NAME})
BM3D_SRC_SUBDIR_PROCESS(
graph2.cpp
)
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME_TMP2})
