## Draw a triangle

첫 번째 예제는 EGL surface위에 default primitive인 삼각형을 그리는 예제입니다. 여기서는 단순히 삼각형을 그리는 것 외에 삼각형을 그리기 위한 준비 작업에는 어떤 것들이 있는지 살펴봅니다.

OpenGL ES(이하 gles)을 이용해서 삼각형을 그리려면 이를 그리기 위한 일종의 캔버스가 준비되어야 합니다. 이를 위해서는 선행되어야 하는 작업들이 있습니다.

- Screen을 이용하여 윈도우를 생성
- EGL을 이용하여 surface 생성

각각의 작업들을 살펴보면 다음과 같습니다.

### Screen을 이용한 윈도우 생성

QNX에서 window를 다루는 방법은 몇 가지가 있지만 그 중에 하나는 Screen이라는 Grahics sub-system을 이용하는 것입니다. 먼저 screen을 위한 context를 생성하고 해당 context를 이용해서 window를 하나 생성합니다. 이렇게 생성한 윈도우에 속성을 설정하는데 이 샘플에서는 color format과 opengl es 버전을 설정합니다.

```C++
screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_FORMAT, (const int[]){ SCREEN_FORMAT_RGBX8888 });
screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, (const int[]){ SCREEN_USAGE_OPENGL_ES2 });
```

Window 속성을 설정했다면 마지막으로 더블 버퍼링을 위한 윈도우 버퍼를 생성합니다.

```C++
screen_create_window_buffers(screen_win, 2);
```

### EGL Surface 생성

이제 생성된 window에 EGL surface를 연결하는 작업이 필요합니다. 이를 위해서는 다음의 작업들이 선행됩니다.

- 현재 연결된 display 정보 획득 및 display 초기화
- 주어진 속성들과 매칭되는 EGL frame buffer 설정을 획득
- egl context 생성
- screen 윈도우로 부터 egl surface 생성
- EGL rendering cotext를 현재 surface에 연결

여기까지 완료하였다면 그림을 그리기 위한 화면 설정은 완료되었다고 할 수 있습니다. 다음으로 진행해야 할 것은 vertex shader와 fragment shader 관련된 설정입니다. 

### Vertex / fragment shader 설정

vertex shader와 fragment shader가 생소하다면 아래 링크의 내용을 한번 살펴보시기 바랍니다. (http://ptgmedia.pearsoncmg.com/images/chap1_9780321933881/elementLinks/01fig01_alt.jpg)

vs와 fs의 설정은 다음의 과정을 통해 이루어집니다.

- Shader의 생성 및 컴파일
- Program의 생성 및 링킹

#### Shader의 생성 및 컴파일

첫 번째 단계는 `glCreateShader` 함수를 통해서 shader 객체를 생성하는 것입니다. 객체가 정상적으로 생성되었다면 `glShaderSource` 함수를 이용해서 실제 shader 코드를 넘겨줍니다. 마지막으로 `glCompileShader` 함수를 호출하면 shader source를 넘겨받은 object가 compile 됩니다.

```C++
GLuint LoadShader ( EGLenum type, const char *shaderSrc )
{
   GLuint shader = 0;
   GLint compiled;

   // Create the shader object
   shader = glCreateShader ( type );

   // Load the shader source
   glShaderSource( shader, 1, &shaderSrc, 0 );

   // Compile the shader
   glCompileShader( shader );

   ...

   return shader;
}
``` 

### 삼각형 그리기 

마지막으로 할 일은 화면이 실제로 삼각형을 그리는 것입니다. 이는 샘플의 `render` 함수에서 이루어지는데 호출 순서를 보면 다음과 같습니다.

#### viewport 설정

사용자가 임의로 실제로 표시될 화면 영역을 설정합니다. 만약 호출하지 않는다면 기본적으로 현재 디스플레이가 가지고 있는 정보를 viewport로 사용합니다.
```C++
	// Set the viewport
	glViewport ( 0, 0, surface_width, surface_height );
```
#### color buffer 초기화

`glClear` 함수를 호출함으로써 현재 화면을 `glClearColor`로 설정된 색상으로 칠한다. 

#### vertex 정보를 로딩

그림을 그리기 위해서는 vertex 정보를 읽어서 shader에 전달해 줘야 합니다. 이 예제에서는 아래와 같은 vertex shader source가 있습니다.

```
    char vShaderStr[] =
    		"attribute vec4 vPosition;                \n"
    		"void main()                              \n"
    		"{                                        \n"
    		"   gl_Position = vPosition;              \n"
    		"}                                        \n";
```

여기에서 vPosition에 삼각형의 vertex 정보를 읽어서 연결해 주어야 하는데 이 동작을 하는 함수가 `glVertexAttribPointer` 입니다. 

```C++
glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, vTriangle);
```

첫 번째 매개변수의 `0`이 vPosition을 의미하며 마지막 매개변수인 vTriangle과 연결됨을 알 수 있습니다.


> *NOTE :*
> 어떻게 0이 vPosition이 되는 것일까요? 그 답은 다음의 링크에서 확인할 수 있습니다. (https://www.opengl.org/wiki/Vertex_Shader#Inputs)
 
> 요약하자면 shader 안에서 혹은 gles API를 통해 명시적으로 사용자 입력(여기서는 삼각형 vertex 정보)가 binding 되지 않는다면 index가 OpenGL ES에 의해서 자동으로 할당됩니다. 그러므로 실제로 코드 작성 시에는 명시적으로 연결을 해 주는 것이 좋습니다. 이 부분은 다른 샘플에서 확인하실 수 있습니다.
    

#### Drawing 함수 호출

삼각형을 그리는 마지막 단계로는 실제로 OpenGL ES에게 해당 삼각형을 그리도록 `glDrawArrays` 함수를 호출하는 것입니다.
