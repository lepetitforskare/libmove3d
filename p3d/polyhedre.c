/******************************************************************************************

  FICHIER: polyhedre.c

  USAGE: module gerant des objets a facettes, creation par sommet
         puis definition des faces
         le calcul des plans des faces est effectuee et reste memorisee uniquement
         a la demande. De meme pour les arretes

******************************************************************************************/


#include "Util-pkg.h"
#include "P3d-pkg.h"
//#include "Planner-pkg.h"
//#include "Localpath-pkg.h"
#include "Collision-pkg.h"


#define POLY_REALLOC(ptr,type,n) MY_REALLOC(ptr,type,n-1,n)

//const int NB_VERTICES = 200;

unsigned int poly_error_value;

static int poly_error_on_shell=TRUE;


/*******************************************************************************************
   convertion des 3 coordonnees float en type vecteur3
*******************************************************************************************/

void poly_f_2_v3(float x, float y, float z, poly_vector3 *vector)
{ (*vector)[0]=(poly_type_de_base)x;
  (*vector)[1]=(poly_type_de_base)y;
  (*vector)[2]=(poly_type_de_base)z;
}


/*******************************************************************************************
 convertion des 3 coordonnees double en type vecteur3
********************************************************************************************/

void poly_d_2_v3(double x, double y, double z, poly_vector3 *vector)
{ (*vector)[0]=(poly_type_de_base)x;
  (*vector)[1]=(poly_type_de_base)y;
  (*vector)[2]=(poly_type_de_base)z;
}


/*******************************************************************************************
   convertion un vector3 en 3 coordonnees float
*******************************************************************************************/

void poly_v3_2_f(poly_vector3 *vector, float *x, float *y, float *z)
{ *x=(float)(*vector)[0];
  *y=(float)(*vector)[1];
  *z=(float)(*vector)[2];
}


/*******************************************************************************************
 convertion un vector3 en  3 coordonnees double
********************************************************************************************/

void poly_v3_2_d(poly_vector3 *vector, double *x, double *y, double *z)
{ *x=(double)(*vector)[0];
  *y=(double)(*vector)[1];
  *z=(double)(*vector)[2];
}


/*******************************************************************************************
 convertion un plan en  4 coordonnees double
********************************************************************************************/

void poly_plane_2_d(poly_plane *plane, double *a, double *b, double *c, double *d)
{ *a=(double)plane->normale[0];
  *b=(double)plane->normale[1];
  *c=(double)plane->normale[2];
  *d=(double)plane->d;
}


/*******************************************************************************************
 active ou desactive l ecriture des erreurs directement sur le shell
   IN: value=TRUE les erreurs sont ecrites
             FALSE les erreurs ne le sont pasmais sont accessible par poly_get_error_value
 ******************************************************************************************/

int set_poly_show_error_on_shell(int value)
{
  poly_error_on_shell=value;
  return value;
}



/*******************************************************************************************
 retourne la valeur de la derniere erreur et remet la valeur de l erreur a 0
********************************************************************************************/

int poly_get_error_value()
{ int error;
  error=poly_error_value;
  poly_error_value=0;
  return error;
}


/*******************************************************************************************
 retourne true si la derniere operation effectuee a generee une erreur
    et false sinon
*******************************************************************************************/

int poly_error()
{ return (poly_error_value>0);
}


/*******************************************************************************************
 initialise un polyhedre, a faire avant de faire quoi que se soit ce dernier
****************************************************************************************** */

void poly_init_poly(poly_polyhedre *polyhedre, char *name)
{ int i,j;
  if (polyhedre==NULL)
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur je ne peux initialiser un pointeur NULL dans polyhedre.c:init_polyhedre\n"));
      poly_error_value=poly_error_null_pointer;
    }
  else
    { polyhedre->name=MY_ALLOC(char,strlen(name)+1);
      if (polyhedre->name==NULL)
        { if (poly_error_on_shell)
            PrintInfo(("\nErreur d allocation memoire dans polyhedre.c:init_polyhedre\n"));
            poly_error_value=poly_error_malloc_failled;
        }
      poly_error_value=poly_error_null_pointer;
      polyhedre->nb_points=0;
      polyhedre->nb_faces=0;
      polyhedre->nb_edges=0;
      strcpy(polyhedre->name,name);
      polyhedre->the_points=NULL;
      polyhedre->the_faces=NULL;
      polyhedre->the_edges=NULL;
      poly_error_value=0;
      for(i=0;i<4;i++)
        for(j=0;j<4;j++)
          if (i!=j) polyhedre->pos[i][j]=0.0;
          else polyhedre->pos[i][j]=1.0;
    }
}


/*******************************************************************************************
 cree et initialise une variable polyhdere et retourne son pointeur
*******************************************************************************************/

poly_polyhedre *poly_create_poly(char *name)
{ poly_polyhedre *polyhedre;

  polyhedre=MY_ALLOC(poly_polyhedre,1);
  if (polyhedre==NULL)
    {  if (poly_error_on_shell)
         PrintInfo(("\nErreur d allocation memoire dans polyhedre.c:poly_make_polyhedre\n"));
      poly_error_value=poly_error_malloc_failled;
    }
  else
    poly_init_poly(polyhedre,name);
  return polyhedre;
}


/*******************************************************************************************
 detruit un polyhedre
*******************************************************************************************/

