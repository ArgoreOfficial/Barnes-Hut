#include "cOctree.h"

#include <Core/cApplication.h>
#include <Core/Renderer/Framework/cVertexLayout.h>
#include <Core/cWindow.h>

#include <string>

#include <App/cNode.h>

cOctree::cOctree()
{
}

cOctree::~cOctree()
{
	delete m_root_node;
	m_root_node = nullptr;
}

void cOctree::create( const double& _size, const double& _theta )
{
	m_theta = _theta;

	cApplication& app = cApplication::getInstance();
	m_renderer = app.getRenderer();
	m_backend = m_renderer->getBackend();

	double vertices[] = {
		-0.5, 0.5,  0.5,     0.5, 0.5,  0.5, // top square
		 0.5, 0.5,  0.5,     0.5, 0.5, -0.5,
		 0.5, 0.5, -0.5,    -0.5, 0.5, -0.5,
		-0.5, 0.5, -0.5,    -0.5, 0.5,  0.5,

		-0.5, -0.5,  0.5,    0.5, -0.5,  0.5, // bottom square
		 0.5, -0.5,  0.5,    0.5, -0.5, -0.5,
		 0.5, -0.5, -0.5,   -0.5, -0.5, -0.5,
		-0.5, -0.5, -0.5,   -0.5, -0.5,  0.5,

		-0.5, -0.5,  0.5,   -0.5,  0.5,  0.5, // connecting edges
		 0.5, -0.5,  0.5,    0.5,  0.5,  0.5,
		 0.5, -0.5, -0.5,    0.5,  0.5, -0.5,
		-0.5, -0.5, -0.5,   -0.5,  0.5, -0.5
	};

	std::string vert = app.loadShaderSource( "../res/debug_lines.vert" );
	std::string frag = app.loadShaderSource( "../res/debug_lines.frag" );
	sShader vert_shader = m_renderer->createShader( vert.data(), eShaderType::Shader_Vertex );
	sShader frag_shader = m_renderer->createShader( frag.data(), eShaderType::Shader_Fragment );

	m_box_shader = m_backend->createShaderProgram();
	m_backend->attachShader( m_box_shader, vert_shader );
	m_backend->attachShader( m_box_shader, frag_shader );
	m_backend->linkShaderProgram( m_box_shader );
	m_size = _size;

	/* create vertex array */
	m_debug_box_vao = m_backend->createVertexArray();
	m_backend->bindVertexArray( m_debug_box_vao );

	/* create vertex buffer */
	m_debug_box_vbo = m_backend->createBuffer( eBufferType::Buffer_Vertex );
	m_backend->bufferData( m_debug_box_vbo, vertices, sizeof( vertices ) );

	cVertexLayout layout;
	layout.push<double>( 3 );

	m_backend->bindVertexLayout( layout );

	m_model_location = m_backend->getUniformLocation( m_box_shader, "model" );
	m_view_location = m_backend->getUniformLocation( m_box_shader, "view" );
	m_proj_location = m_backend->getUniformLocation( m_box_shader, "proj" );
	m_color_location = m_backend->getUniformLocation( m_box_shader, "uColor" );

	m_root_node = new cNode( nullptr, m_size );
}

cNode* cOctree::addPoint( sPoint* _point )
{
	cNode* node = m_root_node->findEmptyNode( _point->position );
	
	if ( !node )
		return nullptr;

	while ( !node->is_leaf || node->data )
		node = node->findEmptyNode( _point->position );
	
	node->data = _point;
	node->accumulatePointMass();
	return node;
}

void cOctree::drawNodeTree( double _scale, glm::mat4& _view, glm::mat4& _proj )
{
	cWindow& window = *cApplication::getInstance().getWindow();
	const float aspect = window.getAspect();

	m_backend->useShaderProgram( m_box_shader );
	m_backend->setUniformMat4f( m_view_location, glm::value_ptr( _view ) );
	m_backend->setUniformMat4f( m_proj_location, glm::value_ptr( _proj ) );
	m_backend->bindVertexArray( m_debug_box_vao );
	
	m_backend->setUniformVec4f( m_color_location, wv::cVector4f( 0.0f, 1.0f, 0.0f, 0.05f ) );
	m_root_node->draw( m_backend, m_model_location, _scale );

	m_backend->setUniformVec4f( m_color_location, wv::cVector4f( 1.0f, 0.0f, 0.0f, 0.05f ) );
	m_root_node->drawCOM( m_backend, m_model_location, _scale );
	
	m_backend->bindVertexArray( 0 );
}

void cOctree::recalculate()
{
	m_root_node->recalculateData();
	m_root_node->recalculateMass( nullptr, nullptr );
}

wv::cVector3d cOctree::computeForces( sPoint& _point )
{
	return m_root_node->computeForces( &_point, m_theta );
}
