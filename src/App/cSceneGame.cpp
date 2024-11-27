#include "cSceneGame.h"

#include <Core/cApplication.h>
#include <Core/Renderer/Framework/cVertexLayout.h>
#include <Core/cWindow.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime>
#include <string>
#include <format>

#include <App/SpaceMath.h>



///////////////////////////////////////////////////////////////////////////////////////


#define THREAD_COUNT 16

#define BURNES_HUTT_THETA 1.0

#define OCTREE_SIZE 1.0e22


///////////////////////////////////////////////////////////////////////////////////////


#define RANDF( _min, _max ) _min + (float)( rand() ) / ( (float)( RAND_MAX / ( _max - _min ) ) )
#define RANDD( _min, _max ) _min + (double)( rand() ) / ( (double)( RAND_MAX / ( _max - _min ) ) )
#define BIG_G 6.67e-11

///////////////////////////////////////////////////////////////////////////////////////

double mapRange( double _x, double _min_in, double _max_in, double _min_out, double _max_out )
{
	double x = ( _x - _min_in ) / ( _max_in - _min_in );
	return _min_out + ( _max_out - _min_out ) * x;
}

///////////////////////////////////////////////////////////////////////////////////////

void cSceneGame::createGalaxy( wv::cVector3d _center, wv::cVector3d _velocity, unsigned int _stars, const double _size, const double _mass_min, const double _mass_max, const double _black_hole_mass )
{
	int index_offset = m_points.size();
	const double height = 5.0e18;

	for ( int i = 0; i < _stars; i++ )
	{
		double t = RANDD( 0, 3.1415 * 2.0 );
		double max_dist_temp = RANDD( _size * 0.05, _size );
		double min_dist = _black_hole_mass / max_dist_temp;
		double height_temp = RANDD( 0, height );
		double dist = RANDD( _size * 0.05, max_dist_temp );
		double s = sin( t ) * dist;
		double c = cos( t ) * dist;

		double mass = RANDD( _mass_min, _mass_max );
		double v = 0.0f;

		if ( i > 0 )
			v = sqrt( BIG_G * _black_hole_mass / dist );

		double s_prim = cos( t );
		double c_prim = -sin( t );

		/* position */
		double x = s; // RANDF( -max_dist, max_dist );
		double y = RANDD( -height_temp, height_temp );
		double z = c; // RANDF( -max_dist, max_dist );

		/* initial velocity */
		double vel_x = s_prim * v; // RANDF( -max_speed, max_speed );
		double vel_y = 0.0; // RANDF( -max_speed * 0.2, max_speed * 0.2 );
		double vel_z = c_prim * v; // RANDF( -max_speed, max_speed );

		/* star colour */
		double bv = mapRange( mass, _mass_min, _mass_max, 2.0, -0.4 );
		double l = mapRange( mass, _mass_min, _mass_max, 0.2, 0.8 );
		double r = 0, g = 0, b = 0;
		SpaceMath::bv2rgb( r, g, b, bv );

		sPoint point;
		point.position = wv::cVector3d{ x, y, z } + _center;
		point.velocity = wv::cVector3d{ vel_x, vel_y, vel_z } + _velocity;
		point.mass = mass;
		m_points.push_back( point );
		m_render_points.push_back( { m_points.back().position, wv::cVector4d{ r, g, b, l } });
	}

	m_points[ index_offset ].position = _center;
	m_points[ index_offset ].mass = _black_hole_mass;
	m_render_points[ index_offset ].position = m_points[ index_offset ].position;
	m_render_points[ index_offset ].color = { 0.0, 1.0, 0.0, 1.0 };
}

///////////////////////////////////////////////////////////////////////////////////////

