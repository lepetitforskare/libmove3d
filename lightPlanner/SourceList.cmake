IF(LIGHT_PLANNER)
SET(BM3D_MODULE_NAME lightPlanner)
BM3D_SRC_SUBDIR_PROCESS(
lightPlannerApi.c lightPlanner.c robotPos.c DlrObject.cpp DlrParser.cpp DlrPlan.cpp DlrPlanner.cpp)
include_directories (${BM3D_MODULE_NAME}/proto)
ENDIF(LIGHT_PLANNER)