void poly_destroy_poly(poly_polyhedre *polyhedre)
{ unsigned int i;
  MY_FREE(polyhedre->name,char,(strlen(polyhedre->name)+1));
  MY_FREE(polyhedre->the_points,poly_vector3,polyhedre->nb_points);
  poly_destroy_edges(polyhedre);
  poly_destroy_planes(polyhedre);
  for(i=0;i<polyhedre->nb_faces;i++)
    { MY_FREE((polyhedre->the_faces[i].the_indexs_points),poly_index,(polyhedre->the_faces[i].nb_points));
    }
  MY_FREE(polyhedre->the_faces,poly_face,polyhedre->nb_faces);
  MY_FREE(polyhedre,poly_polyhedre,1);
}


/*******************************************************************************************
 detruit les arretes calculees et remet le nombre d arretes calculees a 0
*******************************************************************************************/

void poly_destroy_edges(poly_polyhedre *polyhedre)
{ if (polyhedre->the_edges!=NULL) MY_FREE(polyhedre->the_edges,poly_edge,polyhedre->nb_edges);
}


/*******************************************************************************************
 detruit les plans calculees
*******************************************************************************************/

void poly_destroy_planes(poly_polyhedre *polyhedre)
{ unsigned int i;

  for(i=0;i<polyhedre->nb_faces;i++)
    if (polyhedre->the_faces[i].plane!=NULL) MY_FREE(polyhedre->the_faces[i].plane,poly_plane,1);
}


/*******************************************************************************************
retourne le nom du polyhedre
*******************************************************************************************/

char *poly_get_name(poly_polyhedre *polyhedre)
{ poly_error_value=0;
  if (polyhedre!=NULL) return polyhedre->name;
  poly_error_value=poly_error_null_pointer;
      if (poly_error_on_shell)
        PrintInfo(("\nErreur pointeur null dans polyhedre.c:poly_get_name\n"));
      return 0;

}


/*******************************************************************************************
retourne le nombre de points definissant le polyhedre
*******************************************************************************************/

unsigned int poly_get_nb_points(poly_polyhedre *polyhedre)
{ poly_error_value=0;
  return polyhedre->nb_points;
}


/*******************************************************************************************
retourne le nombre de points definissant la face du polyhedre
 et 0 s il y erreur
*******************************************************************************************/

unsigned int poly_get_nb_points_in_face(poly_polyhedre *polyhedre, poly_index face)
{ poly_error_value=0;
  if ((face<1) || (face>polyhedre->nb_faces))
    { poly_error_value= poly_error_impossible_index;
      if (poly_error_on_shell)
        PrintInfo(("\nErreur index non valide dans polyhedre.c:poly_get_nb_points_in_face\n"));
      return 0;
    }
  return polyhedre->the_faces[face-1].nb_points;
}


/* ************************************** */
/* Carl (16 oct. 2000):    new function   */
/*    In:  polyhedron  p,                 */
/*         facet identifying number  f_id */
/*    Out:  pointer to list of vertices   */
/*         of facet  f_id in polyhedron p */
/* ************************************** */

poly_index_p p3d_get_points_in_face(poly_polyhedre *polyhedre, poly_index face)
{
 poly_error_value=0;
  if ((face<1) || (face>polyhedre->nb_faces))
    { poly_error_value= poly_error_impossible_index;
      if (poly_error_on_shell)
        PrintInfo(("\nErreur index non valide dans polyhedre.c:poly_get_points_in_face\n"));
      return NULL;
    }
  return polyhedre->the_faces[face-1].the_indexs_points;
}

/*******************************************************************************************
 retourne le nombre de faces definissant le polyhedre
*******************************************************************************************/

unsigned int poly_get_nb_faces(poly_polyhedre *polyhedre)
{ poly_error_value=0;
  return polyhedre->nb_faces;
}


/*******************************************************************************************
 retourne le nombre d arretes definissant la face du polyhedre
 et 0 s il y erreur
*******************************************************************************************/

unsigned int poly_get_nb_edges_in_face(poly_polyhedre *polyhedre, poly_index face)
{ return poly_get_nb_points_in_face(polyhedre,face);
}


/*******************************************************************************************
 retourne le nombre d arretes definissant le polyhedre
  et 0 s il y erreur
*******************************************************************************************/

unsigned int poly_get_nb_edges(poly_polyhedre *polyhedre)
{ unsigned int n,i;

  poly_error_value=0;
  n=0;

  for(i=0;i<polyhedre->nb_faces;i++)
    n=n+polyhedre->the_faces[i].nb_points;
  if ((n%2)==0)
    { polyhedre->nb_edges=n/2;
      return (n/2);
    }
  else
    { poly_error_value= poly_error_impossible_polyhedre;
      if (poly_error_on_shell)
        PrintInfo(("\nErreur polyhedre non valide dans polyhedre.c:poly_get_nb_edge1\n"));
      return 0;
    }
}


/*******************************************************************************************
 retourne le numero du point place a la n ieme position dans la face
 le numero est nul s il y erreur
*******************************************************************************************/

