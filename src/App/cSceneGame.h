#pragma once

#include <Core/Scene/iScene.h>

#include <Core/Renderer/Backends/iBackend.h>
#include <Core/Renderer/cRenderer.h>
#include <Core/Math/Vector2.h>
#include <vector>

#include <App/cOctree.h>

#include <App/cThreadPool.h>

struct sInputInfo;

enum eDisplayMode
{
	DisplayMode_Stars,
	DisplayMode_Octree,
	DisplayMode_Both
};

class cSceneGame : public iScene
{
public:
	 cSceneGame( void ) { };
	~cSceneGame( void ) { };

	void createGalaxy( wv::cVector3d _center, wv::cVector3d _velocity, unsigned int _stars, const double _size, const double _mass_min, const double _mass_max, const double _black_hole_mass );

	void create( void ) override;
	void destroy( void ) override;

	void onRawInput( sInputInfo* _info ) override;

	void updateUniverse( double _delta_time );
	void updateTitle( double _delta_time );

	void update( double _delta_time ) override;

	void draw( void ) override;

private:

	cRenderer* m_renderer;
	iBackend* m_backend;

	int m_model_location;
	int m_view_location;
	int m_proj_location;
	int m_focal_length_location;

	hShaderProgram m_shader;

	hVertexArray m_vertex_array;
	sBuffer m_vertex_buffer;

	cOctree* m_octree = nullptr;
	cThreadPool m_thread_pool;

	float m_focal_length = 1.0f;

	float m_pitch = 0.0f;
	float m_yaw   = 0.0f;
	float m_zoom  = 25.0f;
	float m_zoom_speed = 20.0f;

	float m_pos_x = 0.0f;
	float m_pos_z = 0.0f;
	float m_pos_y = 0.0f;

	int m_input_pitch = 0;
	int m_input_yaw   = 0;
	int m_input_zoom  = 0;

	int m_input_x = 0;
	int m_input_z = 0;
	int m_input_y = 0;
	
	bool m_run = false;
	bool m_track = false;
	
	eDisplayMode m_display_mode = DisplayMode_Stars;

	std::vector<sPoint> m_points;
	std::vector<sRenderPoint> m_render_points;
};
