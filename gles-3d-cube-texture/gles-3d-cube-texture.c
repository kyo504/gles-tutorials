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

#include <img/img.h>
#include <img/img_codec.h>

// Pre-calculated value of PI / 180.
#define kPI180     0.017453

// Pre-calculated value of 180 / PI.
#define k180PI    57.295780

// Converts degrees to radians.
#define degreesToRadians(x) (x * kPI180)

// Converts radians to degrees.
#define radiansToDegrees(x) (x * k180PI)

screen_context_t screen_ctx;
screen_window_t screen_win;

EGLDisplay egl_display;
EGLContext egl_ctx;
EGLSurface egl_surface;

static GLuint positionLoc;
static GLuint samplerLoc;
static GLuint texcoordLoc;
static GLuint vertexID;
static GLuint mvpLoc;
static GLuint texcoordID;
static GLuint textureID;

GLfloat model[16], mvp[16], view[16];

GLuint programObject;

GLfloat fScale = 0.3f;
GLfloat fRotateX = -30.0f;
GLfloat fRotateY = 45.0f;
GLfloat mRotX[16], mRotY[16], mRotZ[16], mResult[16];

GLint direction = 1;

img_lib_t ilib;

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

static const float texcoord[] =
{
   // Front
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
   // Right
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
   // Back
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
   // Left
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
   // Top
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
   // Bottom
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f
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
         fprintf(stderr, "%s\n", infoLog);
         free ( infoLog );
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;
}

GLuint createTexture2D()
{
	GLuint textureId;

	GLubyte pixels[4*3] =
	{
		255,   0,   0, 	// Red
		0, 255,   0,	// Green
		0,   0, 255,	// Blue
		255, 255, 0 	// Yellow
	};

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			2, // the width of the image in pixels
			2, // the height of the image in pixels
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			pixels
	);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureId;
}

/*
 * This is not used function in this sample
 */
int loadImage(img_lib_t imageLib)
{
	int rc;
	img_t img;

	img_lib_t test = NULL;

	if ((rc = img_lib_attach(&test)) != IMG_ERR_OK) {
		fprintf(stderr, "img_lib_attach() failed: %d\n", rc);
		return -1;
	}

	/* initialize an img_t by setting its flags to 0 */
	img.flags = 0;

	/* if we want, we can preselect a format (ie force the image to be
	   loaded in the format we specify) by enabling the following two
	   lines */

	img.format = IMG_FMT_PKLE_ARGB1555;
	img.flags |= IMG_FORMAT;

	/* likewise, we can 'clip' the loaded image by enabling the following */

	img.w = 64;
	img.flags |= IMG_W;

	img.h = 64;
	img.flags |= IMG_H;

	if ((rc = img_load_file(imageLib, "tile_floor.png", NULL, &img)) != IMG_ERR_OK) {
		fprintf(stderr, "img_load_file failed: %d\n", rc);
		return -1;
	}

	fprintf(stdout, "img is %dx%dx%d\n", img.w, img.h, IMG_FMT_BPP(img.format));

	return rc;
}

static int initOpenGL(void)
{
    char vShaderStr[] =
    		"precision mediump float;                				\n"
    		"uniform mat4 u_mvpMat;									\n"
    		"attribute vec4 a_position;                				\n"
    		"attribute vec2 a_texCoord;								\n"
			"varying vec2 v_texCoord;								\n"
    		"void main()                              				\n"
    		"{                                        				\n"
    		"   gl_Position = u_mvpMat * a_position;				\n"
    		"	v_texCoord = a_texCoord;							\n"
    		"}														\n";

    char fShaderStr[] =
    		"precision mediump float;                       		\n"
    		"varying vec2 v_texCoord;								\n"
    		"uniform sampler2D s_texture;							\n"
    		"void main()                                   			\n"
    		"{                                               		\n"
    		"   gl_FragColor = texture2D(s_texture, v_texCoord);	\n"
    		"}                                              		\n";

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
    samplerLoc = glGetUniformLocation(programObject, "s_texture");
    positionLoc = glGetAttribLocation(programObject, "a_position");
    texcoordLoc = glGetAttribLocation(programObject, "a_texCoord");

    // Generate the buffers for the vertex positions and texture coordinates:
    glGenBuffers(1, &vertexID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vCube), vCube, GL_STATIC_DRAW);

    glGenBuffers(1, &texcoordID);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);

    // Load texture
    textureID = createTexture2D();

    // We don't need the shaders anymore
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

    return EXIT_SUCCESS;
}