int poly_get_index_point_in_face(poly_polyhedre *polyhedre, poly_index face, poly_index index)
{ poly_error_value=0;
  if ((face<1) || (face>polyhedre->nb_faces))
    { poly_error_value= poly_error_impossible_index;
      if (poly_error_on_shell)
        PrintInfo(("\nErreur index de face non valide dans polyhedre.c:poly_get_index_point_in_face\n"));
      return 0;
    }
  if ((index<1) || (index>polyhedre->the_faces[face-1].nb_points))
    { poly_error_value= poly_error_impossible_index;
      if (poly_error_on_shell)
        PrintInfo(("\nErreur index de position non valide dans polyhedre.c:poly_get_index_point_in_face\n"));
      return 0;
    }
  return polyhedre->the_faces[face-1].the_indexs_points[index-1];
}


/*******************************************************************************************
 retourne le numero des points constituant l arrete numero n
 OUT: TRUE si pas de probleme
      FALSE sinon
*******************************************************************************************/

int poly_get_edge_points(poly_polyhedre *polyhedre, poly_index edge, poly_index *p1, poly_index *p2)

{ int ok;

  poly_error_value=0;
  if (polyhedre->the_edges==NULL)
    { ok=poly_build_edges(polyhedre);
      if(!ok) return FALSE;
    }
  if ((edge < 0) || (edge>polyhedre->nb_edges))
    { poly_error_value= poly_error_impossible_index;
      if (poly_error_on_shell)
        PrintInfo(("\nErreur index non valide dans polyhedre.c:poly_get_edge_points\n"));
      return FALSE;
    }
 *p1=polyhedre->the_edges[edge].point1;
 *p2=polyhedre->the_edges[edge].point2;
 return TRUE;
}


/*******************************************************************************************
 retourne le numero des faces constituant l arrete numero n
 OUT: TRUE si pas de probleme
      FALSE sinon
*******************************************************************************************/

int poly_get_edge_faces(poly_polyhedre *polyhedre, poly_index edge, poly_index *f1, poly_index *f2)

{ int ok;

  poly_error_value=0;
  if (polyhedre->the_edges==NULL)
    { ok=poly_build_edges(polyhedre);
      if(!ok) return FALSE;
    }
  if ((edge<1) || (edge>polyhedre->nb_edges))
    { poly_error_value= poly_error_impossible_index;
      if (poly_error_on_shell)
        PrintInfo(("\nErreur index non valide dans polyhedre.c:poly_get_edge_points\n"));
      return FALSE;
    }
 *f1=polyhedre->the_edges[edge-1].face1;
 *f2=polyhedre->the_edges[edge-1].face2;
 return TRUE;
}



/*******************************************************************************************
 ajoute un point au polyhedre
   OUT: TRUE si pas de probleme
        FALSE sinon
*******************************************************************************************/

int poly_add_point(poly_vector3 point, poly_polyhedre *polyhedre)
{ poly_vector3 *the_points;

  int i;

  poly_error_value=0;
  the_points=polyhedre->the_points;
  /*  if (polyhedre->nb_points>NB_VERTICES) PrintInfo(("\nWarning poly %s more %i points:%d\n",polyhedre->name,NB_VERTICES,polyhedre->nb_points)); */
  the_points=POLY_REALLOC(the_points,poly_vector3,(polyhedre->nb_points+1));
  if (the_points==NULL)
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur d allocation memoire dans polyhedre.c: poly_add_point\n"));
      poly_error_value=poly_error_malloc_failled;
      return FALSE;
    }
  else
    { polyhedre->the_points=the_points;
      for(i=0;i<3;i++)
        the_points[polyhedre->nb_points][i]=point[i];
      polyhedre->nb_points++;
      return TRUE;
    }
}


/*******************************************************************************************
 trouve un point dans le polyhedre
   OUT: le numero du point(entre 1 et nb_points) si le point existe
        0 si le point n existe pas
*******************************************************************************************/

int poly_find_point(poly_vector3 point, poly_polyhedre *polyhedre)
{ unsigned int i,j;
  int trouve;
  poly_vector3  *the_points;

  i=0;
  the_points=polyhedre->the_points;
  poly_error_value=0;
  do
    { trouve=TRUE;
      for(j=0;j<3;j++)
        trouve=trouve && (the_points[i][j]==point[j]);
      i++;
    } while((i<polyhedre->nb_points) && !(trouve));
  if (trouve)
    return i;
  else
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur, le point cherche n'existe pas pour polyhedre.c: poly_find_point \n"));
      poly_error_value=poly_error_unknown_point;
      return  0;
    }
}


/*******************************************************************************************
 retourne les coordonees d un point reference dans le polyhedre par
   sa position index(entre 1 et nb_points)
   OUT: TRUE si le point existe
        FALSE si le point n existe pas
*******************************************************************************************/

int poly_get_point_2_v3(poly_polyhedre *polyhedre, poly_index index, poly_vector3 *vector)
{ int i;

  poly_error_value=0;
  if ((index<=polyhedre->nb_points) && (index>0))
    { for(i=0;i<3;i++)
        (*vector)[i]=(polyhedre->the_points[index-1])[i];
      return TRUE;
    }
  else
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur, le point cherche n'existe pas pour polyhedre.c: poly_get_point \n"));
      poly_error_value=poly_error_unknown_point;
      return FALSE;
    }
}

/*******************************************************************************************
 retourne les coordonees d un point reference dans le polyhedre par
   sa position index(entre 1 et nb_points)
   OUT: TRUE si le point existe
        FALSE si le point n existe pas
*******************************************************************************************/

