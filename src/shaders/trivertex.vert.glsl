#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 vertexUVs;

out vec2 fragUVs;
void main()
{
	fragUVs = vertexUVs;
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
