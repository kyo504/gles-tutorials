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

static GLint projectionLoc;
static GLint modelLoc;
static GLint viewLoc;
static GLint positionLoc;
static GLint colorLoc;

static GLuint vCubeID;
static GLuint vColorID;

GLuint programObject;

GLfloat model[16], mvp[16], view[16], prj[16];
GLfloat fScale = 0.5f;
GLfloat fRotateX = -30.0f;
GLfloat fRotateY = 45.0f;
GLfloat mRotX[16], mRotY[16], mRotZ[16], mResult[16];
GLfloat theta = 0.0f;
GLfloat posX = 0.0f;
GLfloat posZ = 0.0f;

GLfloat gridVertices[] = {
		// Increase X
		-1.0f, -0.1f, -1.0f,
		-1.0f, -0.1f, 1.0f,

		-0.5f, -0.1f, -1.0f,
		-0.5f, -0.1f, 1.0f,

		0.0, -0.1f, -1.0f,
		0.0f, -0.1f, 1.0f,

		0.5f, -0.1f, -1.0f,
		0.5f, -0.1f, 1.0f,

		1.0f, -0.1f, -1.0f,
		1.0f, -0.1f, 1.0f,

		// Increase Z
		-1.0f, -0.1f, -1.0f,
		1.0f, -0.1f, -1.0f,

		-1.0f, -0.1f, -0.5f,
		1.0f, -0.1f, -0.5f,

		-1.0f, -0.1f, 0.0f,
		1.0f, -0.1f, 0.0f,

		-1.0f, -0.1f, 0.5f,
		1.0f, -0.1f, 0.5f,

		-1.0f, -0.1f, 1.0f,
		1.0f,  -0.1f, 1.0f
};

GLfloat gridColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

static const GLfloat vAxisVertices[] = {
		// x Axis
		0.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 0.0f,

		// y Axix
		0.0f, 0.0f, 0.0f,
		0.0f, 3.0f, 0.0f,

		// z Axis
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 3.0f
};

static const float axisColor[] =
{
	1.0f, 0.0f, 0.0f, 1.0f,	// R
	0.0f, 1.0f, 0.0f, 1.0f,	// G
	0.0f, 0.0f, 1.0f, 1.0f	// B
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
   1.0f, 0.0f, 0.0f, 1.0f,
   // Right (GREEN)
   0.0f, 1.0f, 0.0f, 1.0f,
   // Back (YELLOW)
   1.0f, 0.0f, 1.0f, 1.0f,
   // Left (BLUE)
   0.0f, 0.0f, 1.0f, 1.0f,
   // Top
   1.0f, 1.0f, 0.0f, 1.0f,
   // Bottom
   0.5f, 0.5f, 0.5f, 1.0f,
};


/*
 *
 */
int readShaderSource(const char *filename, GLubyte **vertexShader, GLubyte **fragmentShader)
{
	char *vsfilename, *fsfilename;
	FILE *vsfp, *fsfp;
	long vsize, fsize;

	vsfilename = (char *)calloc(1, strlen(filename) + strlen(".vert") + 1);
	strcpy(vsfilename, filename);
	strcat(vsfilename, ".vert");
	vsfp = fopen(vsfilename, "r");

	fsfilename = (char *)calloc(1, strlen(filename) + strlen(".frag") + 1);
	strcpy(fsfilename, filename);
	strcat(fsfilename, ".frag");
	fsfp = fopen(fsfilename, "r");

	if ((vsfp == NULL) || (fsfp == NULL)) {
		fprintf(stdout, "Failed to open shader source files\n");
		free(vsfilename);
		free(fsfilename);
		return 0;
	}

	// find size and load vertex shader
	fseek(vsfp, 0, SEEK_END);
	vsize = ftell(vsfp);
	fseek(vsfp, 0, SEEK_SET);
	*vertexShader = (GLubyte *)calloc(1, vsize + 1);
	memset(*vertexShader, 0, vsize +1);
	if(fread(*vertexShader, vsize, 1, vsfp) != 1) {
		fprintf(stdout, "Failed to open shader source files\n");
		free(vsfilename);
		free(fsfilename);
		return 0;
	}

	// find size and load fragment shader

	fseek(fsfp, 0, SEEK_END);
	fsize = ftell(fsfp);
	fseek(fsfp, 0, SEEK_SET);
	*fragmentShader = (GLubyte *)calloc(1, fsize + 1);
	memset(*fragmentShader, 0, fsize +1);
	if(fread(*fragmentShader, fsize, 1, fsfp) != 1) {
		fprintf(stdout, "Failed to open shader source files\n");
		free(vsfilename);
		free(fsfilename);
		return 0;
	}

	free(vsfilename);
	free(fsfilename);

	return 1;
}

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
         fprintf(stderr, "%s\n", infoLog);
         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
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
    		"attribute vec4 a_position;               \n"
    		"uniform mat4 u_projection;					\n"
    		"uniform mat4 u_view;						\n"
    		"uniform mat4 u_model;						\n"
    		"void main()                              \n"
    		"{                                        \n"
    		"   gl_Position = u_projection * u_view * u_model * a_position;              \n"
    		"}                                        \n";

    char fShaderStr[] =
    		"uniform lowp vec4 u_color;                        \n"
    		"void main()                                     \n"
    		"{                                               \n"
    		"   gl_FragColor = u_color;	\n"
    		"}                                               \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    // You can also read shader sources from files.