int poly_get_point_2_d(poly_polyhedre *polyhedre, poly_index index, double *x,double *y,double *z)
{ poly_error_value=0;

  if ((index<=polyhedre->nb_points) && (index>0))
    { *x=(double)(polyhedre->the_points[index-1])[0];
      *y=(double)(polyhedre->the_points[index-1])[1];
      *z=(double)(polyhedre->the_points[index-1])[2];
      return TRUE;
    }
  else
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur, le point cherche n'existe pas pour polyhedre.c: poly_get_point \n"));
      poly_error_value=poly_error_unknown_point;
      return FALSE;
    }
}

/*******************************************************************************************
 mise a l'echelle du polyedre
   IN: polyedre
       scaleX, scaleY, scaleZ

*******************************************************************************************/

void poly_scale_poly(poly_polyhedre *polyhedre, double scaleX, double scaleY, double scaleZ)
{
  unsigned int i;
  poly_vector3  *the_points;


  the_points=polyhedre->the_points;
  for(i=0;i<polyhedre->nb_points;i++)
    {
      the_points[i][0]*=scaleX;
      the_points[i][1]*=scaleY;
      the_points[i][2]*=scaleZ;
    }
}

/*******************************************************************************************
positionnement du polyedre par matrice
   IN: polyedre
       sc

*******************************************************************************************/

void poly_pos_poly_by_mat(poly_polyhedre *polyhedre,poly_matrix4 mat)
{
  unsigned int i;
  poly_vector3  *the_points, point;
  double c;


  the_points=polyhedre->the_points;
  for(i=0;i<polyhedre->nb_points;i++)
    {
      point[0] = the_points[i][0] * mat[0][0] + the_points[i][1] * mat[0][1] + the_points[i][2] * mat[0][2] + mat[0][3];
      point[1] = the_points[i][0] * mat[1][0] + the_points[i][1] * mat[1][1] + the_points[i][2] * mat[1][2] + mat[1][3];
      point[2] = the_points[i][0] * mat[2][0] + the_points[i][1] * mat[2][1] + the_points[i][2] * mat[2][2] + mat[2][3];
      c = the_points[i][0] * mat[3][0] + the_points[i][1] * mat[3][1] + the_points[i][2] * mat[3][2] + mat[3][3];
      the_points[i][0] = point[0]/c;
      the_points[i][1] = point[1]/c;
      the_points[i][2] = point[2]/c;
    }
}

/*******************************************************************************************
 retourne la matrice de position du polyhedre
OUT: mat contient la matrice
*******************************************************************************************/

void poly_get_poly_pos(poly_polyhedre *polyhedre, poly_matrix4 mat)
{ int i,j;

  poly_error_value=0;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      mat[i][j]=polyhedre->pos[i][j];
}


/*******************************************************************************************
 retourne un pointeur sur la matrice de position du polyhedre
*******************************************************************************************/

poly_matrix4 *poly_get_poly_mat(poly_polyhedre *polyhedre)
{ return &(polyhedre->pos);
}


/*******************************************************************************************
 definit la matrice de position du polyhedre
*******************************************************************************************/
void poly_set_poly_pos(poly_polyhedre *polyhedre, poly_matrix4 mat)
{ int i,j;

  poly_error_value=0;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      polyhedre->pos[i][j]=mat[i][j];
}



/*******************************************************************************************
 OUT: la position du point dans la face du polyhedre s il en fait parti
        0 s il n est pas dedans
*******************************************************************************************/

int poly_find_pos_in_face(poly_polyhedre *polyhedre, poly_index index, poly_index point)
{ poly_face *face;
  unsigned int i,j;

  face=&polyhedre->the_faces[index-1];
  i=0;
  j=0;
  while ((i<face->nb_points) && (j==0))
    {if (face->the_indexs_points[i]==point) j=i+1;
     i++;
    }
  return j;
}


/*******************************************************************************************
 retourne les coordonnees du p ieme point de la f ieme  face
  OUT: TRUE si le point existe
       FALSE sinon
*******************************************************************************************/

int poly_get_point_in_pos_in_face(poly_polyhedre *polyhedre, poly_index face, poly_index point, double *x, double *y, double *z)
{ poly_index i;
  if ((face>polyhedre->nb_faces) || (face<1) || (point<1) || (point>polyhedre->nb_points))
    {
      if (poly_error_on_shell){
        PrintInfo(("\nErreur indexs ne pouvant correspondre a un point ou une face polyhedre.c: poly_get_point_in_pos_in_face\n"));}
      poly_error_value=poly_error_impossible_index;
      return FALSE;
  }
  i=polyhedre->the_faces[face-1].the_indexs_points[point-1]-1;
  *x=(double)(polyhedre->the_points[i])[0];
  *y=(double)(polyhedre->the_points[i])[1];
  *z=(double)(polyhedre->the_points[i])[2];
  return TRUE;
}

static int poly_set_face_is_convex_in_face(poly_polyhedre *p,int face,int result)
{
    if (((unsigned int)face>p->nb_faces) || (face<1))
    {
      if (poly_error_on_shell){
	PrintInfo(("\nErreur indexs ne pouvant correspondre a une face polyhedre.c: poly_set_face_is_convex_in_face\n"));}
      poly_error_value=poly_error_impossible_index;
      return FALSE;
    }
  p->the_faces[face-1].face_is_convex = (unsigned int)result;
  return TRUE;
}