void matrixRotateX(float degrees, float matrix[16])
{
    float radians = degreesToRadians(degrees);

    customLoadIdentity(matrix);

    // Rotate X formula.
    matrix[5] = cosf(radians);
    matrix[6] = -sinf(radians);
    matrix[9] = -matrix[6];
    matrix[10] = matrix[5];
}

void matrixRotateY(float degrees, float matrix[16])
{
    float radians = degreesToRadians(degrees);

    customLoadIdentity(matrix);

    // Rotate Y formula.
    matrix[0] = cosf(radians);
    matrix[2] = sinf(radians);
    matrix[8] = -matrix[2];
    matrix[10] = matrix[0];
}

void matrixRotateZ(float degrees, float matrix[16])
{
    float radians = degreesToRadians(degrees);

    customLoadIdentity(matrix);

    // Rotate Z formula.
    matrix[0] = cosf(radians);
    matrix[1] = sinf(radians);
    matrix[4] = -matrix[1];
    matrix[5] = matrix[0];
}

void matrixMultiply(float m1[16], float m2[16], float result[16])
{
    // Fisrt Column
    result[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
    result[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
    result[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
    result[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

    // Second Column
    result[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
    result[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
    result[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
    result[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

    // Third Column
    result[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
    result[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
    result[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
    result[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

    // Fourth Column
    result[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
    result[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
    result[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
    result[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
}

void render()
{
	EGLint surface_width;
	EGLint surface_height;
	float aspect;

	eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &surface_width);
	eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &surface_height);

	// Set the color on the given viewport area
	glClearDepthf(1.0f);
	glClearColor ( 0.2f, 0.2f, 0.2f, 1.0f );
	glEnable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);
	glScissor(0,0,surface_width,surface_height);

	// Set the viewport
	glViewport ( 0, 0, surface_width, surface_height );
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Store the program object
	glUseProgram (programObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, texcoordID);
	glEnableVertexAttribArray(texcoordLoc);
	glVertexAttribPointer(texcoordLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// Bind the texture
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, textureID );

	glDrawArrays ( GL_TRIANGLES, 0, 36 );

	customLoadIdentity(model);
	matrixRotateX(fRotateX, mRotX);
	matrixRotateY(fRotateY, mRotY);
	matrixMultiply(mRotX, mRotY, model);
	customScale(model, fScale, fScale, fScale);
	aspect = (float)surface_height/surface_width;
	customFrustum(view, -1.0, 1.0, -1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
	customMutlMatrix(mvp, view, model);

	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp);

	// Set the sampler texture unit to 0
	glUniform1i ( samplerLoc, 0 );

	// Disable attribute arrays
	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(texcoordLoc);

	//printf("RotateY = %f\n", fRotateY);

	if (fRotateY > 360) {
		fRotateY = 0.0f;
	} else {
		fRotateY += 1.0f;
	}

//	if (direction > 0) {
//		direction = fScale > 0.5 ? -1 : 1;
//	} else {
//		direction = fScale < 0.1 ? 1 : -1;
//	}
//
//	fScale = direction > 0 ? fScale + 0.005f : fScale - 0.005f;

	//printf("dir=%d, scale=%f\n", direction, fScale);
}

int main(int argc, char *argv[])
{
    int rc;

    rc = initScreen();
    if (rc != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    // image library initialize error
//    rc = loadImage(ilib);
//    if (rc != EXIT_SUCCESS) {
//    	return EXIT_FAILURE;
//    }

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
