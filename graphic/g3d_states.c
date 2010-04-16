#include "Graphic-pkg.h"
#include "P3d-pkg.h"
#include "Util-pkg.h"

g3d_states g3d_init_viewer_state(double size)
{
    g3d_states vs;

    vs.size       = size;
    vs.FILAIRE = 0;
    vs.CONTOUR = 0;
    vs.GHOST = 0;
    vs.BB = 0;
    vs.GOURAUD = 0;
    vs.ACTIVE = 1;
    vs.list = -1;

    vs.projection_mode = G3D_PERSPECTIVE;
    vs.transparency_mode = G3D_TRANSPARENT_AND_OPAQUE;

    vs.frustum[0][0]= 0.0;  vs.frustum[0][1]= 0.0;  vs.frustum[0][2]= 0.0;  vs.frustum[0][3]= 0.0;
    vs.frustum[1][0]= 0.0;  vs.frustum[1][1]= 0.0;  vs.frustum[1][2]= 0.0;  vs.frustum[1][3]= 0.0;
    vs.frustum[2][0]= 0.0;  vs.frustum[2][1]= 0.0;  vs.frustum[2][2]= 0.0;  vs.frustum[2][3]= 0.0;
    vs.frustum[3][0]= 0.0;  vs.frustum[3][1]= 0.0;  vs.frustum[3][2]= 0.0;  vs.frustum[3][3]= 0.0;
    vs.frustum[4][0]= 0.0;  vs.frustum[4][1]= 0.0;  vs.frustum[4][2]= 0.0;  vs.frustum[4][3]= 0.0;
    vs.frustum[5][0]= 0.0;  vs.frustum[5][1]= 0.0;  vs.frustum[5][2]= 0.0;  vs.frustum[5][3]= 0.0;

    vs.floorColor[0]= 0.5;
    vs.floorColor[1]= 0.9;
    vs.floorColor[2]= 0.9;
    vs.wallColor[0]= 0.5;
    vs.wallColor[1]= 0.5;
    vs.wallColor[2]= 0.6;
    vs.displayFrame = 1;
    vs.displayJoints = 0;
    vs.enableLight = 1;
    vs.displayShadows = 0;
    vs.displayWalls = 0;
    vs.displayFloor = 0;
    vs.displayTiles = 0;
    vs.allIsBlack = 0;

    //Les plans du sol et des murs vont être ajustés sur les coordonnées de
    //l'environment_box.
    double xmin, xmax, ymin, ymax, zmin, zmax;
    p3d_get_env_box(&xmin, &xmax, &ymin, &ymax, &zmin, &zmax);

    if( xmin>=xmax || ymin>=ymax || zmin>=zmax)
    {
            printf("%s: %d: g3d_new_win(): mauvais paramètres pour la commande p3d_set_env_box.\n\t", __FILE__, __LINE__);
            printf("Il faut les donner sous la forme xmin ymin zmin xmax ymax zmax.\n");
    }


    GLdouble v0[3], v1[3], v2[3];

    //plan du sol (normale selon Z):
    v0[0]= xmin;      v1[0]= xmax;      v2[0]= xmin;
    v0[1]= ymin;      v1[1]= ymin;      v2[1]= ymax;
    v0[2]= zmin;      v1[2]= zmin;      v2[2]= zmin;
    g3d_findPlane(vs.floorPlane, v0, v1, v2);

    //plan du premier mur (normale selon y):
    v0[0]= xmin;      v1[0]= xmin;      v2[0]= xmax;
    v0[1]= ymin;      v1[1]= ymin;      v2[1]= ymin;
    v0[2]= zmin;      v1[2]= zmax;      v2[2]= zmin;
    g3d_findPlane(vs.wallPlanes[0], v0, v1, v2);

    //plan du deuxième mur (normale selon -y):
    v0[0]= xmin;      v1[0]= xmax;      v2[0]= xmin;
    v0[1]= ymax;      v1[1]= ymax;      v2[1]= ymax;
    v0[2]= zmin;      v1[2]= zmin;      v2[2]= zmax;
    g3d_findPlane(vs.wallPlanes[1], v0, v1, v2);

    //plan du troisième mur (normale selon x):
    v0[0]= xmin;      v1[0]= xmin;      v2[0]= xmin;
    v0[1]= ymin;      v1[1]= ymax;      v2[1]= ymin;
    v0[2]= zmin;      v1[2]= zmin;      v2[2]= zmax;
    g3d_findPlane(vs.wallPlanes[2], v0, v1, v2);


    //plan du quatrième mur (normale selon -x):
    v0[0]= xmax;      v1[0]= xmax;      v2[0]= xmax;
    v0[1]= ymin;      v1[1]= ymax;      v2[1]= ymax;
    v0[2]= zmin;      v1[2]= zmax;      v2[2]= zmin;
    g3d_findPlane(vs.wallPlanes[3], v0, v1, v2);

    //positionnement de la lumière:
    vs.lightPosition[0]= 0.5*(xmin+xmax);
    vs.lightPosition[1]= 0.5*(ymin+ymax);
    vs.lightPosition[2]= 0.9*(zmin+zmax);
    vs.lightPosition[3]= 1.0;

    return vs;
}

