/*
 * Copyright (c) 2001-2014 LAAS/CNRS
 * All rights reserved.
 *
 * Redistribution  and  use  in  source  and binary  forms,  with  or  without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice and this list of conditions.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice and  this list of  conditions in the  documentation and/or
 *      other materials provided with the distribution.
 *
 * THE SOFTWARE  IS PROVIDED "AS IS"  AND THE AUTHOR  DISCLAIMS ALL WARRANTIES
 * WITH  REGARD   TO  THIS  SOFTWARE  INCLUDING  ALL   IMPLIED  WARRANTIES  OF
 * MERCHANTABILITY AND  FITNESS.  IN NO EVENT  SHALL THE AUTHOR  BE LIABLE FOR
 * ANY  SPECIAL, DIRECT,  INDIRECT, OR  CONSEQUENTIAL DAMAGES  OR  ANY DAMAGES
 * WHATSOEVER  RESULTING FROM  LOSS OF  USE, DATA  OR PROFITS,  WHETHER  IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR  OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.                                  
 *
 * Siméon, T., Laumond, J. P., & Lamiraux, F. (2001). 
 * Move3d: A generic platform for path planning. In in 4th Int. Symp.
 * on Assembly and Task Planning.
 */
/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.
 *
 *   Created: Wed Jun 23 14:30:18 2004
 */
#ifndef __CEXTRACT__

extern void colisiones_aisladas ( int iniA, int nA, int iniB, int nB, char *lA, char *lB );
extern void write_SD_function ( SDpt SD );
extern void write_SD_functions ( void );
extern void general_SD_function ( SDpt SD );
extern void Obb_sc_SD_function ( SDpt SD );
extern void new_inter_bb_SD ( void );
extern void new_Obb_sc_SD ( void );
extern void new_autocol_bb_or_sc_SD ( Rigid_structure *r, p3d_poly **list, SD_function function );
extern void new_sc_bb_SD ( void );
extern void get_init_variables ( void );
extern void create_residual_SDs ( int *autocolindex, int *intercolindex, int *loopindex );

#endif /* __CEXTRACT__ */
