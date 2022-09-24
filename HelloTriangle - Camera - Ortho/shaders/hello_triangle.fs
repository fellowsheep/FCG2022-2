#version 460

//uniform vec4 inputColor;
out vec4 color;

in vec4 vertexColor;

void main()
{

	color = vertexColor;

}