//-----------------------------------------------------------------------------
// Name: findPlane()
// Desc: find the plane equation given 3 points
//-----------------------------------------------------------------------------
void g3d_findPlane( GLdouble plane[4], GLdouble v0[3], GLdouble v1[3], GLdouble v2[3] )
{
  GLdouble vec0[3], vec1[3];

  // Need 2 vectors to find cross product
  vec0[0] = v1[0] - v0[0];
  vec0[1] = v1[1] - v0[1];
  vec0[2] = v1[2] - v0[2];

  vec1[0] = v2[0] - v0[0];
  vec1[1] = v2[1] - v0[1];
  vec1[2] = v2[2] - v0[2];

  // Find cross product to get A, B, and C of plane equation
  plane[0] =   vec0[1] * vec1[2] - vec0[2] * vec1[1];
  plane[1] = -(vec0[0] * vec1[2] - vec0[2] * vec1[0]);
  plane[2] =   vec0[0] * vec1[1] - vec0[1] * vec1[0];

  plane[3] = -(plane[0] * v0[0] + plane[1] * v0[1] + plane[2] * v0[2]);
}

// Construit les matrices de projection des ombres sur les plans du sol et des murs.
// Cette fonction doit être appelée chaque fois que la position de la lumière change.
void g3d_build_shadow_matrices(g3d_states &vs)
{
  buildShadowMatrix( vs.floorShadowMatrix, vs.lightPosition, vs.floorPlane );
  buildShadowMatrix( vs.wallShadowMatrix[0], vs.lightPosition, vs.wallPlanes[0] );
  buildShadowMatrix( vs.wallShadowMatrix[1], vs.lightPosition, vs.wallPlanes[1] );
  buildShadowMatrix( vs.wallShadowMatrix[2], vs.lightPosition, vs.wallPlanes[2] );
  buildShadowMatrix( vs.wallShadowMatrix[3], vs.lightPosition, vs.wallPlanes[3] );
}

void
g3d_set_win_bgcolor(g3d_states &vs, float r, float v, float b) {
  vs.bg[0] = r;
  vs.bg[1] = v;
  vs.bg[2] = b;
}

void
g3d_set_win_floor_color(g3d_states &vs, float r, float v, float b) {
  vs.floorColor[0] = r;
  vs.floorColor[1] = v;
  vs.floorColor[2] = b;
}

void
g3d_set_win_wall_color(g3d_states &vs, float r, float v, float b) {
  vs.wallColor[0] = r;
  vs.wallColor[1] = v;
  vs.wallColor[2] = b;
}


void
g3d_set_win_camera(g3d_states &vs, float ox,float oy, float oz,
                   float dist, float az, float el,
                   float up0, float up1, float up2) {
  vs.x = ox;
  vs.y = oy;
  vs.z = oz;
  vs.zo = dist;
  vs.az = az, vs.el = el;
  vs.up[0] = up0;
  vs.up[1] = up1, vs.up[2] = up2;
  vs.up[3]=0.0;
}

void
g3d_set_win_center(g3d_states &vs, float ox,float oy, float oz) {
  vs.x = ox;
  vs.y = oy;
  vs.z = oz;
}