static int poly_get_face_is_convex_in_face(poly_polyhedre *polyhedre, poly_index face, unsigned int *resp)
{
  if ((face>polyhedre->nb_faces) || (face<1))
    {
      if (poly_error_on_shell){
	PrintInfo(("\nErreur indexs ne pouvant correspondre a une face polyhedre.c: poly_get_face_is_convex_in_face\n"));}
      poly_error_value=poly_error_impossible_index;
      return FALSE;
    }
  (*resp) = polyhedre->the_faces[face-1].face_is_convex;
  return TRUE;
}

/*******************************************************************************************
 retourne les coordonnees du p ieme point(1 ou 2) de la e ieme  arrete
  OUT: TRUE si le point existe
       FALSE sinon
*******************************************************************************************/

int poly_get_point_in_edge(poly_polyhedre *polyhedre, poly_index edge, poly_index point, double *x, double *y, double *z)
{ poly_index p1,p2;
  if (!poly_get_edge_points(polyhedre,edge,&p1,&p2)) return FALSE;
  if (point==2) p1=p2;
  if (!poly_get_point_2_d(polyhedre,p1,x,y,z))
    return FALSE;
  else
    return TRUE;
}


/*******************************************************************************************
 retourne le numero de l arrete contenant les points numerotes p1 p2
   cree les arretes du polyhedre si elles n existent pas
  OUT: le numero(entre 1 et nb_arretes) s il la trouve
       0 si l arrete n existe pas ou n a pas etee creee
*******************************************************************************************/

int poly_find_edge(poly_polyhedre *polyhedre,poly_index p1,poly_index p2)
{ unsigned int i;
  int ok;
  poly_index p;

  poly_error_value=0;
  if (polyhedre->the_edges==NULL)
   { ok=poly_build_edges(polyhedre);
     if (!ok) return 0;
   }
  if (p1>p2)
    { p=p1;
      p1=p2;
      p2=p;
    }
  if ((p1<1) || (p2>polyhedre->nb_points))
    { if (poly_error_on_shell){
        PrintInfo(("\nErreur indexs ne pouvant correspondre a des points dans polyhedre.c: poly_find_edge\n"));}
      poly_error_value=poly_error_impossible_index;
      return 0;
    }
  ok=FALSE;
  i=0;
  while((!ok) && (i<polyhedre->nb_edges))
    { ok=(polyhedre->the_edges[i].point1==p1) && (polyhedre->the_edges[i].point2==p2);
      i++;
    }

  /* l arrete trouvee est elle completement definie*/
  if (ok)
    return i;
  else
    return 0;
}

/*******************************************************************************************
 cree l arrete contenant les points numerotes p1 p2
  OUT: TRUE si pas d erreur
       FALSE sinon
*******************************************************************************************/

int poly_build_edge(poly_polyhedre *polyhedre,poly_index f,poly_index p1,poly_index p2)
{ unsigned int i;
  int j,ok;
  poly_index p;
  poly_edge *the_edges;
  double n;

  poly_error_value=0;
  p=0;
  if (p1>p2)
    { p=p1;
      p1=p2;
      p2=p;
    }
  the_edges=polyhedre->the_edges;

  if ((p1<1) || (p2>polyhedre->nb_points))
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur indexs ne pouvant correspondre a des points dans polyhedre.c: poly_find_edge\n"));
      poly_error_value=poly_error_impossible_index;
      return 0;
    }

  ok=FALSE;
  i=0;
  while((!ok) && (i<polyhedre->nb_edges))
    { ok=(polyhedre->the_edges[i].point1==p1) && (polyhedre->the_edges[i].point2==p2);
      i++;
    }

  if (!ok)  /* l arrete n existe pas on la cree */
    { the_edges=POLY_REALLOC(the_edges,poly_edge,(polyhedre->nb_edges+1));
      if (the_edges==NULL)
        { if (poly_error_on_shell)
            PrintInfo(("\nErreur d allocation dans polyhedre.c:poly_find_edge\n"));
          poly_error_value=poly_error_malloc_failled;
          return FALSE;
	}
      else
        { polyhedre->the_edges=the_edges;
	  if (p==0)
            { the_edges[polyhedre->nb_edges].face1=f;
              the_edges[polyhedre->nb_edges].face2=0;
            }
          else
            { the_edges[polyhedre->nb_edges].face2=f;
              the_edges[polyhedre->nb_edges].face1=0;
            }
          the_edges[polyhedre->nb_edges].point1=p1;
          the_edges[polyhedre->nb_edges].point2=p2;
          n=0;

          for(j=0;j<3;j++)
            { the_edges[polyhedre->nb_edges].u[j]=polyhedre->the_points[p2-1][j]-polyhedre->the_points[p1-1][j];
              n=n+the_edges[polyhedre->nb_edges].u[j]*the_edges[polyhedre->nb_edges].u[j];
            }
          n=sqrt(n);
          if (n==0.0)
            { if (poly_error_on_shell)
                PrintInfo(("\nErreur calcul d une arrete sur un meme point dans polyhedre.c:poly_find_edge\n"));
              poly_error_value=poly_error_edge_needs_2_points;
              return FALSE;
            }

          for(j=0;j<3;j++)
            the_edges[polyhedre->nb_edges].u[j]=the_edges[polyhedre->nb_edges].u[j]/n;
          polyhedre->nb_edges++;
          return TRUE;
        }
    }
  else /* elle existe deja et on la complete */
    { if (p==0)
        the_edges[i-1].face1=f;
      else
        the_edges[i-1].face2=f;
      return TRUE;
    }
}






