#include "cWindow.h"

#include <Core/cApplication.h>

#include <stdio.h>

void keyCallback( GLFWwindow* _window, int _key, int _scancode, int _action, int _mods )
{
	sInputInfo* info = new sInputInfo();
	info->buttondown = _action == GLFW_PRESS;
	info->buttonup = _action == GLFW_RELEASE;
	info->repeat = _action == GLFW_REPEAT;
	info->key = _key;
	info->scancode = _scancode;
	info->mods = _mods;

	cApplication::getInstance().onRawInput( info );

	delete info;
}

void onResizeCallback( GLFWwindow* window, int _width, int _height )
{
	cApplication::getInstance().onResize( _width, _height );
}

void processInput( GLFWwindow* window )
{
	if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
		glfwSetWindowShouldClose( window, true );
}


cWindow::cWindow()
{

}

cWindow::~cWindow()
{

}

int cWindow::create( unsigned int _width, unsigned int _height, const char* _title )
{

	if ( !glfwInit() )
	{
		fprintf( stderr, "Failed to init GLFW\n" );
		return 1;
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	printf( "GLFW version: %s\n", glfwGetVersionString() );

	m_window_object = glfwCreateWindow( _width, _height, _title, NULL, NULL );
	if ( !m_window_object )
	{
		fprintf( stderr, "Failed to create GLFW window\n" );
		return 1;
	}
	glfwMakeContextCurrent( m_window_object );

	glfwSwapInterval( 0 );

	glfwSetFramebufferSizeCallback( m_window_object, onResizeCallback );
	glfwSetKeyCallback( m_window_object, keyCallback );
	m_width = _width;
	m_height = _height;

	return 0;
}

void cWindow::destroy( void )
{
	glfwTerminate();
}

void cWindow::beginFrame( void )
{
	processInput( m_window_object );
	
}

void cWindow::endFrame( void )
{
	glfwSwapBuffers( m_window_object );
	glfwPollEvents();
}

float cWindow::getAspect()
{
	if ( m_width == 0 || m_height == 0 )
		return 1.0f;

	return (float)m_width / (float)m_height;
}

void cWindow::onResize( int _width, int _height )
{
	m_width = _width;
	m_height = _height;
}

double cWindow::getTime()
{
    return glfwGetTime();
}

void cWindow::setTitle( const char* _title )
{
	glfwSetWindowTitle( m_window_object, _title );
}
