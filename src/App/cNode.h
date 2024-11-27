#pragma once

#include <Core/Math/Vector3.h>

class iBackend;
struct sPoint;

class cNode
{
public:

	cNode( cNode* _parent, double _size ):
		parent{ _parent },
		size{ _size }
	{ }
	~cNode();

	unsigned char getNodeAtPoint( const wv::cVector3d& _position );
	cNode* findEmptyNode( const wv::cVector3d& _position );
	bool containsPoint( const wv::cVector3d& _position );

	void split();
	void checkParticleExited();
	void clearChildren();
	void recalculateData();
	void accumulatePointMass();
	void recalculateMass( double* _out_mass, wv::cVector3d* _out_com );

	wv::cVector3d computeForces( sPoint* _point, const float& _theta );

	void draw( iBackend* _backend, int& _model_loc, double _scale );
	void drawCOM( iBackend* _backend, int& _model_loc, double _scale );

	cNode* parent = nullptr;
	cNode* children[ 8 ]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	bool is_leaf = true;

	wv::cVector3d com;                   /* center of mass */
	wv::cVector3d center{ 0.0, 0.0, 0.0 }; /* center of node */
	double size = 0.0;
	double mass = 0.0;

	sPoint* data = nullptr;

	/*

	   Node Children Layout:

			+Y (up)
	  -Z
		\
		2--------6
		|\\      |\
		| \@     | \
   -X ---@ 3--------7 -- +X
		0--|-----4  |
		 \ |   @  \ |
		  \|    \  \|
		   1-----\--5
				  \
				  +Z

			-Y (down)

		Node position corresponds to the 3 bits ( 0 = -v, 1 = +v ) of it's id

	0 - - -
	1 - - +
	2 - + -
	3 - + +
	4 + - -
	5 + - +
	6 + + -
	7 + + +

	*/

};
