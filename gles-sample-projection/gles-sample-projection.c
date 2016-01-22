/**
 ** eglImageApp
 **
 ** Sample app that that shows how to create an EGLImage from a
 ** Screen pixmap, create a texture from that EGLImage, and display
 ** it on a quad on the screen.
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

screen_context_t screen_ctx;
screen_window_t screen_win;

EGLDisplay egl_display;
EGLContext egl_ctx;
EGLSurface egl_surface;

static GLuint positionLoc;
static GLuint colorLoc;
static GLuint vertexID;
static GLuint colorID;
static GLuint mvpLoc;

GLfloat model[16], mvp[16], view[16];

GLuint programObject;

static const float vCube[] =
{
   // Front
   -0.5f, 0.5f, 0.5f,
   -0.5f, -0.5f, 0.5f,
   0.5f, 0.5f, 0.5f,
   0.5f, 0.5f, 0.5f,
   -0.5f, -0.5f, 0.5f,
   0.5f, -0.5f, 0.5f,
   // Right
   0.5f, 0.5f, 0.5f,
   0.5f, -0.5f, 0.5f,
   0.5f, 0.5f, -0.5f,
   0.5f, 0.5f, -0.5f,
   0.5f, -0.5f, 0.5f,
   0.5f, -0.5f, -0.5f,
   // Back
   0.5f, 0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   -0.5f, 0.5f, -0.5f,
   -0.5f, 0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   // Left
   -0.5f, 0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, 0.5f, 0.5f,
   -0.5f, 0.5f, 0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, 0.5f,
   // Top
   -0.5f, 0.5f, -0.5f,
   -0.5f, 0.5f, 0.5f,
   0.5f, 0.5f, -0.5f,
   0.5f, 0.5f, -0.5f,
   -0.5f, 0.5f, 0.5f,
   0.5f, 0.5f, 0.5f,
   // Bottom
   -0.5f, -0.5f, 0.5f,
   -0.5f, -0.5f, -0.5f,
   0.5f, -0.5f, 0.5f,
   0.5f, -0.5f, 0.5f,
   -0.5f, -0.5f, -0.5f,
   0.5f, -0.5f, -0.5f
};

static const float colors[] =
{
   // Front
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   // Right
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   // Back
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   // Left
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   0.0625f, 0.57421875f, 0.92578125f, 1.0f,
   // Top
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   0.29296875f, 0.66796875f, 0.92578125f, 1.0f,
   // Bottom
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f,
   0.52734375f, 0.76171875f, 0.92578125f, 1.0f
};

const float unit_matrix[] =
{
   1.0f, 0.0f, 0.0f, 0.0f,
   0.0f, 1.0f, 0.0f, 0.0f,
   0.0f, 0.0f, 1.0f, 0.0f,
   0.0f, 0.0f, 0.0f, 1.0f
};

static void customLoadIdentity(float matrix[16])
{
	int i = 0;

	for (i = 0; i < 16; i++)
      matrix[i] = unit_matrix[i];
}

static void customMutlMatrix(float matrix[16], const float matrix0[16], const float matrix1[16])
{
   int i, row, column;
   float temp[16];
   for (column = 0; column < 4; column++)
   {
      for (row = 0; row < 4; row++)
      {
         temp[column * 4 + row] = 0.0f;
         for (i = 0; i < 4; i++)
            temp[column * 4 + row] += matrix0[i * 4 + row] * matrix1[column * 4 + i];
      }
   }
   for (i = 0; i < 16; i++)
      matrix[i] = temp[i];
}

static void customScale(float matrix[16], const float sx, const float sy, const float sz)
{
   matrix[0]  *= sx;
   matrix[1]  *= sx;
   matrix[2]  *= sx;
   matrix[3]  *= sx;

   matrix[4]  *= sy;
   matrix[5]  *= sy;
   matrix[6]  *= sy;
   matrix[7]  *= sy;

   matrix[8]  *= sz;
   matrix[9]  *= sz;
   matrix[10] *= sz;
   matrix[11] *= sz;
}

static int customFrustum(float result[16],
              const float left, const float right,
              const float bottom, const float top,
              const float near, const float far)
{
   if ((right - left) == 0.0f || (top - bottom) == 0.0f || (far - near) == 0.0f)
      return 0;

   result[0] = 2.0f / (right - left);
   result[1] = 0.0f;
   result[2] = 0.0f;
   result[3] = 0.0f;

   result[4] = 0.0f;
   result[5] = 2.0f / (top - bottom);
   result[6] = 0.0f;
   result[7] = 0.0f;

   result[8] = 0.0f;
   result[9] = 0.0f;
   result[10] = -2.0f / (far - near);
   result[11] = 0.0f;

   result[12] = -(right + left) / (right - left);
   result[13] = -(top + bottom) / (top - bottom);
   result[14] = -(far + near) / (far - near);
   result[15] = 1.0f;

   return 1;
}

//static void customRotate(float matrix[16], const float anglex, const float angley, const float anglez)
//{
//   const float pi = 3.141592f;
//   float temp[16];
//   float rz = 2.0f * pi * anglez / 360.0f;
//   float rx = 2.0f * pi * anglex / 360.0f;
//   float ry = 2.0f * pi * angley / 360.0f;
//   float sy = asinf(ry);
//   float cy = acosf(ry);
//   float sx = asinf(rx);
//   float cx = acosf(rx);
//   float sz = asinf(rz);
//   float cz = acosf(rz);
//
//   customLoadIdentity(temp);
//
//   temp[0] = cy * cz - sx * sy * sz;
//   temp[1] = cz * sx * sy + cy * sz;
//   temp[2] = -cx * sy;
//
//   temp[4] = -cx * sz;
//   temp[5] = cx * cz;
//   temp[6] = sx;
//
//   temp[8] = cz * sy + cy * sx * sz;
//   temp[9] = -cy * cz * sx + sy * sz;
//   temp[10] = cx * cy;
//
//   customMutlMatrix(matrix, matrix, temp);
//}

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
    		"attribute vec4 a_Position;                \n"
    		"attribute vec4 a_color;                  \n"
    		"varying vec4 v_color;                    \n"
    		"void main()                              \n"
    		"{                                        \n"
    		"   gl_Position = u_mvpMat * a_Position;              \n"
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


    // We don't need the shaders anymore
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

    return EXIT_SUCCESS;
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

    EGLint surface_width;
    EGLint surface_height;

	   eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &surface_width);
	   eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &surface_height);

	   // Set the color on the given viewport area
	   glClearDepthf(1.0f);
	   glClearColor ( 0.0f, 1.0f, 1.0f, 1.0f );
//	   glEnable(GL_CULL_FACE);
//	   glEnable(GL_SCISSOR_TEST);
//	   glScissor(0,0,600,600);

	   // Set the viewport
	   glViewport ( 0, 0, surface_width, surface_height );
	   // Clear the color buffer
	   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    // Store the program object
	   glUseProgram (programObject);

	   // Load the vertex data
//	   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vSquare );
//	   glEnableVertexAttribArray ( 0 );
//	   glDrawArrays ( GL_TRIANGLE_FAN, 0, 4 );

	   glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	   glEnableVertexAttribArray(positionLoc);
	   glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

//	   glBindBuffer(GL_ARRAY_BUFFER, colorID);
//	   glEnableVertexAttribArray(colorLoc);
//	   glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	   customLoadIdentity(model);
	   customLoadIdentity(view);
	   float aspect = 1;
	   customFrustum(view, -1.0, 1.0, -1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
	   //customRotate(model, 45.0f, 45.0f, 0.0f);
	   customMutlMatrix(mvp, view, model);

	   glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp);
	   //glUniformMatrix4fv(transformLoc, 1, false, matrix);

	   glDrawArrays ( GL_TRIANGLES, 0, 6 );
	   //glDrawElements ( GL_TRIANGLE_STRIP, 36, GL_FLOAT, 0);

	   glFlush();

   	rc = eglSwapBuffers(egl_display, egl_surface);
       if (rc != EGL_TRUE) {
           fprintf(stderr, "eglSwapBuffers failed\n");
       }

    while (1) {


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