void
g3d_save_win_camera(g3d_states &vs) {
  int i;

  vs.sx = vs.x;
  vs.sy = vs.y;
  vs.sz = vs.z;
  vs.szo = vs.zo;
  vs.saz = vs.az, vs.sel = vs.el;
  for(i=0;i<4;i++) vs.sup[i] = vs.up[i];
}

void
g3d_restore_win_camera(g3d_states &vs) {
  int i;

  vs.x = vs.sx;
  vs.y = vs.sy;
  vs.z = vs.sz;
  vs.zo = vs.szo;
  vs.az = vs.saz, vs.el = vs.sel;
  for(i=0;i<4;i++) vs.up[i] = vs.sup[i];
}

void
g3d_load_saved_camera_params(double* params)
{
  g3d_states vs =  g3d_get_states_by_name((char*)"Move3D");
  int i;

  vs.sx = params[0];
  vs.sy = params[1];
  vs.sz = params[2];
  vs.szo = params[3];
  vs.saz = params[4], vs.sel = params[5];
  for(i=0;i<4;i++) vs.sup[i] = params[6+i];
}

/* fonctions pour copier les donnees relies a la camera de     */
/* la structure G3D_Window de facon utilisable dans operations */
/* avec transformations homogenes                              */
void
get_lookat_vector(g3d_states &vs, p3d_vector4 Vec) {
  Vec[0] = vs.x;
  Vec[1] = vs.y;
  Vec[2] = vs.z;
  Vec[3] = 1.0;
}

void
get_pos_cam_matrix(g3d_states &vs, p3d_matrix4 Transf) {
  /* Caution: ici on change les parametres de translation de la */
  /* matrix, les rest des elementes doivent etre initialises    */
  /* dans la fonction qu'appel                                  */
  Transf[0][3] = vs.zo * (cos(vs.az)*cos(vs.el));
  Transf[1][3] = vs.zo * (sin(vs.az)*cos(vs.el));
  Transf[2][3] = vs.zo * sin(vs.el);
}

//! Moves the camera in its look direction while removing the motions along Z axis.
//! \param d the length of the desired camera motion
void g3d_move_win_camera_forward(g3d_states &vs, float d )
{
        vs.x = vs.x - cos(vs.az)*d;
        vs.y = vs.y - sin(vs.az)*d;
}


//! \param d the length of the desired camera motion
void g3d_move_win_camera_sideways(g3d_states &vs, float d )
{
        vs.x = vs.x + sin(vs.az)*d;
        vs.y = vs.y - cos(vs.az)*d;
}

//! Rotates the camera around Z axis
//! \param d the angle of the desired camera rotation
void g3d_rotate_win_camera_rz(g3d_states &vs, float d )
{
        vs.az = vs.az - d;
}

//! Performs a camera zoom.
//! \param d the "distance" of the zoom
void g3d_zoom_win_camera(g3d_states &vs, float d )
{
   vs.zo = vs.zo - d;
   if(vs.zo < 0.1) vs.zo= 0.1;
}

/* fonction pour recalculer le vector 'up' de la camera */
/* quand on change la reference                         */
void
recalc_cam_up(g3d_states &vs, p3d_matrix4 transf) {
  p3d_vector4 v_up;
  int i;

  p3d_matvec4Mult(transf,vs.up,v_up);
  for(i=0;i<4;i++) vs.up[i] = v_up[i];
}

/* fonction qui change les parametres de position de la       */
/* camera qui son controles pour la souris quand on change    */
/* la reference                                               */
void
recalc_mouse_param(g3d_states &vs, p3d_vector4 Xc, p3d_vector4 Xw) {
  double incx,incy,incz,incd;
  double azim,elev;

  vs.x = Xw[0];
  vs.y = Xw[1];
  vs.z = Xw[2];

  incx = Xc[0] - Xw[0];
  incy = Xc[1] - Xw[1];
  incz = Xc[2] - Xw[2];
  incd = sqrt(SQR(incx)+SQR(incy));
  azim = atan2(incy,incx);
  elev = atan2(incz,incd);
  /* set azim and elev in right range */
  azim = angle_limit_2PI(azim);

  if(elev>M_PI/2) {
    elev = M_PI - elev;
    if(azim>M_PI) azim -= M_PI;
    else azim += M_PI;
  }
  if(elev<-M_PI/2) {
    elev = -M_PI - elev;
    if(azim>M_PI) azim -= M_PI;
    else azim += M_PI;
  }
  vs.az = azim;
  vs.el = elev;
}

