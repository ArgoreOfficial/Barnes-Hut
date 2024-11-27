#include "cNode.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Core/Renderer/Backends/iBackend.h>
#include <Core/Renderer/cRenderer.h>
#include <App/sPoint.h>

#include <App/SpaceMath.h>

cNode::~cNode()
{
	parent = nullptr;
	data = nullptr;
	
	for ( int i = 0; i < 8; i++ )
	{
		if ( children[ i ] )
		{
			delete children[ i ];
			children[ i ] = nullptr;
		}
	}


}

unsigned char cNode::getNodeAtPoint( const wv::cVector3d& _position )
{
	unsigned char x = ( _position.x > center.x ) * 0b0100;
	unsigned char y = ( _position.y > center.y ) * 0b0010;
	unsigned char z = ( _position.z > center.z ) * 0b0001;

	return x | y | z;
}

cNode* cNode::findEmptyNode( const wv::cVector3d& _position )
{
	if ( !containsPoint( _position ) )
	{
		if ( !parent )
			return nullptr;

		return parent; // ->findEmptyNode( _position );
	}

	if ( !data && is_leaf )
		return this;

	unsigned char node_id = getNodeAtPoint( _position );
	
	if ( is_leaf )
		split();

	return children[ (int)node_id ]; // ->findEmptyNode( _position );
}

bool cNode::containsPoint( const wv::cVector3d& _position )
{
	double hsize = size / 2.0;
	bool x = _position.x <= ( center.x + hsize ) && _position.x >= ( center.x - hsize );
	bool y = _position.y <= ( center.y + hsize ) && _position.y >= ( center.y - hsize );
	bool z = _position.z <= ( center.z + hsize ) && _position.z >= ( center.z - hsize );
	return x && y && z;
}

void cNode::split()
{
	if ( !is_leaf )
		return;

	is_leaf = false;
	double p = size / 4.0;

	for ( int i = 0; i < 8; i++ )
		children[ i ] = new cNode( this, size / 2.0f );

	children[ 0 ]->center = center + wv::cVector3d{ -p, -p, -p };
	children[ 1 ]->center = center + wv::cVector3d{ -p, -p,  p };
	children[ 2 ]->center = center + wv::cVector3d{ -p,  p, -p };
	children[ 3 ]->center = center + wv::cVector3d{ -p,  p,  p };
	children[ 4 ]->center = center + wv::cVector3d{  p, -p, -p };
	children[ 5 ]->center = center + wv::cVector3d{  p, -p,  p };
	children[ 6 ]->center = center + wv::cVector3d{  p,  p, -p };
	children[ 7 ]->center = center + wv::cVector3d{  p,  p,  p };

	if ( data )
	{
		cNode* node = children[ (int)getNodeAtPoint( data->position ) ];
		node->data = data;
		data = nullptr;
	}
}

void cNode::checkParticleExited()
{
	if ( data && !containsPoint( data->position ) )
	{
		if ( !parent )
			return;

		cNode* node = parent->findEmptyNode( data->position );
		while ( node && ( !node->is_leaf || node->data ) )
			node = node->findEmptyNode( data->position );

		if ( node )
			node->data = data;
		
		data = nullptr;
		is_leaf = true;
	}
}

void cNode::clearChildren()
{

	int empty_children = 0;

	for ( int i = 0; i < 8; i++ )
	{
		if ( children[ i ]->data == nullptr && children[ i ]->is_leaf )
			empty_children++;

		if ( children[ i ]->data )
		{
			com += children[ i ]->data->position * children[ i ]->data->mass;
			mass += children[ i ]->data->mass;
		}
	}

	if ( empty_children == 8 )
	{
		is_leaf = true;
		for ( int i = 0; i < 8; i++ )
		{
			delete children[ i ];
			children[ i ] = nullptr;
		}
	}
}

/*
* 
* why it doesn't work / what needs to be fixed:
* 
* node mass and com is reset, but that isn't always in order
* which means we get masses like
* 
* root:5 -> node:4 -> node:0 -> node:3 -> leaf:0
* very brocken
* 
*/



void cNode::recalculateData()
{
	com = wv::cVector3d( 0.0, 0.0, 0.0 );
	mass = 0.0;

	checkParticleExited();

	if ( !is_leaf )
	{
		for ( int i = 0; i < 8; i++ )
			children[ i ]->recalculateData();

		clearChildren();
	}
	
	if ( data )
		accumulatePointMass();
	
}

void cNode::accumulatePointMass()
{
	cNode* node = this;

	while ( node )
	{
		node->mass += data->mass;
		node->com += data->position * data->mass;
		node->com /= node->mass;

		node = node->parent;
	}
}

void cNode::recalculateMass( double* _out_mass, wv::cVector3d* _out_com )
{
	mass = 0.0;
	com.x = 0.0; 
	com.y = 0.0;
	com.z = 0.0;

	if ( !is_leaf )
	{
		for ( int i = 0; i < 8; i++ )
		{
			children[ i ]->recalculateMass( &mass, &com );
			mass += children[ i ]->mass;
			com += children[ i ]->com * children[ i ]->mass;
		}
	}
	
	if ( data )
	{
		mass += data->mass;
		com += data->position * data->mass;
	}

	if( mass > 0 )
		com /= mass;
}

wv::cVector3d cNode::computeForces( sPoint* _point, const float& _theta )
{
	if ( data )
	{
		if ( _point == data )
			return { 0.0, 0.0, 0.0 };

		return SpaceMath::computeForce(_point->position, data->position, _point->mass, data->mass);
	}

	if ( is_leaf)
		return { 0.0, 0.0, 0.0 };

	double r = ( _point->position - com ).length();
	double theta = size / r;
	if ( theta <= _theta )
	{
		return SpaceMath::computeForce( _point->position, com, _point->mass, mass );
	}

	wv::cVector3d accumulated;

	for ( int i = 0; i < 8; i++ )
		accumulated += children[ i ]->computeForces( _point, _theta );
	
    return accumulated;
}

void cNode::draw( iBackend* _backend, int& _model_loc, double _scale )
{
	if ( data )
	{
		glm::mat4 model = glm::mat4( 1.0f );
		model = glm::translate( model, glm::vec3( center.x * _scale, center.y * _scale, center.z * _scale ) );
		model = glm::scale( model, glm::vec3{ size * _scale, size * _scale, size * _scale } );

		_backend->setUniformMat4f( _model_loc, glm::value_ptr( model ) );
		_backend->drawArrays( 24, eDrawMode::DrawMode_Lines );
	}

	if ( is_leaf )
		return;

	for ( int i = 0; i < 8; i++ )
		if ( children[ i ] )
			children[ i ]->draw( _backend, _model_loc, _scale );
}

void cNode::drawCOM( iBackend* _backend, int& _model_loc, double _scale )
{
	if ( !is_leaf )
	{
		double mass_scale = _scale * 0.1f;
		glm::mat4 model = glm::mat4( 1.0f );
		model = glm::translate( model, glm::vec3( com.x * _scale, com.y * _scale, com.z * _scale ) );
		model = glm::scale( model, glm::vec3{ size * mass_scale, size * mass_scale, size * mass_scale } );

		_backend->setUniformMat4f( _model_loc, glm::value_ptr( model ) );
		_backend->drawArrays( 24, eDrawMode::DrawMode_Lines );
	}
	
	if( !is_leaf )
		for ( int i = 0; i < 8; i++ )
			if ( children[ i ] )
				children[ i ]->drawCOM( _backend, _model_loc, _scale );
}