//    GLubyte *vsStr = NULL, *fsStr = NULL;

//    if(!readShaderSource("/tmp/shader", &vsStr, &fsStr)) {
//    	return 0;
//    }

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

    positionLoc = glGetAttribLocation(programObject, "a_position");
    projectionLoc = glGetUniformLocation(programObject, "u_projection");
    viewLoc = glGetUniformLocation(programObject, "u_view");
    modelLoc = glGetUniformLocation(programObject, "u_model");
    colorLoc = glGetUniformLocation(programObject, "u_color");

    // We don't need the shaders anymore
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

    return EXIT_SUCCESS;
}

void render()
{
	EGLint surface_width, surface_height;
	GLfloat aspect;

	eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &surface_width);
	eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &surface_height);

//	surface_width = 600;
//	surface_height = 600;

	aspect = (GLfloat)surface_height/(GLfloat)surface_width;

	// Set the color on the given viewport area
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearDepthf(1.0f);
	glDepthRangef(0.0,1.0);
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );

	// Set the viewport
	glViewport ( 0, 0, surface_width, surface_height );
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Store the program object
	glUseProgram (programObject);

	// Set the line width
    glLineWidth(2);

    glEnableVertexAttribArray(positionLoc);
    glEnableVertexAttribArray(colorLoc);

	posX = 3 * cosf(degreesToRadians(theta)) - 3 * sinf(degreesToRadians(theta));
	posZ = 3 * sinf(degreesToRadians(theta)) + 3 * cosf(degreesToRadians(theta));

    glExtLoadIdentity(model);
    glExtLoadIdentity(view);
    glExtLoadIdentity(prj);

    ///////////////////////////////////////////////////////////////////////////
    // For grid and axis

    // Set a model matrix for grid and axis
    glExtScale(model, fScale, fScale, fScale);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

    // Draw a grid
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, gridVertices);
    glUniform4fv(colorLoc, 1, gridColor);
    glDrawArrays(GL_LINES, 0, 20);

    // Draw x, y, z axis
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, vAxisVertices);
    glUniform4fv(colorLoc, 1, &axisColor[0]);
    glDrawArrays(GL_LINES, 0, 2);

    glUniform4fv(colorLoc, 1, &axisColor[4]);
    glDrawArrays(GL_LINES, 2, 2);

    glUniform4fv(colorLoc, 1, &axisColor[8]);
    glDrawArrays(GL_LINES, 4, 2);
    //////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////
    // For the first cube
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, vCube);
    glExtLoadIdentity(model);
    // Set a model matrix for the first cube
    GLfloat rotate[16];
    glExtTranslate(model, 2.0f, 1.0f, 0.0f);
    glExtRotateZ(theta, rotate);
    glExtMultiply(model, model, rotate);
    glExtScale(model, 0.1f, 0.1f, 0.1f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
    	fprintf(stdout, "[glError] code : 0x%04X\n", err);
    }

    // Draw the first cube
    int i=0;
    for(i=0; i<6; i++) {
        glUniform4fv(colorLoc, 1, &colors[4*i]);
        glDrawArrays(GL_TRIANGLES, 6*i, 6);
    }
    /////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    // For the second cube
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, vCube);
    glExtLoadIdentity(model);
    // Set a model matrix for the first cube
    glExtTranslate(model, 1.0f, -0.5f, 0.0f);
    glExtRotateX(theta, rotate);
    glExtMultiply(model, model, rotate);
    glExtScale(model, 0.1f, 0.1f, 0.1f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

    // Draw the first cube
    for(i=0; i<6; i++) {
        glUniform4fv(colorLoc, 1, &colors[4*i]);
        glDrawArrays(GL_TRIANGLES, 6*i, 6);
    }
    /////////////////////////////////////////////////////////////////////////

    // Set a matrix for projection
	glExtPerspective(prj, 45.0f, aspect, 1.0f, 100.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, prj);

	// Set a matrix for view
	glExtLookat(view,
			posX, 1, posZ, 		// EYE (Camera Position)
			0.0f, 0.0f, 0.0f,	// position where EYE looks at
			0.0f, 1.0f, 0.0f	// Up vector
	);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);


	theta = (theta + 1.0f) > 360.0f ? 0.0f : theta + 0.5f;


	// Disable attribute arrays
	glDisableVertexAttribArray(positionLoc);

	glFlush();
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
