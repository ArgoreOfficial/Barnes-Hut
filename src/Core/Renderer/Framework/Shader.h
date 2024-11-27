#pragma once

/*
 *
 * Shader.h
 * 
 * Type definitions for shader handles
 * 
 */

typedef unsigned int hShader;
typedef unsigned int hShaderProgram;

enum eShaderType
{
	Shader_None = 0,
	Shader_Vertex, 
	Shader_Fragment,
	Shader_Geometry,
	Shader_Compute /* not implemented */
};

struct sShader
{
	const hShader handle;
	const eShaderType type;
};