#pragma once

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

class cWindow
{
public:
	 cWindow();
	~cWindow();

	int  create ( unsigned int _width, unsigned int _height, const char* _title );
	void destroy( void );

	void beginFrame( void );
	void endFrame  ( void );

	bool shouldClose( void ) const { return glfwWindowShouldClose( m_window_object ); }

	unsigned int getWidth() { return m_width; }
	unsigned int getHeight() { return m_height; }
	float getAspect();
	
	void onResize( int _width, int _height );

	GLFWwindow* const getWindowObject( void ) { return m_window_object; }
	unsigned int getWidth ( void ) const { return m_width; }
	unsigned int getHeight( void ) const { return m_height; }
	double getTime();
	void setTitle( const char* _title );
private:

	GLFWwindow* m_window_object = nullptr;
	unsigned int m_width = 0;
	unsigned int m_height = 0;

};
