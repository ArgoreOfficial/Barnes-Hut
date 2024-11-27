#include "cBackend_OpenGL.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h> /* temporary */

#include <Core/cWindow.h>
#include <Core/Renderer/Framework/cVertexLayout.h>

#include <stdio.h>


cBackend_OpenGL::cBackend_OpenGL()
{

}

cBackend_OpenGL::~cBackend_OpenGL()
{

}

void cBackend_OpenGL::create( cWindow& _window )
{
	if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
	{
		fprintf( stderr, "Failed to initialize GLAD\n" );
		return;
	}
	glViewport( 0, 0, _window.getWidth(), _window.getHeight() );

	const GLubyte* renderer = glGetString( GL_RENDERER );
	const GLubyte* vendor = glGetString( GL_VENDOR );
	const GLubyte* version = glGetString( GL_VERSION );
	const GLubyte* glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

	GLint major, minor;
	glGetIntegerv( GL_MAJOR_VERSION, &major );
	glGetIntegerv( GL_MINOR_VERSION, &minor );

	printf( "Renderer Backend: OpenGL\n" );
	printf( "   Vendor            : %s\n", vendor );
	printf( "   Renderer          : %s\n", renderer );
	printf( "   Version (string)  : %s\n", version );
	printf( "   Version (integer) : %d.%d\n", major, minor );
	printf( "   GLSL Version      : %s\n", glslVersion );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_PROGRAM_POINT_SIZE );
}

void cBackend_OpenGL::clear( unsigned int _color )
{
	float r = ( _color & 0xFF000000 ) / 256.0f;
	float g = ( _color & 0x00FF0000 ) / 256.0f;
	float b = ( _color & 0x0000FF00 ) / 256.0f;
	float a = ( _color & 0x000000FF ) / 256.0f;

	glClearColor( r, g, b, a );
	glClear( GL_COLOR_BUFFER_BIT );
}

void cBackend_OpenGL::onResize( int _width, int _height )
{
	glViewport( 0, 0, _width, _height );
}

sShader cBackend_OpenGL::createShader( const char* _source, eShaderType _type )
{
	int  success;
	char infoLog[ 512 ];
	unsigned int shader;

	switch ( _type )
	{
	case Shader_Vertex:
		shader = glCreateShader( GL_VERTEX_SHADER );
		break;
	case Shader_Fragment:
		shader = glCreateShader( GL_FRAGMENT_SHADER );
		break;
	case Shader_Geometry:
		shader = glCreateShader( GL_GEOMETRY_SHADER );
		break;
	default:
		return { 0, eShaderType::Shader_None };
		break;
	}

	glShaderSource( shader, 1, &_source, NULL );
	glCompileShader( shader );

	glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( shader, 512, NULL, infoLog );
		printf( "ERROR::SHADER::COMPILATION_FAILED\n %s \n", infoLog );
	}

	return { shader, _type };
}

hShaderProgram cBackend_OpenGL::createShaderProgram()
{
	hShaderProgram program = glCreateProgram();

	return program;
}

int getBufferTarget_OpenGL( eBufferType _type )
{
	switch ( _type )
	{
	case eBufferType::Buffer_Vertex:
		return GL_ARRAY_BUFFER;
		break;
	case eBufferType::Buffer_Index:
		return GL_ELEMENT_ARRAY_BUFFER;
		break;
	}

	return GL_NONE;
}

sBuffer cBackend_OpenGL::createBuffer( eBufferType _type )
{
	hBuffer buffer = 0;
	glGenBuffers( 1, &buffer );
	int target = getBufferTarget_OpenGL( _type );
	if( !target )
		return { 0, eBufferType::Buffer_None };

	glBindBuffer( target, buffer );

	return { buffer, _type };
}

hVertexArray cBackend_OpenGL::createVertexArray()
{
	hVertexArray vertex_array;
	glGenVertexArrays( 1, &vertex_array );
	return vertex_array;
}

void cBackend_OpenGL::attachShader( hShaderProgram& _program, sShader& _shader )
{
	unsigned int frag = _shader.handle;
	glAttachShader( _program, frag );
}

void cBackend_OpenGL::linkShaderProgram( hShaderProgram& _program )
{
	int  success;
	char infoLog[ 512 ];

	glLinkProgram( _program );

	glGetProgramiv( _program, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( _program, 512, NULL, infoLog );
		printf( "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n %s \n", infoLog );
	}
}

void cBackend_OpenGL::useShaderProgram( hShaderProgram _program )
{
	glUseProgram( _program );
}

void cBackend_OpenGL::bufferData( sBuffer& _buffer, void* _data, size_t _size )
{
	int target = getBufferTarget_OpenGL( _buffer.type );
	if ( !target )
		return;

	glBufferData( target, _size, _data, GL_DYNAMIC_DRAW ); /* move usage to buffer object */
	
}

void cBackend_OpenGL::bindVertexLayout( cVertexLayout& _layout )
{
	const auto& elements = _layout.getElements();
	unsigned int offset = 0;

	for ( unsigned int i = 0; i < elements.size(); i++ )
	{
		const auto& element = elements[ i ];

		GLenum type = GL_UNSIGNED_BYTE;

		switch ( element.type )
		{
		case cVertexLayout::Float:
			type = GL_FLOAT;
			break;
		case cVertexLayout::Uint:
			type = GL_UNSIGNED_INT;
			break;
		case cVertexLayout::Byte:
			type = GL_UNSIGNED_BYTE;
			break;
		case cVertexLayout::Double:
			type = GL_DOUBLE;
			break;
		}

		glEnableVertexAttribArray( i );
		glVertexAttribPointer( i, element.count, type, false, _layout.getStride(), (const void*)offset );
		offset += element.count * (unsigned int)element.size;
	}
}

void cBackend_OpenGL::bindVertexArray( hVertexArray _vertex_array )
{
	glBindVertexArray( _vertex_array );
}

int getPrimitive_OpenGL( eDrawMode _mode )
{
	switch ( _mode )
	{
	case eDrawMode::DrawMode_Lines:
		return GL_LINES;
		break;
	case eDrawMode::DrawMode_Points:
		return GL_POINTS;
		break;
	case eDrawMode::DrawMode_Triangle:
		return GL_TRIANGLES;
		break;
	}

	return GL_LINES;
}

void cBackend_OpenGL::drawArrays( unsigned int _vertex_count, eDrawMode _mode )
{
	glDrawArrays( getPrimitive_OpenGL( _mode ), 0, _vertex_count);
}

void cBackend_OpenGL::drawElements( unsigned int _index_count, eDrawMode _mode )
{
	glDrawElements( getPrimitive_OpenGL( _mode ), _index_count, GL_UNSIGNED_INT, 0 );
}

int cBackend_OpenGL::getUniformLocation( hShaderProgram _shader, const char* _uniform )
{
	return glGetUniformLocation( _shader, _uniform );;
}

void cBackend_OpenGL::setUniformMat4f( int _location, float* _matrix_ptr )
{
	glUniformMatrix4fv( _location, 1, GL_FALSE, _matrix_ptr );
}

void cBackend_OpenGL::setUniformFloat( int _location, float _float )
{
	glUniform1f( _location, _float );
}

void cBackend_OpenGL::setUniformVec4f( int _location, wv::cVector4<float> _vector )
{
	glUniform4f( _location, _vector.x, _vector.y, _vector.z, _vector.w );
}

void cBackend_OpenGL::setUniformVec4d( int _location, wv::cVector4<double> _vector )
{
	glUniform4d( _location, _vector.x, _vector.y, _vector.z, _vector.w );
}
