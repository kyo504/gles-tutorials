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

static GLint colorLoc;
static GLint positionLoc;

GLint direction1 = 1;
GLint direction2 = 1;
GLint direction3 = 1;

GLfloat lLine[] = {
		// Line 0
		-0.3f, -0.5f, 0.0f,
		-0.3f, 0.0f, 0.0f,

		// Line 1
		-0.2f, 0.5f, 0.0f,
		-0.2f, 0.8f, 0.0f
};

GLfloat rLine[] = {
		// Line 0
		0.3f, -0.5f, 0.0f,
		0.3f, 0.0f, 0.0f,

		// Line 1
		0.2f, 0.5f, 0.0f,
		0.2f, 0.8f, 0.0f
};

GLfloat cLine[] = {
		// Line 0
		0.0f, -0.5f, 0.0f,
		0.0f, 0.0f, 0.0f,

		// Line 1
		0.0f, 0.5f, 0.0f,
		0.0f, 0.8f, 0.0f
};

GLfloat colorRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat colorGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };

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
    		"attribute vec4 a_position;                	\n"
    		"void main()                              \n"
    		"{                                        \n"
    		"   gl_Position = a_position;              \n"
    		"}                                        \n";

    char fShaderStr[] =
    		"precision mediump float;                        \n"
    		"uniform vec4 u_color;							\n"
    		"void main()                                     \n"
    		"{                                               \n"
    		"   gl_FragColor = u_color;  					\n"
    		"}                                               \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
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

    // Store the program object
    glUseProgram (programObject);

    positionLoc = glGetAttribLocation(programObject, "a_position");
    colorLoc = glGetUniformLocation(programObject, "u_color");

    // We don't need the shaders anymore
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

    return EXIT_SUCCESS;
}

void CalcBezierPoint(float t, float* p, float* p0, float* p1, float* p2, float* p3)
{
	float u = 1 - t;
	float tt = t*t;
	float uu = u*u;
	float uuu = uu*u;
	float ttt = tt*t;

	int i;
	for(i=0; i<3; i++){
		p[i] = uuu * p0[i];
		p[i] += 3 * uu * t * p1[i];
		p[i] += 3 * u * tt * p2[i];
		p[i] += ttt * p3[i];
	}
}

void drawParellelGuideline()
{
	GLfloat left2right[] =
	{
			-0.5f, -0.5f, 0.0f,
			-0.5f, 0.0f, 0.0f,

			0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.0f
	};

	GLfloat right2left[] =
	{
			0.5f, -0.5f, 0.0f,
			0.5f, 0.0f, 0.0f,

			0.0f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
	};

	GLfloat line0[] =
	{
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f
	};

	GLfloat line1[] =
	{
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f
	};

	float t = 0;
	int i=0, nCount=100;

	CalcBezierPoint(t, line0, &left2right[0], &left2right[3], &left2right[6], &left2right[9]);
	CalcBezierPoint(t, line1, &right2left[0], &right2left[3], &right2left[6], &right2left[9]);

	glLineWidth(2);

    glUniform4fv(colorLoc, 1, colorRed);

	for(i=0; i<=nCount; i++) {
		t = i/(float)nCount;
		CalcBezierPoint(t, &line0[3], &left2right[0], &left2right[3], &left2right[6], &left2right[9]);
		CalcBezierPoint(t, &line1[3], &right2left[0], &right2left[3], &right2left[6], &right2left[9]);

		glVertexAttribPointer (positionLoc, 3, GL_FLOAT, GL_FALSE, 0, line0);
		glDrawArrays ( GL_LINES, 0, 2 );

		line0[0] = line0[3];
		line0[1] = line0[4];

		glVertexAttribPointer (positionLoc, 3, GL_FLOAT, GL_FALSE, 0, line1);
		glDrawArrays ( GL_LINES, 0, 2 );

		line1[0] = line1[3];
		line1[1] = line1[4];
	}

	//glUniformMatrix4fv(colorLoc, 1, GL_FALSE, colorRed);
}

void render()
{
	EGLint surface_width;
	EGLint surface_height;

	eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &surface_width);
	eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &surface_height);

	// Set the viewport
	glClearColor ( 1.0f, 1.0f, 1.0f, 1.0f );
	glViewport ( 0, 0, surface_width, surface_height );

	// Clear the color buffer
	glClear ( GL_COLOR_BUFFER_BIT );

