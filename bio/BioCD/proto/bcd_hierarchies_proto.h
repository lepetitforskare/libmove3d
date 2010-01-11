/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.
 *
 *   Created: Wed Jun 23 14:30:19 2004
 */
#ifndef __CEXTRACT__

extern int valid_pair ( p3d_poly *poly1, p3d_poly *poly2 );
extern short int donatouch_basic ( double *min1, double *min2, double *max1, double *max2 );
extern void bio_set_surface_d ( double distance );
extern short int enough_Surf_Dist ( double *min1, double *min2, double *max1, double *max2 );
extern void set_minimum_Sdistance_to_max ( void );
extern short int enough_minS_Dist ( double *min1, double *min2, double *max1, double *max2 );
extern void bio_set_col_mode ( int mode );
extern int  bio_get_col_mode(void);
extern void box_coll_withinProt ( Bboxpt b1, Bboxpt b2 );
extern void box_coll_general ( Bboxpt b1, Bboxpt b2 );
extern void imprimir ( boxnodept node );
extern boxnodept create_rigid_root ( Rigid_structure *rigido );
extern void freeboxhierarchy2 ( Rigid_structure *r );
extern void bio_create_molecule_root ( Robot_structure *robot );
extern void sup_split ( supnodept node );
extern void sup_hierarchy1 ( supnodept node );
extern void sup_hierarchy ( supnodept node );
extern void leaf_sup_collision ( supnodept node1, supnodept node2 );
extern void sup_collision ( supnodept node1, supnodept node2 );
extern void freesuphierarchy ( Robot_structure *robot );
extern void my_robot_autocollision ( Robot_structure *robot );
extern double min_dist_report ( p3d_poly **poly1, p3d_poly **poly2 );
extern void biocol_report ( int *col_number, p3d_poly ***list1, p3d_poly ***list2 );
extern void biocol_report_dist(int *atom_number, p3d_poly ***list1, p3d_poly ***list2, double **dist);
extern void set_required_collisions ( int rc );
extern void set_required_collisions_to_max ( void );
extern void set_n_collisions_to_zero ( void );
extern int get_n_collisions ( void );
extern int too_much_collisions ( void );

#endif /* __CEXTRACT__ */
