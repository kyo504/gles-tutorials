/**
 ** Simple Texture 2
 **
 ** This app shows how to create texture and apply it onto the given rectangle
 ** In this case, we use array buffer (glGenBuffers)
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

static GLint samplerLoc;
static GLint positionLoc;
static GLint texcoordLoc;
static GLuint vertexID;
static GLuint texcoordID;
static GLuint textureId;
static GLuint programObject;

GLfloat vVertices[] =
	{
		-0.5f,  0.5f, 0.0f, // Position 0
		-0.5f, -0.5f, 0.0f, // Position 1
		0.5f, -0.5f, 0.0f,  // Position 2
		0.5f, -0.5f, 0.0f,  // Position 3
		0.5f,  0.5f, 0.0f, 	// Position 4
		-0.5f,  0.5f, 0.0f	// Position 6
	};

GLfloat texcoords[] =
	{
		0.0f,  0.0f,        // TexCoord 0
		0.0f,  1.0f,        // TexCoord 1
		1.0f,  1.0f,        // TexCoord 2
		1.0f,  1.0f,        // TexCoord 2
		1.0f,  0.0f,         // TexCoord 3
		0.0f,  0.0f        // TexCoord 0
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
 		 fprintf(stdout, "%s\n", infoLog);
         free ( infoLog );
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;

}

/*
 * Create a simple 2x2 texture image with four different colors
 */
GLuint CreateSimpleTexture2D( )
{
   // Texture object handle
   GLuint textureId;

   // 2x2 Image, 3 bytes per pixel (R, G, B)
   GLubyte pixels[4 * 3] =
   {
      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0  // Yellow
   };

   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, &textureId );

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_2D, textureId );

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   return textureId;
}

static int initOpenGL(void)
{
    char vShaderStr[] =
    	"precision mediump float;					\n"
		"attribute vec4 a_position;               	\n"
    	"attribute vec2 a_texCoord;					\n"
    	"varying vec2 v_texCoord;					\n"
		"void main()                              	\n"
		"{                                        	\n"
		"   gl_Position = a_position;              	\n"
    	"	v_texCoord = a_texCoord;				\n"
		"}                                        	\n";

    char fShaderStr[] =
		"precision mediump float;                       		\n"
    	"uniform sampler2D s_texture;							\n"
    	"varying vec2 v_texCoord;								\n"
		"void main()                                     		\n"
		"{                                               		\n"
		"   gl_FragColor = texture2D(s_texture, v_texCoord);	\n"
		"}                                               		\n";

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
 		 fprintf(stdout, "%s\n", infoLog);
 		 free ( infoLog );
 	  }

 	  glDeleteProgram ( programObject );

 	  return EXIT_FAILURE;
    }

    // Get the sampler location
    samplerLoc = glGetUniformLocation(programObject, "s_texture");

    positionLoc = glGetAttribLocation(programObject, "a_position");
    texcoordLoc = glGetAttribLocation(programObject, "a_texCoord");

    glGenBuffers(1, &vertexID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &texcoordID);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

    // Load texture
    textureId = CreateSimpleTexture2D();

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // We don't need the shaders anymore
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

    return EXIT_SUCCESS;
}

/*
 * Draw a rectangle
 */
void render()
{
	EGLint surface_width;
	EGLint surface_height;

	eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &surface_width);
	eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &surface_height);

	// Set the viewport
	glViewport ( 0, 0, surface_width, surface_height );

	// Clear the color buffer
	glClear ( GL_COLOR_BUFFER_BIT );

	// Use the program object
	glUseProgram(programObject);

	// Load the vertex position
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexAttribPointer( positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0 );
	glEnableVertexAttribArray(positionLoc);

	// Load the texture coordinate
	glBindBuffer(GL_ARRAY_BUFFER, texcoordID);
	glVertexAttribPointer( texcoordLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0 );
	glEnableVertexAttribArray(texcoordLoc);

	// Bind the texture
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, textureId );

	// Set the sampler texture unit to 0
	glUniform1i( samplerLoc, 0);

	//glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
	glDrawArrays( GL_TRIANGLES, 0, 6 );

	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(texcoordLoc);
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
            break;
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
