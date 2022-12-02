#version 460

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{

	//...pode ter mais linhas de código aqui!
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
}