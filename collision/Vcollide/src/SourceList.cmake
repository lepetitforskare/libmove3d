SET(BM3D_MODULE_NAME_TMP2 ${BM3D_MODULE_NAME})
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME}/src)
include_directories (${BM3D_MODULE_NAME})
BM3D_SRC_SUBDIR_PROCESS(NBody.C PairData.C VCollide.C VInternal.C)
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME_TMP2})
