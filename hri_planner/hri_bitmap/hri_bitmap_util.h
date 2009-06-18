#ifndef HRI_BTMAP_UTIL
#define HRI_BTMAP_UTIL


#include "Util-pkg.h"
#include "P3d-pkg.h"
#include "Graphic-pkg.h"
#include "Hri_planner-pkg.h"
#include "math.h"

bool on_map(int x, int y, int z, hri_bitmap* bitmap);

int get_direction(hri_bitmap_cell *satellite_cell, hri_bitmap_cell *center_cell);

#endif

