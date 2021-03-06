SET(BM3D_MODULE_NAME_TMP ${BM3D_MODULE_NAME})
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME}/BioCD)
BM3D_INC_DIR_PROCESS (${BM3D_MODULE_NAME}/proto)
BM3D_INC_DIR_PROCESS (${BM3D_MODULE_NAME}/include)
BM3D_SRC_SUBDIR_PROCESS(bcd_hierarchies.c bcd_init.c bcd_resize.c bcd_shordist.c bcd_test.c bcd_util.c)
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME_TMP})
