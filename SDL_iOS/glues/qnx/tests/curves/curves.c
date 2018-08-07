/*                                                              */
/* This test is based on tess.c from OpenGL Redbook,            */
/* initialization part on QSSL's egl* demo                      */
/*                                                              */
/* // Mike Gorchak, 2009. GLU ES test                           */
/*                                                              */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

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

#define order    4          /* make a cubic spline                                  */
#define cvcount  10         /* number of cvs                                        */
#define stride   3          /* just refers to the number of float values in each cv */
#define numknots (cvcount+order)
#define numcvs   (cvcount*stride)

/* our globals */
float knots[numknots];      /* somewhere to keep the knots   */
float cvs[numcvs];          /* somewhere to keep the cvs     */
int frame=0;                /* for cheezy animation purposes */

GLUnurbsObj* mynurbs=NULL;  /* the nurbs tesselator (or rather a pointer to what will be it) */


void init_scene(int width, int height)
{
   int knot;

   /* Setup our viewport for OpenGL ES */
   glViewport(0, 0, (GLint)width, (GLint)height);
   /* Setup our viewport for GLU ES (required when using OpenGL ES 1.0 only) */
   gluViewport(0, 0, (GLint)width, (GLint)height);

   /* setup a perspective projection */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustumf(-1.0f, 1.0f, -0.75f, 0.75f, 1.0f, 100.0f);

   /* set the modelview matrix */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   /* set point size for the cv rendering */
   glPointSize(4.0f);

   /* set the clear colour to black */
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

   /* no lighting needed */
   glDisable(GL_LIGHTING);

   /* position the camera in the world */
   glTranslatef(0.0f, 0.0f, -1.0f);
   glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
   glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

   /* now, make the nurbs tesselator we are going to use */
   mynurbs=gluNewNurbsRenderer();

   /* these lines set how to tesselate the curve -    */
   /* simply by chopping it up into 100 line segments */
   gluNurbsProperty(mynurbs, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
   gluNurbsProperty(mynurbs, GLU_U_STEP, 100);

   /* set up the knot vector - make it continuous */
   for (knot=0; knot<numknots; knot++)
   {
      knots[knot]=knot;
   }
}

void render_scene()
{
   int count=0;
   int u=0;

   glClear(GL_COLOR_BUFFER_BIT);

   /* Curve color */
   glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

   /* tells GLU we are going to describe a nurbs curve */
   gluBeginCurve(mynurbs);
      /* send it the definition and pointers to cv and knot data */
      gluNurbsCurve(mynurbs, numknots, knots, stride, cvs, order, GLU_MAP1_VERTEX_3);
   /* thats all... */
   gluEndCurve(mynurbs);

   /* Control points color */
   glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

   /* Enable vertex array */
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, cvs);

   /* render the control vertex positions */
   glDrawArrays(GL_POINTS, 0, cvcount);
   glDisableClientState(GL_VERTEX_ARRAY);

   /* Flush all drawings */
   glFlush();

   /* Update knots */
   for (u=0; u<cvcount; u++)
   {
      cvs[count++]=0;
      cvs[count++]=cos(5.2f*sqrt(u*u)+frame*0.01f)*0.5f;
      cvs[count++]=sin(10.0f*sqrt(u*u)+frame*0.013f)*0.5f;
   }
   frame++;
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
   } while(1);

   /* Destroy NURBS renderer */
   gluDeleteNurbsRenderer(mynurbs);

   return 0;
}
