/**
 ** 3D Cube App
 **
 ** This sample shows rotating 3D cube with 3 axes(x,y,z). Basic transformation and projection techniques are applied
 **
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <screen/screen.h>

#include "glExtension.h"

screen_context_t screen_ctx;
screen_window_t screen_win;

EGLDisplay egl_display;
EGLContext egl_ctx;
EGLSurface egl_surface;

static GLint mvpLoc;
static GLint positionLoc;
static GLint colorLoc;
static GLuint vertexID;
static GLuint colorID;
static GLuint axisVertexID;
static GLuint axisColorID;

GLint direction = 1;
GLuint programObject;

GLfloat model[16], mvp[16], view[16];
GLfloat fScale = 0.5f;
GLfloat fRotateX = -30.0f;
GLfloat fRotateY = 45.0f;
GLfloat mRotX[16], mRotY[16], mRotZ[16], mResult[16];

static const float vAxis[] =
{
	// x Axis
	0.0f, 0.0f, 0.0f,
	2.0f, 0.0f, 0.0f,

	// y Axix
	0.0f, 0.0f, 0.0f,
	0.0f, 2.0f, 0.0f,

	// z Axis
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 2.0f
};

static const float axisColor[] =
{
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f
};

static const float vCube[] =
{
   // Front
   -1.0f, 1.0f, 1.0f,
   -1.0f, -1.0f, 1.0f,
   1.0f, 1.0f, 1.0f,
   1.0f, 1.0f, 1.0f,
   -1.0f, -1.0f, 1.0f,
   1.0f, -1.0f, 1.0f,
   // Right
   1.0f, 1.0f, 1.0f,
   1.0f, -1.0f, 1.0f,
   1.0f, 1.0f, -1.0f,
   1.0f, 1.0f, -1.0f,
   1.0f, -1.0f, 1.0f,
   1.0f, -1.0f, -1.0f,
   // Back
   1.0f, 1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   -1.0f, 1.0f, -1.0f,
   -1.0f, 1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   -1.0f, -1.0f, -1.0f,
   // Left
   -1.0f, 1.0f, -1.0f,
   -1.0f, -1.0f, -1.0f,
   -1.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 1.0f,
   -1.0f, -1.0f, -1.0f,
   -1.0f, -1.0f, 1.0f,
   // Top
   -1.0f, 1.0f, -1.0f,
   -1.0f, 1.0f, 1.0f,
   1.0f, 1.0f, -1.0f,
   1.0f, 1.0f, -1.0f,
   -1.0f, 1.0f, 1.0f,
   1.0f, 1.0f, 1.0f,
   // Bottom
   -1.0f, -1.0f, 1.0f,
   -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, 1.0f,
   1.0f, -1.0f, 1.0f,
   -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f
};

static const float colors[] =
{
   // Front (RED)
   1.0f, 0.0f, 0.0f, 0.5f,
   1.0f, 0.0f, 0.0f, 0.5f,
   1.0f, 0.0f, 0.0f, 0.5f,
   1.0f, 0.0f, 0.0f, 0.5f,
   1.0f, 0.0f, 0.0f, 0.5f,
   1.0f, 0.0f, 0.0f, 0.5f,

   // Right (GREEN)
   0.0f, 1.0f, 0.0f, 0.5f,
   0.0f, 1.0f, 0.0f, 0.5f,
   0.0f, 1.0f, 0.0f, 0.5f,
   0.0f, 1.0f, 0.0f, 0.5f,
   0.0f, 1.0f, 0.0f, 0.5f,
   0.0f, 1.0f, 0.0f, 0.5f,

   // Back (
   1.0f, 0.0f, 1.0f, 1.0f,
   1.0f, 0.0f, 1.0f, 1.0f,
   1.0f, 0.0f, 1.0f, 1.0f,
   1.0f, 0.0f, 1.0f, 1.0f,
   1.0f, 0.0f, 1.0f, 1.0f,
   1.0f, 0.0f, 1.0f, 1.0f,

   // Left (BLUE)
   0.0f, 0.0f, 1.0f, 1.0f,
   0.0f, 0.0f, 1.0f, 1.0f,
   0.0f, 0.0f, 1.0f, 1.0f,
   0.0f, 0.0f, 1.0f, 1.0f,
   0.0f, 0.0f, 1.0f, 1.0f,
   0.0f, 0.0f, 1.0f, 1.0f,

   // Top
   1.0f, 1.0f, 0.0f, 1.0f,
   1.0f, 1.0f, 0.0f, 1.0f,
   1.0f, 1.0f, 0.0f, 1.0f,
   1.0f, 1.0f, 0.0f, 1.0f,
   1.0f, 1.0f, 0.0f, 1.0f,
   1.0f, 1.0f, 0.0f, 1.0f,

   // Bottom
   0.5f, 0.5f, 0.5f, 1.0f,
   0.5f, 0.5f, 0.5f, 1.0f,
   0.5f, 0.5f, 0.5f, 1.0f,
   0.5f, 0.5f, 0.5f, 1.0f,
   0.5f, 0.5f, 0.5f, 1.0f,
   0.5f, 0.5f, 0.5f, 1.0f
};

static int initScreen()
{
    int rc;

    //Create the screen context
    rc = screen_create_context(&screen_ctx, SCREEN_APPLICATION_CONTEXT);
    if (rc) {
        perror("screen_create_window");
        return EXIT_FAILURE;
    }

    //Create the screen window that will be render onto
    rc = screen_create_window(&screen_win, screen_ctx);
    if (rc) {
        perror("screen_create_window");
        return EXIT_FAILURE;
    }

    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_FORMAT, (const int[]){ SCREEN_FORMAT_RGBX8888 });
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, (const int[]){ SCREEN_USAGE_OPENGL_ES2 });

    rc = screen_create_window_buffers(screen_win, 2);
    if (rc) {
        perror("screen_create_window_buffers");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int initEGL(void)
{
    EGLBoolean rc;
    EGLConfig egl_conf = (EGLConfig)0;
    EGLint num_confs = 0;
    const EGLint egl_ctx_attr[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    const EGLint egl_attrib_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 0,
        EGL_DEPTH_SIZE, 0,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SAMPLE_BUFFERS, 1,	// Added for anti-aliased lines
        EGL_NONE
    };

    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_display == EGL_NO_DISPLAY) {
        fprintf(stderr, "eglGetDisplay failed\n");
        return EXIT_FAILURE;
    }

    rc = eglInitialize(egl_display, NULL, NULL);
    if (rc != EGL_TRUE) {
        fprintf(stderr, "eglInitialize failed\n");
        return EXIT_FAILURE;
    }

    rc = eglChooseConfig(egl_display, egl_attrib_list, &egl_conf, 1, &num_confs);
    if ((rc != EGL_TRUE) || (num_confs == 0)) {
        fprintf(stderr, "eglChooseConfig failed\n");
        return EXIT_FAILURE;
    }

    egl_ctx = eglCreateContext(egl_display, egl_conf, EGL_NO_CONTEXT, (EGLint*)&egl_ctx_attr);
    if (egl_ctx == EGL_NO_CONTEXT) {
        fprintf(stderr, "eglCreateContext failed\n");
        return EXIT_FAILURE;
    }

    //Create the EGL surface from the screen window
    egl_surface = eglCreateWindowSurface(egl_display, egl_conf, screen_win, NULL);
    if (egl_surface == EGL_NO_SURFACE) {
        fprintf(stderr, "eglCreateWindowSurface failed\n");
        return EXIT_FAILURE;
    }

    rc = eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_ctx);
    if (rc != EGL_TRUE) {
        fprintf(stderr, "eglMakeCurrent failed\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( EGLenum type, const char *shaderSrc )
{
   GLuint shader = 0;
   GLint compiled;

   // Create the shader object
   shader = glCreateShader ( type );

   if ( !shader ) {
      return 0;
   }

   // Load the shader source
   glShaderSource( shader, 1, &shaderSrc, 0 );

   // Compile the shader
   glCompileShader( shader );

   // Check the compile status
   glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled ) {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 ) {
         char *infoLog = malloc ( sizeof ( char ) * infoLen );
         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
         fprintf(stderr, "%s\n", infoLog);
         free ( infoLog );
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;

}

static int initOpenGL(void)
{
    char vShaderStr[] =
    		"precision mediump float;                 \n"
    		"uniform mat4 u_mvpMat;\n"
    		"attribute vec4 a_position;                \n"
    		"attribute vec4 a_color;                  \n"
    		"varying vec4 v_color;                    \n"
    		"void main()                              \n"
    		"{                                        \n"
    		"   gl_Position = u_mvpMat * a_position;              \n"
    		"   v_color = a_color;                    \n"
    		"}                                        \n";

    char fShaderStr[] =
    		"varying lowp vec4 v_color;                        \n"
    		"void main()                                     \n"
    		"{                                               \n"
    		"   gl_FragColor = v_color;                      \n"
    		"}                                               \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
    fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );

    // Create the program object
    programObject = glCreateProgram ( );

    if ( programObject == 0 ) {
 	  return 0;
    }

    glAttachShader ( programObject, vertexShader );
    glAttachShader ( programObject, fragmentShader );

    // Link the program
    glLinkProgram ( programObject );

    // Check the link status
    glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

    if ( !linked ) {
 	  GLint infoLen = 0;

 	  glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

 	  if ( infoLen > 1 ) {
 		 char *infoLog = malloc ( sizeof ( char ) * infoLen );
 		 glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         fprintf(stderr, "%s\n", infoLog);
 		 free ( infoLog );
 	  }

 	  glDeleteProgram ( programObject );

 	  return EXIT_FAILURE;
    }

    mvpLoc = glGetUniformLocation(programObject, "u_mvpMat");
    positionLoc = glGetAttribLocation(programObject, "a_position");
    colorLoc = glGetAttribLocation(programObject, "a_color");

    // Generate the buffers for the vertex positions and colors:
    glGenBuffers(1, &vertexID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vCube), vCube, GL_STATIC_DRAW);

    glGenBuffers(1, &colorID);
    glBindBuffer(GL_ARRAY_BUFFER, colorID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glGenBuffers(1, &axisVertexID);
    glBindBuffer(GL_ARRAY_BUFFER, axisVertexID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vAxis), vAxis, GL_STATIC_DRAW);

    glGenBuffers(1, &axisColorID);
    glBindBuffer(GL_ARRAY_BUFFER, axisColorID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisColor), axisColor, GL_STATIC_DRAW);

    // Set color to be cleared when glClear is called
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

    // We don't need the shaders anymore
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

    return EXIT_SUCCESS;
}

void render()
{
	EGLint surface_width, surface_height;
	float aspect;

	eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &surface_width);
	eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &surface_height);

	// Set aspect ratio
	aspect = (float)surface_height/surface_width;

	// Set the viewport
	glViewport ( 0, 0, surface_width, surface_height );

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Store the program object
	glUseProgram (programObject);

	///////////////////////////////////////////////////////////
	// Draw x,y,z axes

	// Set the line width
    glLineWidth(2.0);

	glBindBuffer(GL_ARRAY_BUFFER, axisVertexID);
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, axisColorID);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glExtRotateX(fRotateX, mRotX);
	glExtRotateY(fRotateY, mRotY);
	glExtMultiply(model, mRotX, mRotY);
	glExtFrustum(view, -2.0, 2.0, -2.0 * aspect, 2.0 * aspect, -2.0, 2.0);
	glExtMultiply(mvp, view, model);
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp);

	glDrawArrays(GL_LINES, 0, 6);

	//////////////////////////////////////////////////////////
	// Draw a 3D Cube

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, colorID);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glExtRotateX(fRotateX, mRotX);
	glExtRotateY(fRotateY, mRotY);
	glExtMultiply(model, mRotX, mRotY);
	glExtScale(model, fScale, fScale, fScale);
	glExtFrustum(view, -2.0, 2.0, -2.0 * aspect, 2.0 * aspect, -2.0, 2.0);
	glExtMultiply(mvp, view, model);

	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp);

	glDrawArrays ( GL_TRIANGLES, 0, 36 );

//	if (fRotateY > 360) {
//		fRotateY = 0.0f;
//	} else {
//		fRotateY += 1.0f;
//	}

	// Disable attribute arrays
	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(colorLoc);
}

int main(int argc, char *argv[])
{
    int rc;

    rc = initScreen();
    if (rc != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    rc = initEGL();
    if (rc != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    rc = initOpenGL();
    if (rc != EXIT_SUCCESS) {
    	return EXIT_FAILURE;
    }

    while (1) {
    	render();

       	rc = eglSwapBuffers(egl_display, egl_surface);
        if (rc != EGL_TRUE) {
        	fprintf(stderr, "eglSwapBuffers failed\n");
        }
    }

    //Clean up
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(egl_display, egl_surface);
    eglDestroyContext(egl_display, egl_ctx);
    eglTerminate(egl_display);
    eglReleaseThread();

    screen_destroy_window(screen_win);
    screen_destroy_context(screen_ctx);

    return EXIT_SUCCESS;
}