/*******************************************************************************************
 cree toutes les arretes du polyhedre
   OUT : TRUE si pas de probleme
         FALSE sinon
*******************************************************************************************/

int poly_build_edges(poly_polyhedre *polyhedre)
{ poly_index f,p;
  int ok;
  poly_face *face;

  ok=TRUE;
  if (polyhedre->the_edges!=NULL) poly_destroy_edges(polyhedre);
  polyhedre->nb_edges=0;
  polyhedre->the_edges = NULL;

  for(f=1;f<=polyhedre->nb_faces;f++)
    { face=&polyhedre->the_faces[f-1];
      for(p=0;p<face->nb_points-1;p++)
	{
	  if(face->the_indexs_points[p] != face->the_indexs_points[p+1]) /* Carl: test added - 7 Nov. 2000 */
	    ok=ok && (poly_build_edge(polyhedre,f,face->the_indexs_points[p],face->the_indexs_points[p+1])>0);
	}
      p = 0;
      while((face->the_indexs_points[face->nb_points-1] == face->the_indexs_points[p])&&(p<face->nb_points))
	p++; /* Carl: test added - 7 Nov. 2000 */
      if(p<face->nb_points) /* Carl: test added - 7 Nov. 2000: was connection with 0 instead of with p */
	ok=ok && (poly_build_edge(polyhedre,f,face->the_indexs_points[face->nb_points-1],face->the_indexs_points[p])>0);
    }
  return ok;
}


/*******************************************************************************************
 definie le plan de la suface par sa normale unitaire et le coefficient d definit par
   ax+by+cz+d=0 avec [a;b;c] normale unitaire exterieur si la face a la bonne orientation
   OUT: TRUE si pas de probleme
        FALSE sinon
*******************************************************************************************/

int poly_build_plane_face(poly_polyhedre *polyhedre,poly_index numero)
{ poly_face *face;
  poly_vector3 u1,u2,p;
  double n;
  unsigned int k;
  int i;

  poly_error_value=0;
  face=&polyhedre->the_faces[numero-1];
  if (face->plane==NULL)
    { face->plane=MY_ALLOC(poly_plane,1);
      if (face->plane==NULL)
        { if (poly_error_on_shell)
            PrintInfo(("\nErreur d allocation memoire dans type-poly.c: poly_build_plane_face\n"));
          poly_error_value=poly_error_malloc_failled;
          return FALSE;
	}
    }

  k = 0;
  n = 0.0;
  while((n==0.0)&&(k<face->nb_points-2))
    {
      /* on determine les deux premieres arretes de la face */
      i=poly_get_point_2_v3(polyhedre,face->the_indexs_points[k],&u1);
      i=poly_get_point_2_v3(polyhedre,face->the_indexs_points[k+1],&p);
      i=poly_get_point_2_v3(polyhedre,face->the_indexs_points[k+2],&u2);

      for(i=0;i<3;i++)
	{
	  u1[i]=p[i]-u1[i];
	  u2[i]=u2[i]-p[i];
	}


      /* on calcul la normale a partir du produit vectoriel des vecteurs unitaires des arretes */
      face->plane->normale[0]=u1[1]*u2[2]-u1[2]*u2[1];
      face->plane->normale[1]=u1[2]*u2[0]-u1[0]*u2[2];
      face->plane->normale[2]=u1[0]*u2[1]-u1[1]*u2[0];

      n=0.0;
      for(i=0;i<3;i++)
	n=n+face->plane->normale[i]*face->plane->normale[i];
      n=sqrt(n);
      k++; /* on essaie tous les triples de sommets avant de conclure que la face est anormale */
    }
  if (n==0.0)
    { if (poly_error_on_shell)
        PrintInfo(("\n%s:Erreur normale nulle dans polyhedre.c: poly_build_plane_face\n",
	       polyhedre->name));
      poly_error_value=poly_error_normal_nulle;
      return FALSE;
    }

  for(i=0;i<3;i++)
    face->plane->normale[i]=face->plane->normale[i]/n;

  /* le coefficient d est le produit scalaire de la normale par un point du plan ici le premier*/
  face->plane->d=-face->plane->normale[0]*p[0]-face->plane->normale[1]*p[1]-face->plane->normale[2]*p[2];
  return TRUE;
}


/*******************************************************************************************
 retourne le plan de la face du polyhedre
   si le plan n existe pas, la fonction le cree
   OUT: plan prend la valeur du plan de la face
        et int prend TRUE si pas de probleme
                     FALSE sinon
*******************************************************************************************/

int poly_get_plane(poly_polyhedre *polyhedre, poly_index index, poly_plane *plane)
{ int ok;

  ok=TRUE;
  poly_error_value=0;
  if ((index>polyhedre->nb_faces) || (index<1))
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur index faux dans polyhedre.c:poly_get_plane\n"));
      poly_error_value=poly_error_impossible_index;
      return FALSE;
    }
  if (polyhedre->the_faces[index-1].plane==NULL)
     ok=poly_build_plane_face(polyhedre,index);
  if (!ok)
    return FALSE;
  else
    { *plane=*(polyhedre->the_faces[index-1].plane);
      return TRUE;
    }

}

/*******************************************************************************************
 retourne le plan de la face du polyhedre
   si le plan n existe pas, la fonction le cree
   OUT: plan prend la valeur du plan de la face
        et int prend TRUE si pas de probleme
                     FALSE sinon
*******************************************************************************************/