//	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, vTriangle);
	glEnableVertexAttribArray ( positionLoc );
//	glEnableVertexAttribArray ( colorLoc );
//	glDrawArrays ( GL_LINES, 0, 4 );

	drawParellelGuideline();

	GLfloat bLine1[] = {
			// q0
			0.0f, 0.0f, 0.0f,
			// q1
			0.0f, 0.0f, 0.0f
	};

	GLfloat bLine2[] = {
			// q0
			0.0f, 0.0f, 0.0f,
			// q1
			0.0f, 0.0f, 0.0f
	};

	GLfloat bLine3[] = {
			// q0
			0.0f, 0.0f, 0.0f,
			// q1
			0.0f, 0.0f, 0.0f
	};

	GLfloat hLine[] = {
			// q0
			0.0f, 0.0f, 0.0f,
			// q1
			0.0f, 0.0f, 0.0f
	};

	int nCount = 100;
	int i=0;
	float t1=0, t2=0, t3=0;

	glLineWidth(2);

    glUniform4fv(colorLoc, 1, colorGreen);

	CalcBezierPoint(t1, bLine1, &lLine[0], &lLine[3], &lLine[6], &lLine[9]);
	CalcBezierPoint(t2, bLine2, &rLine[0], &rLine[3], &rLine[6], &rLine[9]);
	CalcBezierPoint(t3, bLine3, &cLine[0], &cLine[3], &cLine[6], &cLine[9]);

	for(i = 1; i<=nCount; i++) {
		t1 = i / (float)nCount;
		t2 = i / (float)nCount;
		t3 = i / (float)nCount;
		CalcBezierPoint(t1, &bLine1[3], &lLine[0], &lLine[3], &lLine[6], &lLine[9]);
		CalcBezierPoint(t2, &bLine2[3], &rLine[0], &rLine[3], &rLine[6], &rLine[9]);
		CalcBezierPoint(t3, &bLine3[3], &cLine[0], &cLine[3], &cLine[6], &cLine[9]);

		glVertexAttribPointer (positionLoc, 3, GL_FLOAT, GL_FALSE, 0, bLine1);
		glDrawArrays ( GL_LINES, 0, 2 );

		bLine1[0] = bLine1[3];
		bLine1[1] = bLine1[4];

		glVertexAttribPointer (positionLoc, 3, GL_FLOAT, GL_FALSE, 0, bLine2);
		glDrawArrays ( GL_LINES, 0, 2 );

		bLine2[0] = bLine2[3];
		bLine2[1] = bLine2[4];

		glVertexAttribPointer (positionLoc, 3, GL_FLOAT, GL_FALSE, 0, bLine3);
		glDrawArrays ( GL_LINES, 0, 2 );

		bLine3[0] = bLine3[3];
		bLine3[1] = bLine3[4];

		if ( i%20 == 0 && i != nCount) {
			hLine[0] = bLine1[0];
			hLine[1] = bLine1[1];

			hLine[3] = bLine2[0];
			hLine[4] = bLine2[1];

			glVertexAttribPointer (positionLoc, 3, GL_FLOAT, GL_FALSE, 0, hLine);
			glDrawArrays ( GL_LINES, 0, 2 );
		}
	}

	if (direction1 > 0) {
		direction1 = lLine[9] > 0.2f ? -1 : 1;
	} else {
		direction1 = lLine[9] < -0.6f ? 1 : -1;
	}

	if (direction2 > 0) {
		direction2 = rLine[9] > 0.6f ? -1 : 1;
	} else {
		direction2 = rLine[9] < -0.2f ? 1 : -1;
	}

	if (direction3 > 0) {
		direction3 = cLine[9] > 0.4f ? -1 : 1;
	} else {
		direction3 = cLine[9] < -0.4f ? 1 : -1;
	}

	lLine[9] = direction1 > 0 ? lLine[9] + 0.005f : lLine[9] - 0.005f;
	rLine[9] = direction2 > 0 ? rLine[9] + 0.005f : rLine[9] - 0.005f;
	cLine[9] = direction3 > 0 ? cLine[9] + 0.005f : cLine[9] - 0.005f;

	glDisableVertexAttribArray(positionLoc);
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