//! @ingroup graphic
//! Initializes OpenGL main parameters.
void g3d_init_OpenGL()
{
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.2, 1.2);

  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

//! @ingroup graphic
//! Sets the OpenGL projection matrix used by default by the g3d_windows.
//! Use this function instead of calling directly gluPerspective (unlesss you want some specific parameters)
//! to avoid dispersion of the same setting code.
//! \param mode projection mode (perspective or orthographic)
void g3d_set_projection_matrix(g3d_projection_mode mode)
{
  GLint current_mode;
  GLint viewport[4], width, height;
  GLdouble ratio, d;

  g3d_states vs = g3d_get_cur_states();

  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetIntegerv(GL_MATRIX_MODE, &current_mode);

  width = viewport[2];
  height= viewport[3];

  ratio= ((GLdouble) width)/((GLdouble) height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  switch(mode)
  {
    case G3D_PERSPECTIVE:
      gluPerspective(25.0, ratio, vs.size/500.0, 100.0*vs.size);
    break;
    case G3D_ORTHOGRAPHIC:
      d= vs.zo;
      glOrtho(-ratio*d, ratio*d, -d, d, -10*d, 10*d);
    break;
  }

  glMatrixMode(current_mode);
}

//! @ingroup graphic
//! Saves the current OpenGL pixel buffer as a ppm (PortablePixMap) image file (uncompressed format).
//! In other words: takes a screenshot of the current active OpenGL window.
//! \param filename name of the image file where to save the pixel buffer
//! \return 1 in case of success, 0,otherwise
int g3d_export_OpenGL_display(char *filename)
{
  size_t length;
  unsigned int i,j, width, height, change_name= 0;
  GLint viewport[4];
  char filename2[128];
  unsigned char *pixels= NULL;
  unsigned char *pixels_inv= NULL;
  FILE *file= NULL;

  glGetIntegerv(GL_VIEWPORT, viewport);
  width = viewport[2];
  height= viewport[3];

  pixels    = (unsigned char*) malloc(3*width*height*sizeof(unsigned char));
  pixels_inv= (unsigned char*) malloc(3*width*height*sizeof(unsigned char));

  length= strlen(filename);

  if(length < 5)
  {  change_name= 1;  }
  else if(filename[length-4]!='.' || filename[length-3]!='p' || filename[length-2]!='p' || filename[length-1]!='m')
  {  change_name= 1;  }

  if(change_name)
  {
    printf("%s: %d: file \"%s\" does not have the good extension (it should be a .ppm file).\n",__FILE__,__LINE__, filename);
    strcpy(filename2, filename);
    strcat(filename2, ".ppm");
    printf("It is renamed \"%s\".\n",filename2);
  }
  else
  { strcpy(filename2, filename);  }

  file= fopen(filename2,"w");


  if(file==NULL)
  {
    printf("%s: %d: can not open \"%s\".\n",__FILE__,__LINE__,filename2);
    fclose(file);
    return 0;
  }

  glReadBuffer(GL_FRONT);

  // choose 1-byte alignment:
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  // get the image pixels (from (0,0) position):
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  // glReadPixels returns an upside-down image.
  // we have to first flip it
  // NB: in pixels the 3 colors of a pixel follows each other immediately (RGBRGBRGB...RGB).
  for(i=0; i<width; i++)
  {
    for(j=0; j<height; j++)
    {
      pixels_inv[3*(i+j*width)]  = pixels[3*(i+(height-1-j)*width)+0];
      pixels_inv[3*(i+j*width)+1]= pixels[3*(i+(height-1-j)*width)+1];
      pixels_inv[3*(i+j*width)+2]= pixels[3*(i+(height-1-j)*width)+2];
    }
  }

  fprintf(file, "P6\n");
  fprintf(file, "# creator: BioMove3D\n");
  fprintf(file, "%d %d\n", width, height);
  fprintf(file, "255\n");

  fwrite(pixels_inv, sizeof(unsigned char), 3*width*height, file);

  fclose(file);

  free(pixels);
  free(pixels_inv);

  return 1;
}

//! @ingroup graphic
//! Sets the light parameters for things that will be displayed in the shadow.
void g3d_set_dim_light()
{
  g3d_states vs = g3d_get_cur_states();

  GLfloat light_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
  GLfloat light_diffuse[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
  GLfloat light_specular[4]= { 0.3f, 0.3f, 0.3f, 1.0f };

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, vs.lightPosition);
}

//! @ingroup graphic
//! Sets the default material parameters for OpenGL.
void g3d_set_default_material()
{
  GLfloat mat_ambient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
  GLfloat mat_diffuse[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
  GLfloat mat_specular[4]= { 0.8f, 0.8f, 0.8f, 1.0f };
  GLfloat mat_emission[4]= { 0.05f, 0.05f, 0.05f, 1.0f };
  GLfloat shininess = 90.0f;

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

//    GLfloat specularity = 0.3f;
//     GLfloat emissivity = 0.05f;
//     GLfloat shininess = 10.0f;
//     GLfloat materialColor[] = {0.2f, 0.2f, 1.0f, 1.0f};
//     //The specular (shiny) component of the material
//     GLfloat materialSpecular[] = {specularity, specularity, specularity, 1.0f};
//     //The color emitted by the material
//     GLfloat materialEmission[] = {emissivity, emissivity, emissivity, 1.0f};
//
//     glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);
//     glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
//     glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
//     glMaterialf(GL_FRONT, GL_SHININESS, shininess); //The shininess parameter

      /////////////   From xavier
//  GLfloat mat_ambient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
//  GLfloat mat_diffuse[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
//  GLfloat mat_specular[4]= { 0.5f, 0.5f, 0.5f, 1.0f };
//  GLfloat mat_emission[4]= { 0.2f, 0.2f, 0.2f, 1.0f };
//  GLfloat shininess = 60.0f;
//
//  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
//  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
//  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
//  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
//  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
//
//     GLfloat specularity = 0.3f;
//     GLfloat emissivity = 0.05f;
//     GLfloat shininess = 25.0f;
//     GLfloat materialColor[] = {0.2f, 0.2f, 1.0f, 1.0f};
//     //The specular (shiny) component of the material
//     GLfloat materialSpecular[] = {specularity, specularity, specularity, 1.0f};
//     //The color emitted by the material
//     GLfloat materialEmission[] = {emissivity, emissivity, emissivity, 1.0f};
//
//     glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);
//     glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
//     glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
//     glMaterialf(GL_FRONT, GL_SHININESS, shininess); //The shininess parameter

}

//! @ingroup graphic
//! Set the material parameters for things that are in the shadow (floor or wall part) for OpenGL.
void g3d_set_shade_material()
{
  GLfloat mat_ambient[4]    = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat mat_diffuse[4]    = { 0.4f, 0.4f, 0.4f, 1.0f };
  GLfloat mat_specular[4]   = { 0.2f, 0.2f, 0.2f, 1.0f };
  GLfloat mat_emission[4]   = { 0.05f, 0.05f, 0.05f, 1.0f };
  GLfloat shininess = 10.0f;

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void g3d_draw_frame(void) {
//   GLdouble mat_ambient_diffuse[4]= { 0., .0, .0, 1.0 };
  g3d_states vs  = g3d_get_cur_states();

  double a= vs.size;
  double a1,a9;

  a1 = .1 * a;
  a9 = .9 * a;

  glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_LINE_BIT);

  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glColor3d(0.,0.,0.);
  glLineWidth(2.0);
  glBegin(GL_LINES);
  glVertex3d(.0, .0, .0);
  glVertex3d(.0, .0, a);
  glVertex3d(.0, .0, .0);
  glVertex3d(a, .0, .0);
  glVertex3d(.0, .0, .0);
  glVertex3d(.0, a, .0);
  glEnd();
  glLineWidth(1.0);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glDisable(GL_CULL_FACE);
  glBegin(GL_POLYGON);
  glVertex3d(.0, .0, a);
  glVertex3d(-a1, .0, a9);
  glVertex3d(a1, .0, a9);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex3d(a, .0, .0);
  glVertex3d(a9, .0, -a1);
  glVertex3d(a9, .0, a1);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex3d(.0, a, .0);
  glVertex3d(.0, a9, -a1);
  glVertex3d(.0, a9, a1);
  glEnd();

 glPopAttrib();
}