/*                                                              */
/* This test is based on tess.c from OpenGL Redbook,            */
/* initialization part on QSSL's egl* demo                      */
/*                                                              */
/* // Mike Gorchak, 2009. GLU ES test                           */
/*                                                              */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <gf/gf.h>
#include <gf/gf3d.h>
#include <GLES/gl.h>
#include <GLES/egl.h>

#include "glues.h"

gf_dev_t    gfdev;
gf_layer_t  layer;
int         layer_idx;

static EGLDisplay display;
static EGLSurface surface;

static EGLint attribute_list[]=
{
   EGL_NATIVE_VISUAL_ID, 0,
   EGL_NATIVE_RENDERABLE, EGL_TRUE,
   EGL_RED_SIZE, 5,
   EGL_GREEN_SIZE, 5,
   EGL_BLUE_SIZE, 5,
   EGL_DEPTH_SIZE, 16,
   EGL_NONE
};

GLfloat mat_red_diffuse[]={0.7f, 0.0f, 0.1f, 1.0f};
GLfloat mat_green_diffuse[]={0.0f, 0.7f, 0.1f, 1.0f};
GLfloat mat_blue_diffuse[]={0.0f, 0.1f, 0.7f, 1.0f};
GLfloat mat_yellow_diffuse[]={0.7f, 0.8f, 0.1f, 1.0f};
GLfloat mat_ambient[4]={0.2f, 0.2f, 0.2f, 0.2f};
GLfloat mat_specular[]={1.0f, 1.0f, 1.0f, 1.0f};
GLfloat mat_shininess[]={100.0f};
GLfloat knots[8]={0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat pts1[4][4][3], pts2[4][4][3];
GLfloat pts3[4][4][3], pts4[4][4][3];
GLUnurbsObj* nurb;

void init_scene(int width, int height)
{
   int u, v;

   /* Setup our viewport for OpenGL ES */
   glViewport(0, 0, (GLint)width, (GLint)height);
   /* Setup our viewport for GLU ES (required when using OpenGL ES 1.0 only) */
   gluViewport(0, 0, (GLint)width, (GLint)height);

   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);

   /* Since GL ES has no GL_AUTO_NORMAL, we implement it in GLU */
   gluEnable(GLU_AUTO_NORMAL);

   nurb=gluNewNurbsRenderer();
   gluNurbsProperty(nurb, GLU_SAMPLING_TOLERANCE, 25.0f);
   gluNurbsProperty(nurb, GLU_DISPLAY_MODE, GLU_FILL);

   /* Build control points for NURBS mole hills. */
   for(u=0; u<4; u++)
   {
      for(v=0; v<4; v++)
      {
         /* Red. */
         pts1[u][v][0]=2.0f*((GLfloat)u);
         pts1[u][v][1]=2.0f*((GLfloat)v);
         if ((u==1 || u==2) && (v==1 || v==2))
         {
            /* Stretch up middle. */
            pts1[u][v][2]=6.0f;
         }
         else
         {
            pts1[u][v][2]=0.0f;
         }

         /* Green. */
         pts2[u][v][0]=2.0f*((GLfloat)u-3.0f);
         pts2[u][v][1]=2.0f*((GLfloat)v-3.0f);
         if ((u==1 || u==2) && (v==1 || v==2))
         {
            if (u==1 && v==1)
            {
               /* Pull hard on single middle square. */
               pts2[u][v][2]=15.0f;
            }
            else
            {
                /* Push down on other middle squares. */
                pts2[u][v][2]=-2.0f;
            }
         }
         else
         {
            pts2[u][v][2]=0.0f;
         }

         /* Blue. */
         pts3[u][v][0]=2.0f*((GLfloat)u-3.0f);
         pts3[u][v][1]=2.0f*((GLfloat)v);
         if ((u==1 || u==2) && (v==1 || v==2))
         {
            if (u==1 && v==2)
            {
               /* Pull up on single middple square. */
               pts3[u][v][2]=11.0f;
            }
            else
            {
               /* Pull up slightly on other middle squares. */
               pts3[u][v][2]=2.0f;
            }
         }
         else
         {
            pts3[u][v][2]=0.0f;
         }

         /* Yellow. */
         pts4[u][v][0]=2.0f*((GLfloat)u);
         pts4[u][v][1]=2.0f*((GLfloat)v-3.0f);
         if ((u==1 || u==2 || u==3) && (v==1 || v==2))
         {
            if (v==1)
            {
               /* Push down front middle and right squares. */
               pts4[u][v][2]=-2.0f;
            }
            else
            {
               /* Pull up back middle and right squares. */
               pts4[u][v][2]=5.0f;
            }
         }
         else
         {
            pts4[u][v][2]=0.0f;
         }
      }
   }

   /* Stretch up red's far right corner. */
   pts1[3][3][2]=6;
   /* Pull down green's near left corner a little. */
   pts2[0][0][2]=-2;
   /* Turn up meeting of four corners. */
   pts1[0][0][2]=1;
   pts2[3][3][2]=1;
   pts3[3][0][2]=1;
   pts4[0][3][2]=1;

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(55.0f, (GLfloat)width/(GLfloat)height, 2.0f, 24.0f);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0f, 0.0f, -15.0f);
   glRotatef(330.0f, 1.0f, 0.0f, 0.0f);
}

