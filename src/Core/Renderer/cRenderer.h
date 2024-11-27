#pragma once

class iBackend;
class iShader;
class cWindow;
class cPointCloud;

#include <Core/Renderer/Framework/Shader.h>
#include <Core/Renderer/Framework/Buffer.h>

class cRenderer 
{
public:
	enum eBackendType
	{
		OpenGL,
		Direct3D11
	};

	 cRenderer( void );
	~cRenderer( void );
	
	void create( cWindow& _window, cRenderer::eBackendType _backend );
	sShader createShader( const char* _source, eShaderType _type );

	void onResize( int _width, int _height );
	void clear( unsigned int _color );

	void beginFrame( void );
	void endFrame  ( void );
	
	hShaderProgram getDefaultShader( void ) const { return m_shader_default; }
	iBackend* getBackend( void ) { return m_backend; }
	
private:

	void createDefaultShader( void );

	iBackend* m_backend = nullptr;

	const char* m_shader_default_vert =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"uniform mat4 proj;\n"
		"uniform mat4 view;\n"
		"uniform mat4 model;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = proj * view * model * vec4(aPos, 1.0);\n"
		"}\0";

	const char* m_shader_default_frag =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f );\n"
		"}\0";
	
	hShaderProgram m_shader_default;
};