int poly_get_plane_2_d(poly_polyhedre *polyhedre, poly_index index, double *a,double *b,double *c, double *d)
{ int ok;

  ok=TRUE;
  poly_error_value=0;
  if ((index>polyhedre->nb_faces) || (index<1))
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur index faux dans polyhedre.c:poly_get_plane\n"));
      poly_error_value=poly_error_impossible_index;
      return FALSE;
    }
  if (polyhedre->the_faces[index-1].plane==NULL)
     ok=poly_build_plane_face(polyhedre,index);
  if (!ok)
    return FALSE;
  else
    { *a=polyhedre->the_faces[index-1].plane->normale[0];
      *b=polyhedre->the_faces[index-1].plane->normale[1];
      *c=polyhedre->the_faces[index-1].plane->normale[2];
      *d=polyhedre->the_faces[index-1].plane->d;
      return TRUE;
    }

}



/*******************************************************************************************
 definie les plans des sufaces par sa normale unitaire et le coefficient d definit par
   ax+by+cz+d=0 avec [a;b;c] normale unitaire exterieur si la face a la bonne orientation
   OUT: TRUE si pas de probleme
        FALSE sinon
*******************************************************************************************/

int poly_build_planes(poly_polyhedre *polyhedre)
{ unsigned int i;
  int ok;

  ok=TRUE;
  for(i=1;i<=polyhedre->nb_faces;i++)
    ok=(ok && poly_build_plane_face(polyhedre,i));
  return ok;
}


/*******************************************************************************************
 creer un face sur un polyhedre
   on doit lui fournir les numeros des points(entre 1 et nb_points)
   et le nombre de point dans la facette
   cette fonction initialise egalement les arretes
   OUT: TRUE si la creation de la facette a reussie
        FALSE sinon
*******************************************************************************************/

int poly_build_face(poly_index *the_indexs ,unsigned int nombre, poly_polyhedre *polyhedre)
{ poly_face *the_faces;
  poly_index *the_indexs_points;
  unsigned int i;
  int ok;

   poly_error_value=0;
  /* verification de la validite de la liste envoyee */
  if ((nombre<3) || (nombre>polyhedre->nb_points))
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur, il faut trois points pour definir une face dans polyhedre.c: poly_build_face\n"));
      poly_error_value=poly_error_plane_needs_3_points;
      return FALSE;
    }
  ok=TRUE;
  for(i=0;i<nombre;i++)
    ok=ok && (the_indexs[i]<=polyhedre->nb_points);
  if (!ok)
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur, la liste des sommets est non conforme pour polyhedre.c: poly_build_face\n"));
      poly_error_value=poly_error_impossible_list_edges;
      return FALSE;
    }
  the_faces=polyhedre->the_faces;

  /* reallocation de la memoire et teste si la creation a reussie*/
  the_faces=POLY_REALLOC(the_faces,poly_face,(polyhedre->nb_faces+1));
  if (the_faces==NULL)
    { if (poly_error_on_shell)
        PrintInfo(("\nErreur d allocation memoire pour les faces dans polyhedre.c: poly_build_face\n"));
      poly_error_value=poly_error_malloc_failled;
      return FALSE;
    }
  else
    { polyhedre->the_faces=the_faces;
      the_indexs_points=MY_ALLOC(poly_index,nombre);
      if (the_indexs_points==NULL)
        { if (poly_error_on_shell)
            PrintInfo(("\nErreur d allocation memoire pour les indexs dans polyhedre.c: poly_build_face\n"));
          poly_error_value=poly_error_malloc_failled;
          return FALSE;
	}

      the_faces[polyhedre->nb_faces].face_is_convex=UNKNOWN;
      the_faces[polyhedre->nb_faces].the_indexs_points=the_indexs_points;
      the_faces[polyhedre->nb_faces].nb_points=nombre;
      the_faces[polyhedre->nb_faces].plane=NULL;
      for(i=0;i<nombre;i++)
        the_indexs_points[i]=the_indexs[i];
      polyhedre->nb_faces++;
      return TRUE;
    }
}


int p3d_poly_is_convex(poly_polyhedre *p)
{
  int result = TRUE;
  unsigned int i, nr_edges, index1, index2;
  poly_face *f1=NULL;
  poly_face *f2=NULL;
  poly_vector3 *norm1=NULL;
  poly_vector3 *norm2=NULL;
  poly_vector3 vec;
  double flag;

  if(!poly_build_edges(p))
    result = FALSE;
  if(!poly_build_planes(p))
    result = FALSE;
  nr_edges = p->nb_edges;
  for(i=0;(result)&&(i<nr_edges);i++)
    {
      f1=NULL;
      f2=NULL;
      index1=p->the_edges[i].face1-1 ;
      if((index1>=0)&&(index1<(p->nb_faces)))
	{
	  f1=&(p->the_faces[index1]);
	  norm1 = &(f1->plane->normale);
	  if ( EQ((*norm1)[0],0.0) && EQ((*norm1)[1],0.0) && EQ((*norm1)[2],0.0) )
	    f1=NULL ;
	}
      index2=p->the_edges[i].face2-1 ;
      if((index2>=0)&&(index2<(p->nb_faces)))
	{
	  f2=&(p->the_faces[index2]);
	  norm2 = &(f2->plane->normale);
	  if ( EQ((*norm2)[0],0.0) && EQ((*norm2)[1],0.0) && EQ((*norm2)[2],0.0) )
	    f2=NULL ;
	}
      if (f1 && f2)
	{
	  p3d_vectXprod(*norm1,*norm2,vec);
	  flag = p3d_vectDotProd(vec,p->the_edges[i].u);
	  if(flag < 0.0)
	    result = FALSE;
	}
      else
	{
	  result = FALSE;
	}
    }

  /* clean up planes and edges, since too expensive in memory */
  poly_destroy_edges(p);
  p->nb_edges=0;
  p->the_edges = NULL;
/*   poly_destroy_planes(p); */
  return result;
}

