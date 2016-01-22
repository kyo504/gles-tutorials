## Draw a 3D Cube

이번 예제에서는 x,y,z 축과 축의 중심에 3D Cube를 그리게 됩니다. 이전 샘플을 확장하여 작성되었기 때문에 중복되는 부분은 이전 샘플 문서를 참고 부탁 드립니다.

### Vertex position과 color를 위한 Buffer 생성

먼저 살펴 볼 내용은 `initOpenGL` 함수에 추가 된 부분입니다.

```C++
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
```

코드에서 보는 것처럼 이번에는 shader source에 몇 줄이 포함 된 것을 확인할 수 있습니다. 이 코드들은 다음을 위함입니다.

- u_mvpMat : x, y 축을 기준으로 회전하여 cube의 모양을 정확히 보기 위한 변수
- a_color, v_color : cube의 각 면을 다른 색으로 drawing하기 위한 변수

`u_mvpMat` 변수의 경우 메인 코드에서 변환을 위한 매트릭스 연산의 결과 값을 shader에 전달하여 해당 변환을 표현하게 됩니다. `a_color` 는 사용자로부터 입력 받는 color 매트릭스 값이며 이 값이 v_color 값이 복사되며 이 값이 fragment shader로 전달됩니다. 그러면 아래 코드와 같이

```C++
    char fShaderStr[] =
    		"varying lowp vec4 v_color;                        \n"
    		"void main()                                     \n"
    		"{                                               \n"
    		"   gl_FragColor = v_color;                      \n"
    		"}                                               \n";
```  

별도의 변환 없이 전달된 값으로 fragment의 색상을 표현합니다.

다음으로는 추가된 것은 vertex position과 color를 위한 buffer를 생성하는 부분입니다.

이전 예제에서 보면 Vertex 정보들이 clint memory에 저장되어 있고 이 데이터는 glDrawArrays 나 glDrawElemets 함수가 호출 될 때 graphics 메모리로 복사 되어야 합니다. 하지만 매 draw가 발생할 때마다 vertex 정보를 복사하는 것 대신에 graphics 메모리에 cache 된 데이터를 사용한다면 렌더링 성능을 향상 시킬 수 있습니다. 이를 위해서 사용되는 것이 **vertex buffer object(VBO)** 입니다. 

```C++
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
```
VBO를 초기화하는 것은 위의 코드에서처럼 3단계로 나누어져 있습니다.

- glGenBuffers: buffer object 이름을 반환합니다. 일종의 식별자 역할을 함.
- glBindBuffers: Use a previously created buffer as the active VBO.
- glBufferData: 클라이언트 메모리에 있는 데이터를 VBO로 복사한다.

이렇게 복사된 데이터를 이용하면 client에서 graphics 메모리로 drawing 할 때마다 복사하는 번거러움을 줄일 수 있습니다.

### X,Y,Z 축 과 3D Cube 그리기

위에서 언급한 buffer를 이용해서 x,y,z 축을 그립니다.  

```C++
	glBindBuffer(GL_ARRAY_BUFFER, axisVertexID);
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, axisColorID);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
```

그리고 정면을 보게 되면 3D인지 알 수가 없으므로 아래와 같이 transformation과 projection을 적용합니다. 이에 대한 자세한 내용은 다음 샘플에서 다루도록 하겠습니다. 

```C++
	glExtRotateX(fRotateX, mRotX);
	glExtRotateY(fRotateY, mRotY);
	glExtMultiply(model, mRotX, mRotY);
	glExtFrustum(view, -2.0, 2.0, -2.0 * aspect, 2.0 * aspect, -2.0, 2.0);
	glExtMultiply(mvp, view, model);
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp);
```

한 가지 여기서 더 보고 갈 것은 shader 변수 중 uniform, attribute, varying 인데 이에 대한 정의는 아래와 같습니다.

**Uniform**
Application에서 OpenGL ES API를 통해 Shader로 전달되는 읽기 전용 값을 저장하는 변수. Uniform 변수는 Vertex Shader와 Fragment Shader에서 공유되며 주로 Matrix, Lighting Parameter, Color 등의 값을 저장하는데 사용된다.

```
uniform mat4 viewProjMatrix;
uniform mat4 viewMatrix;
uniform vec3 lightPosition;

glGetUniformLocation
glUniform*
```
 
**Attribute**
Vertex Shader에서만 사용가능한 타입으로 Vertex 각각의 정보를 전달하기 위해 사용된다. 일반적으로 Position, Normal, Texture Coordinate, Color 등의 정보가  전달된다.

```
attribute vec4 a_position;
attribute vec2 a_texCoord0;

glVertexAttribPointer
glBindAttribLocation
```

**Varying**
Vertex Shader의 Output이자 Fragment Shader의 Input으로 사용될 변수를 지정하는 데 사용된다. Application 쪽에선 건드릴 수 없는 변수이므로 관련 API 또한 존재하지 않는다.

```
varying vec2 texCoord;
varying vec4 color;
```