void render_scene()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* Render red hill. */
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_red_diffuse);
   gluBeginSurface(nurb);
      gluNurbsSurface(nurb, 8, knots, 8, knots,
                      4 * 3, 3, &pts1[0][0][0],
                      4, 4, GLU_MAP2_VERTEX_3);
   gluEndSurface(nurb);

   /* Render green hill. */
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_green_diffuse);
   gluBeginSurface(nurb);
      gluNurbsSurface(nurb, 8, knots, 8, knots,
                      4 * 3, 3, &pts2[0][0][0],
                      4, 4, GLU_MAP2_VERTEX_3);
   gluEndSurface(nurb);

   /* Render blue hill. */
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_blue_diffuse);
   gluBeginSurface(nurb);
      gluNurbsSurface(nurb, 8, knots, 8, knots,
                      4 * 3, 3, &pts3[0][0][0],
                      4, 4, GLU_MAP2_VERTEX_3);
   gluEndSurface(nurb);

   /* Render yellow hill. */
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_yellow_diffuse);
   gluBeginSurface(nurb);
      gluNurbsSurface(nurb, 8, knots, 8, knots,
                      4 * 3, 3, &pts4[0][0][0],
                      4, 4, GLU_MAP2_VERTEX_3);
   gluEndSurface(nurb);

   /* Flush all drawings */
   glFlush();
}

int main(int argc, char** argv)
{
   gf_3d_target_t      target;
   gf_display_t        gf_disp;
   EGLConfig           config;
   EGLContext          econtext;
   EGLint              num_config;
   gf_dev_info_t       info;
   gf_layer_info_t     linfo;
   gf_display_info_t   disp_info;
   GLuint              width, height;
   GLuint              it;

   /* initialize the graphics device */
   if (gf_dev_attach(&gfdev, NULL, &info)!=GF_ERR_OK)
   {
      perror("gf_dev_attach()");
      return -1;
   }

   /* Setup the layer we will use */
   if (gf_display_attach(&gf_disp, gfdev, 0, &disp_info)!=GF_ERR_OK)
   {
      fprintf(stderr, "gf_display_attach() failed\n");
      return -1;
   }

   width=disp_info.xres;
   height=disp_info.yres;

   layer_idx=disp_info.main_layer_index;

   /* get an EGL display connection */
   display=eglGetDisplay(gfdev);
   if (display==EGL_NO_DISPLAY)
   {
      fprintf(stderr, "eglGetDisplay() failed\n");
      return -1;
   }

   if (gf_layer_attach(&layer, gf_disp, layer_idx, 0)!=GF_ERR_OK)
   {
      fprintf(stderr, "gf_layer_attach() failed\n");
      return -1;
   }

   /* initialize the EGL display connection */
   if (eglInitialize(display, NULL, NULL)!=EGL_TRUE)
   {
      fprintf(stderr, "eglInitialize: error 0x%x\n", eglGetError());
      return -1;
   }

   for (it=0;; it++)
   {
      /* Walk through all possible pixel formats for this layer */
      if (gf_layer_query(layer, it, &linfo)==-1)
      {
         fprintf(stderr, "Couldn't find a compatible frame "
                         "buffer configuration on layer %d\n", layer_idx);
         return -1;
      }

      /*
       * We want the color buffer format to match the layer format,
       * so request the layer format through EGL_NATIVE_VISUAL_ID.
       */
      attribute_list[1]=linfo.format;

      /* Look for a compatible EGL frame buffer configuration */
      if (eglChooseConfig(display, attribute_list, &config, 1, &num_config)==EGL_TRUE)
      {
         if (num_config>0)
         {
            break;
         }
      }
   }

   /* create a 3D rendering target */
   if (gf_3d_target_create(&target, layer, NULL, 0, width, height, linfo.format)!=GF_ERR_OK)
   {
      fprintf(stderr, "Unable to create rendering target\n");
      return -1;
   }

   gf_layer_set_src_viewport(layer, 0, 0, width-1, height-1);
   gf_layer_set_dst_viewport(layer, 0, 0, width-1, height-1);
   gf_layer_enable(layer);

   /*
    * The layer settings haven't taken effect yet since we haven't
    * called gf_layer_update() yet.  This is exactly what we want,
    * since we haven't supplied a valid surface to display yet.
    * Later, the OpenGL ES library calls will call gf_layer_update()
    * internally, when  displaying the rendered 3D content.
    */

   /* create an EGL rendering context */
   econtext=eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
   if (econtext==EGL_NO_CONTEXT)
   {
      fprintf(stderr, "Create context failed: 0x%x\n", eglGetError());
      return -1;
   }

   /* create an EGL window surface */
   surface=eglCreateWindowSurface(display, config, target, NULL);
   if (surface==EGL_NO_SURFACE)
   {
      fprintf(stderr, "Create surface failed: 0x%x\n", eglGetError());
      return -1;
   }

   /* connect the context to the surface */
   if (eglMakeCurrent(display, surface, surface, econtext)==EGL_FALSE)
   {
      fprintf(stderr, "Make current failed: 0x%x\n", eglGetError());
      return -1;
   }

   init_scene(width, height);

   do {
      render_scene();
      glFinish();
      eglWaitGL();
      eglSwapBuffers(display, surface);

      /* Do not load CPU by rendering the same picture too fast */
      sleep(1);
   } while(1);

   return 0;
}
