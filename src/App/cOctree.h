#pragma once

#include <Core/Renderer/Backends/iBackend.h>
#include <Core/Renderer/cRenderer.h>
#include <Core/Math/Vector3.h>
#include <Core/Renderer/Framework/Shader.h>
#include <Core/Renderer/Framework/Buffer.h>
#include <Core/Renderer/Framework/VertexArray.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <App/sPoint.h>

#include "cThreadPool.h"

class cNode;

class cOctree
{
public:
	 cOctree();
	~cOctree();

	void create( const double& _size, const double& _theta );
	cNode* addPoint( sPoint* _point );
	void drawNodeTree( double _scale, glm::mat4& _view, glm::mat4& _proj );
	void recalculate();

	wv::cVector3d computeForces( sPoint& _point );

private:
	
	cRenderer* m_renderer;
	iBackend* m_backend;

	cNode* m_root_node = nullptr;

	hShaderProgram m_box_shader;

	hVertexArray m_debug_box_vao;
	sBuffer m_debug_box_vbo;

	int m_model_location = -1;
	int m_view_location  = -1;
	int m_proj_location  = -1;
	int m_color_location = -1;
	
	double m_size = 1.0;
	double m_theta = 1.0;
};