#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texcoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec4 vertexColor;
out vec2 texCoord;

void main()
{

	//...pode ter mais linhas de código aqui!
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	vertexColor = vec4(color,1.0);
	texCoord = vec2(texcoord.x, 1 - texcoord.y);
}