/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.
 *
 *   Created: Wed Jun 23 14:30:03 2004
 */
#ifndef __CEXTRACT__

extern int p3d_write_graph ( p3d_graph *G, const char *file );
extern int p3d_write_graph_buff ( p3d_graph *G, void** buffer, size_t *bsize );
extern int p3d_read_graph ( char *file );
extern int p3d_read_graph_buff ( void* buffer, double *version );
extern int p3d_read_this_graph ( FILE *fd, p3d_graph **pG, p3d_env * envPt, p3d_rob * robotPt );
extern void p3d_print_node(p3d_graph *G, p3d_node *N);

#endif /* __CEXTRACT__ */
