SET(BM3D_MODULE_NAME_TMP ${BM3D_MODULE_NAME})
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME}/qtFormRobot)
BM3D_SRC_SUBDIR_PROCESS(moverobot.cpp)
include_directories (${BM3D_MODULE_NAME})
BM3D_QT_GENERATE_MOC(moverobot.hpp)
BM3D_QT_GENERATE_UI_HEADERS(moverobot.ui)
SET(BM3D_MODULE_NAME ${BM3D_MODULE_NAME_TMP})