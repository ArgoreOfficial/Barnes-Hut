#pragma once

#include <Core/renderer/backends/iBackend.h>
#include <map>

class cBackend_OpenGL : public iBackend
{
public:
	 cBackend_OpenGL();
	~cBackend_OpenGL();

	void create( cWindow& _window ) override;
	void clear( unsigned int _color ) override;
	
	void onResize( int _width, int _height ) override;

	sShader createShader( const char* _source, eShaderType _type ) override;
	hShaderProgram createShaderProgram() override;
	sBuffer createBuffer( eBufferType _type ) override;
	hVertexArray createVertexArray() override;

	void attachShader( hShaderProgram& _program, sShader& _shader ) override;
	void linkShaderProgram( hShaderProgram& _program ) override;

	void useShaderProgram( hShaderProgram _program ) override;
	void bufferData( sBuffer& _buffer, void* _data, size_t _size ) override;

	void bindVertexLayout( cVertexLayout& _layout ) override;
	void bindVertexArray( hVertexArray _vertex_array ) override;

	void drawArrays( unsigned int _vertex_count, eDrawMode _mode ) override;
	void drawElements( unsigned int _index_count, eDrawMode _mode ) override;

	int getUniformLocation( hShaderProgram _shader, const char* _uniform ) override;
	void setUniformMat4f( int _location, float* _matrix_ptr ) override;
	void setUniformFloat( int _location, float _float ) override;
	void setUniformVec4f( int _location, wv::cVector4<float> _vector ) override;
	void setUniformVec4d( int _location, wv::cVector4<double> _vector ) override;

private:

};
