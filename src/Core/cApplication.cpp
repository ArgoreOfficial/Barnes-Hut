#include "cApplication.h"

#include <App/cSceneGame.h>

#include <Core/cWindow.h>
#include <Core/Renderer/cRenderer.h>

#include <iostream>
#include <fstream>
#include <string>

cApplication::cApplication() :
	m_window{ new cWindow() },
	m_renderer{ new cRenderer() },
	m_scene{ new cSceneGame() }
{
}

cApplication::~cApplication()
{
	m_window->destroy();
}

void cApplication::onCreate()
{
	m_window->create( 1500, 1000, "renderer idfk" );
	m_renderer->create( *m_window, cRenderer::eBackendType::OpenGL );

	m_scene->create();
}

void cApplication::onResize( int _width, int _height )
{
	m_renderer->onResize( _width, _height );
	m_window->onResize( _width, _height );
}

void cApplication::onRawInput( sInputInfo* _info )
{
	m_scene->onRawInput( _info );
}

void cApplication::run()
{
	double time = m_window->getTime();;
	double delta_time = 0.0;

	while ( !m_window->shouldClose() )
	{
		double now = m_window->getTime();
		delta_time = now - time;
		time = now;
		
		m_window->beginFrame();
		m_renderer->beginFrame();

		m_scene->update( delta_time );

		m_renderer->clear( 0x000000FF );

		m_scene->draw();

		m_renderer->endFrame();
		m_window->endFrame();
	}
}

std::string cApplication::loadShaderSource( const char* _path )
{
	std::string line, text;
	std::ifstream in( _path );

	while ( std::getline( in, line ) )
	{
		text += line + "\n";
	}
	
	return text;
}