/* ************************************** */
/* Carl (10 oct. 2000):    new function   */
/*    In:  polyhedron  p,                 */
/*         facet identifying number  f_id */
/*    Out: TRUE if facet f_id is convex,  */
/*         FALSE otherwise                */
/* ************************************** */
int p3d_poly_facet_is_convex(poly_polyhedre *p, int f_id)
{
  int nr_edges = poly_get_nb_points_in_face(p,f_id);
  int nr_points;
  int i, result = TRUE;
  poly_vector3  point1;
  poly_vector3  point2;
  poly_vector3  point3;
  poly_vector3 edge1;
  poly_vector3 edge2;
  poly_vector3 norm1;
  poly_vector3 norm2;
  unsigned int stored_answer;

  poly_get_face_is_convex_in_face(p,f_id,&stored_answer);
  if(stored_answer == TRUE)
    return TRUE;
  if(stored_answer == FALSE)
    return FALSE;
  /* stored_answer == UNKNOWN, let's compute */
  nr_points = nr_edges;
/*   if(!poly_build_edges(p)) */
/*     result = FALSE; */
/*   else if(!poly_build_planes(p)) */
/*     result = FALSE; */
/*   else */
    {
      poly_get_point_in_pos_in_face(p,f_id,nr_edges-1,&(point1[0]),&(point1[1]),&(point1[2]));
      poly_get_point_in_pos_in_face(p,f_id,nr_edges,&(point2[0]),&(point2[1]),&(point2[2]));
      poly_get_point_in_pos_in_face(p,f_id,1,&(point3[0]),&(point3[1]),&(point3[2]));
      p3d_vectSub(point2,point1,edge1);
      p3d_vectSub(point3,point2,edge2);
      p3d_vectXprod(edge1,edge2,norm1);

      for(i=2;(result)&&(i<=nr_edges);i++)
	{
	  p3d_vectCopy(point3,point2);
	  p3d_vectCopy(edge2,edge1);
	  poly_get_point_in_pos_in_face(p,f_id,i,&(point3[0]),&(point3[1]),&(point3[2]));
	  p3d_vectSub(point3,point2,edge2);
	  p3d_vectXprod(edge1,edge2,norm2);
	  if(GEQ(norm1[0]*norm2[0],0.0)&&GEQ(norm1[1]*norm2[1],0.0)&&GEQ(norm1[2]*norm2[2],0.0))
	    {
	      /* same sense, continue looking */
	      p3d_vectCopy(norm2,norm1);
	    }
	  else
	    {
	      /* norm1 and norm2 not in same sense */
	      result = FALSE;

	    }
	}
    }
    poly_set_face_is_convex_in_face(p,f_id,result);
    return result;
}

/*************************************************************************/
/*!\fn void poly_compute_poly_BB(poly_polyhedre *p, double *xmin,double *xmax,double *ymin,double *ymax,double *zmin,double *zmax)
 * brief compute the bounding box of a polyhedre
 *
 * \param p         the polyhedre
 * \param xmin      pointer to xmin of the BB
 * \param xmax      pointer to xmax of the BB
 * \param ymin      pointer to ymin of the BB
 * \param ymax      pointer to ymax of the BB
 * \param zmin      pointer to zmin of the BB
 * \param zmax      pointer to zmax of the BB
 */
/*************************************************************************/
void poly_compute_poly_BB(poly_polyhedre *p,
			  double *xmin,
			  double *xmax,
			  double *ymin,
			  double *ymax,
			  double *zmin,
			  double *zmax)
{
  unsigned int j;
  poly_vector3 *points;

  points = p->the_points;

  if(points[0])
    {
      /* init extreme BB values by first vertex: */
      *xmin = points[0][0];
      *xmax = points[0][0];
      *ymin = points[0][1];
      *ymax = points[0][1];
      *zmin = points[0][2];
      *zmax = points[0][2];
      /* visit all other vertices: */
      for(j=1;j<p->nb_points;j++)
	{
	  /* get next vertex */
	  /* update extreme BB values */
	  if(*xmin > points[j][0])
	    *xmin = points[j][0];
	  if(*xmax < points[j][0])
	    *xmax = points[j][0];
	  if(*ymin > points[j][1])
	    *ymin = points[j][1];
	  if(*ymax < points[j][1])
	    *ymax = points[j][1];
	  if(*zmin > points[j][2])
	    *zmin = points[j][2];
	  if(*zmax < points[j][2])
	    *zmax = points[j][2];
	}
    }
  else
    {
      *xmin = 0.0;
      *xmax = 0.0;
      *ymin = 0.0;
      *ymax = 0.0;
      *zmin = 0.0;
      *zmax = 0.0;
    }
}
