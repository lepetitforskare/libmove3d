/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern int tri_get_a_triangle_vertex ( int triangle_nr, int vertex_nr );
extern int tri_angulate_facet ( p3d_poly *m3d_polyh, int fnum, int nr_triangles, tricd_triangulation *tricd_decomp_p );
extern int tri_angulate_facet1 ( p3d_poly *m3d_polyh, int fnum, int nr_triangles, tricd_triangulation *tricd_decomp_p, output_triangle_p *triangle_id_list_p );
extern int tri_angulate_facet2 ( p3d_poly *m3d_polyh, int fnum, int nr_triangles, tricd_triangulation *tricd_decomp_p, output_triangle_p *triangle_id_list_p, int fill_triangle_id_list );
extern int tri_angulate_simple_polygon ( p3d_poly *m3d_polyh, int facet_nr, int *nr_triangles );

#endif /* __CEXTRACT__ */