void cSceneGame::create( void )
{
	srand( (int)time( 0 ) );

	cApplication& app = cApplication::getInstance();
	m_renderer = app.getRenderer();
	m_backend = m_renderer->getBackend();
	
	m_octree = new cOctree();
	m_octree->create( OCTREE_SIZE, BURNES_HUTT_THETA ); /* create space */

	/* note: keep above 2385km */
	/* no particular reason    */

	createGalaxy( { 1.0, 1.0, 1.0 }, { 500.0, 0.0, 0.0 }, 15000,
					7.1e20,        /* size */
					1.5911e29,     /* min mass */
					3.88147e32,    /* mass max */
					6.3e38 );      /* black hole mass */
	
	createGalaxy( { 4.0e21, 2.0e20, 4.0e20 }, { -500.0, 0.0, 0.0 }, 5000,
				  4.45e20,       /* size */
				  1.5911e29,     /* min mass */
				  3.38147e32,    /* mass max */
				  5.3e37 );      /* black hole mass */

	/* shader stuff */

	std::string vert = app.loadShaderSource( "../res/star.vert" );
	std::string frag = app.loadShaderSource( "../res/star.frag" );
	sShader vert_shader = m_renderer->createShader( vert.data(), eShaderType::Shader_Vertex );
	sShader frag_shader = m_renderer->createShader( frag.data(), eShaderType::Shader_Fragment );

	m_shader = m_backend->createShaderProgram();
	m_backend->attachShader( m_shader, vert_shader );
	m_backend->attachShader( m_shader, frag_shader );
	m_backend->linkShaderProgram( m_shader );

	/* create vertex array */
	m_vertex_array = m_backend->createVertexArray();
	m_backend->bindVertexArray( m_vertex_array );

	/* create vertex buffer */
	m_vertex_buffer = m_backend->createBuffer( eBufferType::Buffer_Vertex );
	m_backend->bufferData( m_vertex_buffer, m_points.data(), m_points.size() * sizeof( sPoint ) );

	cVertexLayout layout;
	layout.push<double>( 3 );
	layout.push<double>( 4 );

	m_backend->bindVertexLayout( layout );

	m_model_location = m_backend->getUniformLocation( m_shader, "model" );
	m_view_location = m_backend->getUniformLocation( m_shader, "view" );
	m_proj_location = m_backend->getUniformLocation( m_shader, "proj" );
	m_focal_length_location = m_backend->getUniformLocation( m_shader, "focal_length" );

	for ( int i = 0; i < m_points.size(); i++ )
		m_octree->addPoint( &m_points[ i ] );
	
	m_octree->recalculate();

	m_thread_pool.createWorkers( THREAD_COUNT, m_octree );
}

///////////////////////////////////////////////////////////////////////////////////////

void cSceneGame::destroy( void )
{
	delete m_octree;
	m_octree = nullptr;

	m_points.clear();
	m_render_points.clear();
}

#define KEY_RIGHT 262
#define KEY_LEFT 263
#define KEY_UP 265
#define KEY_DOWN 264

#define KEY_LEFT_SHIFT 340
#define KEY_LEFT_CONTROL 341
#define KEY_SPACE 32

