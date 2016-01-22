precision mediump float;
attribute vec4 a_position;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main()
{
	gl_Position = u_projection * u_view * u_model * a_position;
}

