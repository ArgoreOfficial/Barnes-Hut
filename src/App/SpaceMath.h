#pragma once

#include <Core/Math/Vector3.h>

namespace SpaceMath
{
	/* formula derived from https://beltoforion.de/en/barnes-hut-galaxy-simulator/ */
	static wv::cVector3d computeForce( wv::cVector3d& _1p, wv::cVector3d& _2p, double& _1m, double& _2m )
	{
		double mm = _1m * _2m;
		wv::cVector3d rirj = _1p - _2p;
		double magnitude = rirj.length();
		return ( rirj / pow( magnitude, 3 ) ) * mm;
	}

	/* B-V to RGB */
	static void bv2rgb( double& r, double& g, double& b, double bv )    // RGB <0,1> <- BV <-0.4,+2.0> [-]
	{
		double t;
		r = 0.0; g = 0.0; b = 0.0;
		if ( bv < -0.4 ) bv = -0.4;
		if ( bv > 2.0 ) bv = 2.0;
		if ( ( bv >= -0.40 ) && ( bv < 0.00 ) ) { t = ( bv + 0.40 ) / ( 0.00 + 0.40 ); r = 0.61 + ( 0.11 * t ) + ( 0.1 * t * t ); }
		else if ( ( bv >= 0.00 ) && ( bv < 0.40 ) ) { t = ( bv - 0.00 ) / ( 0.40 - 0.00 ); r = 0.83 + ( 0.17 * t ); }
		else if ( ( bv >= 0.40 ) && ( bv < 2.10 ) ) { t = ( bv - 0.40 ) / ( 2.10 - 0.40 ); r = 1.00; }
		if ( ( bv >= -0.40 ) && ( bv < 0.00 ) ) { t = ( bv + 0.40 ) / ( 0.00 + 0.40 ); g = 0.70 + ( 0.07 * t ) + ( 0.1 * t * t ); }
		else if ( ( bv >= 0.00 ) && ( bv < 0.40 ) ) { t = ( bv - 0.00 ) / ( 0.40 - 0.00 ); g = 0.87 + ( 0.11 * t ); }
		else if ( ( bv >= 0.40 ) && ( bv < 1.60 ) ) { t = ( bv - 0.40 ) / ( 1.60 - 0.40 ); g = 0.98 - ( 0.16 * t ); }
		else if ( ( bv >= 1.60 ) && ( bv < 2.00 ) ) { t = ( bv - 1.60 ) / ( 2.00 - 1.60 ); g = 0.82 - ( 0.5 * t * t ); }
		if ( ( bv >= -0.40 ) && ( bv < 0.40 ) ) { t = ( bv + 0.40 ) / ( 0.40 + 0.40 ); b = 1.00; }
		else if ( ( bv >= 0.40 ) && ( bv < 1.50 ) ) { t = ( bv - 0.40 ) / ( 1.50 - 0.40 ); b = 1.00 - ( 0.47 * t ) + ( 0.1 * t * t ); }
		else if ( ( bv >= 1.50 ) && ( bv < 1.94 ) ) { t = ( bv - 1.50 ) / ( 1.94 - 1.50 ); b = 0.63 - ( 0.6 * t * t ); }
	}
};