void cSceneGame::onRawInput( sInputInfo* _info )
{
	int delta = _info->buttondown ? 1 : -1;

	if ( _info->buttondown || _info->repeat )
	{
		if ( _info->key == 'G' && !m_run )
			updateUniverse( 4.155e14 );
	}

	if ( _info->repeat )
		return;

	if ( _info->buttondown )
	{
		switch ( _info->key )
		{
		case KEY_SPACE: m_run   ^= 1; break;
		case 'R':       m_track ^= 1; break;
		case 'T':
			int next_display = m_display_mode + 1;
			m_display_mode = (eDisplayMode)( next_display % 3 );
			break;
		}
	}

	switch ( _info->key )
	{
	case KEY_RIGHT: m_input_yaw   +=  delta; break;
	case KEY_LEFT:  m_input_yaw   += -delta; break;
	case KEY_UP:    m_input_pitch += -delta; break;
	case KEY_DOWN:  m_input_pitch +=  delta; break;
	case KEY_LEFT_CONTROL: m_input_zoom -= delta; break;
	case KEY_LEFT_SHIFT: m_input_zoom += delta; break;

	case 'W': m_input_z += delta; m_track = false; break;
	case 'S': m_input_z -= delta; m_track = false; break;
	case 'A': m_input_x += delta; m_track = false; break;
	case 'D': m_input_x -= delta; m_track = false; break;
	case 'E': m_input_y -= delta; m_track = false; break;
	case 'Q': m_input_y += delta; m_track = false; break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void cSceneGame::updateUniverse( double _delta_time )
{
	if ( !m_thread_pool.doneWorking() )
		return;
	
	for ( int i = 0; i < m_points.size(); i++ )
	{
		m_points[ i ].last_position   = m_points[ i ].position;
		m_points[ i ].position       += m_points[ i ].velocity * _delta_time;

		m_render_points[ i ].position = m_points[ i ].position;
	}
		
	m_octree->recalculate();

	if ( m_track )
	{
		m_pos_x = (float)( -m_points[ 0 ].position.x * 3.5e-20 );
		m_pos_y = (float)( -m_points[ 0 ].position.y * 3.5e-20 );
		m_pos_z = (float)( -m_points[ 0 ].position.z * 3.5e-20 );
	}

	for ( int i = 0; i < m_points.size(); i++ )
		m_thread_pool.queueWork( &m_points[ i ], _delta_time );
	
}

///////////////////////////////////////////////////////////////////////////////////////

void cSceneGame::updateTitle( double _delta_time )
{
	cWindow& window = *cApplication::getInstance().getWindow();

	const char* display_mode_str = "NULL";
	switch ( m_display_mode )
	{
	case DisplayMode_Both:
		display_mode_str = "BOTH";
		break;
	case DisplayMode_Octree:
		display_mode_str = "OCTREE ONLY";
		break;
	case DisplayMode_Stars:
		display_mode_str = "STARS ONLY";
		break;
	}
	window.setTitle( std::format(
		"Multithreaded N-Body       Particles: {}       Display(T): {}       Track(R): {}       FPS: {}",
		std::to_string( m_points.size() ).c_str(),
		display_mode_str,
		( m_track?"TRUE":"FALSE" ),
		( 1.0 / _delta_time ) ).c_str() );
}

///////////////////////////////////////////////////////////////////////////////////////

void cSceneGame::update( double _delta_time )
{
	updateTitle( _delta_time );
	
	m_pitch += _delta_time * m_input_pitch;
	m_yaw += _delta_time * m_input_yaw;
	m_zoom += _delta_time * m_input_zoom * m_zoom_speed;

	float move_z = cos( m_yaw ) * m_input_z + sin( m_yaw ) * m_input_x;
	float move_x = -sin( m_yaw ) * m_input_z + cos( m_yaw ) * m_input_x;

	float zoom = pow( 1.1f, -m_zoom );
	float speed = zoom * 10.0f * _delta_time;

	m_pos_z += move_z * speed;
	m_pos_x += move_x * speed;
	m_pos_y += m_input_y * speed;

	if ( m_run )
		updateUniverse( 4.155e14 );

	/* buffer new data */
	m_backend->bufferData( m_vertex_buffer, m_render_points.data(), m_render_points.size() * sizeof( sRenderPoint ) );
}

///////////////////////////////////////////////////////////////////////////////////////

void cSceneGame::draw( void )
{
	const float FOV = 45.0f;
	cWindow& window = *cApplication::getInstance().getWindow();
	const double scale = 3.5e-20; /* a galaxy is very big */
	float aspect = window.getAspect();

	float zoom = pow( 1.1f, -m_zoom );

	m_focal_length = ( window.getWidth() / 2.0f ) / tan( FOV / 2.0f );

	/* create transforms */
	glm::mat4 model = glm::mat4( 1.0 );
	model = glm::scale( model, glm::vec3{ scale, scale, scale } );

	glm::mat4 view = glm::mat4( 1.0 );
	view = glm::translate( view, glm::vec3{ 0.0, 0.0, 30.0f * -zoom } );
	view = glm::rotate( view, m_pitch, glm::vec3{ 1.0f, 0.0f, 0.0f } );
	view = glm::rotate( view, m_yaw, glm::vec3{ 0.0f, 1.0f, 0.0f } );
	view = glm::translate( view, glm::vec3{ m_pos_x, m_pos_y, m_pos_z } );
	
	glm::mat4 projection = glm::perspective( glm::radians( 45.0f ), aspect, 0.0000001f, 10000.0f );

	/* draw octree */
	if ( m_display_mode != DisplayMode_Stars )
		m_octree->drawNodeTree( scale, view, projection );
	
	if ( m_display_mode == DisplayMode_Octree )
		return;

	/* apply transform uniforms */
	m_backend->useShaderProgram( m_shader );
	m_backend->setUniformMat4f( m_model_location, glm::value_ptr( model ) );
	m_backend->setUniformMat4f( m_view_location, glm::value_ptr( view ) );
	m_backend->setUniformMat4f( m_proj_location, glm::value_ptr( projection ) );
	m_backend->setUniformFloat( m_focal_length_location, m_focal_length );

	/* draw call */
	m_backend->bindVertexArray( m_vertex_array );
	m_backend->drawArrays( (unsigned int)m_points.size(), eDrawMode::DrawMode_Points );
	m_backend->bindVertexArray( 0 );

}
