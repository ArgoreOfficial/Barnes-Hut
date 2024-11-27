#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec4 color;
out float dist;

void main()
{
   vec4 pos = view * model * vec4(aPos, 1.0);

   color = aColor;
   dist = length( pos );
   
   gl_PointSize = 1;
   gl_Position = proj * pos;
}