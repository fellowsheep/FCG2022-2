#version 460

//uniform vec4 inputColor;
out vec4 color;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D texBuffer;

void main()
{

	color = texture(texBuffer,texCoord);//vertexColor;

}