#version 330 core

layout(location = 0) in vec3 aPos;

layout(location = 1) in vec3 vertexNormal;

layout(location = 2) in vec2 aTex;

//uniform float x, y, z;

uniform mat4 transform;

uniform mat4 projection;

uniform mat4 view;


out vec2 texCoord;

out vec3 normCoord;

out vec3 fragPos;


void main(){

	//vec3 newPos = vec3(aPos.x + x, aPos.y + y, aPos.z + z);
	//gl_Position = vec4(newPos, 1.0);

	normCoord = mat3(transpose(inverse(transform))) * vertexNormal;


	fragPos = vec3(transform * vec4(aPos,1.0));

	gl_Position = projection * view * transform * vec4(aPos, 1.0);

	texCoord = aTex;
}