SET(BM3D_MODULE_NAME_TMP ${BM3D_MODULE_NAME})
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME}/BioStructures)
include_directories (${BM3D_MODULE_NAME}/proto)
include_directories (${BM3D_MODULE_NAME}/include)
BM3D_SRC_SUBDIR_PROCESS(aaa_rw.c biostruct_util.c FORM_aaa.c FORM_files_selector.c FORM_joint_lig.c psf_defs_bio.c psf_get_set.c psf_rw.c psf_to_p3d.c psf_util.c)
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME_TMP})
