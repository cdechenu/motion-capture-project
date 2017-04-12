#ifndef VECTOR_H
#define VECTOR_H

//  ============================================================
//
//                        Vector.h
//
//  ============================================================


#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>

using namespace std;

class	Vector 
{

   private :
	
      double		_vx, _vy, _vz ;

   public :

			//  Constructors

      inline Vector () ;

      inline Vector ( const double, const double = 0.0, const double = 0.0 ) ;

			//  Copy Constructor

      inline Vector ( const Vector& ) ;

			//  Destructor

      virtual ~Vector () ;

			//  Assignment

      inline Vector& operator= ( const Vector& ) ;

			//  Output

      friend ostream& operator<< ( ostream&, const Vector& ) ;

			//  Comparison

      inline friend int operator== ( const Vector&, const Vector& ) ;
      inline friend int operator!= ( const Vector&, const Vector& ) ;

			//  Sum, Difference, Scalar Product

      inline friend Vector operator+ ( const Vector&, const Vector& ) ;
      inline friend Vector operator- ( const Vector&, const Vector& ) ;
      inline friend Vector operator- ( const Vector& ) ;
      inline friend Vector operator* ( const double&, const Vector& ) ;
      inline friend Vector operator* ( const Vector&, const double& ) ;
      inline friend Vector operator/ ( const Vector&, const double& ) ;

			//  Immediate Sum, Difference, Scalar Product

      inline Vector& operator+= ( const Vector& ) ;
      inline Vector& operator-= ( const Vector& ) ;
      inline Vector& operator*= ( const double& ) ;
      inline Vector& operator/= ( const double& ) ;

			//  Member Functions
			   //  Dot Product

      inline friend double dot ( const Vector&, const Vector& ) ;

			   //  Cross Product

      inline friend Vector cross ( const Vector&, const Vector& ) ;

			   //  Normalization, Make it a unit Vector

      inline void normalize () ;

			   //  Vector Length

      inline double length () const ;

			//  Access Functions to get 
			//    x-coordinate, y-coordinate or
			//    z-coordinate of the vector
                             
	  //safe way
      inline double x () const { return _vx ; } ;
      inline double y () const { return _vy ; } ;
      inline double z () const { return _vz ; } ;

	  //returning reference to privates
	  inline double& x ()	{ return _vx ; } ;
      inline double& y ()	{ return _vy ; } ;
      inline double& z ()	{ return _vz ; } ;

	  inline friend double distance(const Vector point_a, const Vector point_b);
  	  inline friend double distancesq(const Vector point_a, const Vector point_b);
	  void set(double x,double y, double z)
	  {
		_vx = x;
        _vy = y;
        _vz = z;

	  }

	  
 

} ;

//  Constants
const Vector VZero ( 0.0, 0.0, 0.0 ) ;
const Vector VX ( 1.0, 0.0, 0.0 ) ;
const Vector VY ( 0.0, 1.0, 0.0 ) ;
const Vector VZ ( 0.0, 0.0, 1.0 ) ;

/////////////////////////////////////////////////////
// USEFUL GLOBALS
/////////////////////////////////////////////////////

#include <GL/glut.h>

#define glVertexVec(X) glVertex3d(X.x(), X.y(), X.z())
#define glNormalVec(X) glNormal3d(X.x(), X.y(), X.z())
#define glTranslateVec(X) glTranslated(X.x(), X.y(), X.z())
#define glRotateVec(X, Y) glRotated(X, Y.x(), Y.y(), Y.z())

//following utility for getting scalars out of vector
#define ROTCEV(X) X.x(), X.y(), X.z() 


/////////////////////////////////////////////////////


inline Vector :: Vector ()

   {
      _vx = 0.0 ;
      _vy = 0.0 ;
      _vz = 0.0 ;
   }

inline Vector :: Vector ( const double x, const double y, const double z )

   {
      _vx = x ;
      _vy = y ;
      _vz = z ;
   }

inline Vector :: Vector ( const Vector& v ) 

   {
      _vx = v._vx ;
      _vy = v._vy ;
      _vz = v._vz ;
   }

inline Vector :: ~Vector () { }
      

/*
   ==========================================================

	       OPERATOR=

   ==========================================================
*/


inline Vector& Vector :: operator= ( const Vector& v ) 

   {
      if ( this == &v ) return ( *this ) ;

      _vx = v._vx ;
      _vy = v._vy ;
      _vz = v._vz ;

      return ( *this ) ;
   }

/*
   ==========================================================

	       OUTPUT

   ==========================================================
*/

inline ostream& operator<< ( ostream& co, const Vector& v )

   {
      co << "< "
	 << v._vx 
	 << ", "
	 << v._vy 
	 << ", "
	 << v._vz 
	 << " >" ;

      return co ;
   }

/*
   ==========================================================

	       COMPARISON OPERATIONS

   ==========================================================
*/

inline int operator== ( const Vector& v1, const Vector& v2 ) 

   {
      if ( ( v1._vx == v2._vx ) &&
           ( v1._vy == v2._vy ) &&
           ( v1._vz == v2._vz ) )
	    return ( true ) ;
	 else
	    return ( false ) ;
   }

inline int operator!= ( const Vector& v1, const Vector& v2 ) 

   {
      if ( ( v1._vx != v2._vx ) ||
           ( v1._vy != v2._vy ) ||
           ( v1._vz != v2._vz ) )
	    return ( true ) ;
	 else
	    return ( false ) ;
   }

/*
   ==========================================================

	       ARITHMETIC OPERATIONS

   ==========================================================
*/

inline Vector operator+ ( const Vector& v1, const Vector& v2 ) 

   {
      Vector vv ;

      vv._vx = v1._vx + v2._vx ;
      vv._vy = v1._vy + v2._vy ;
      vv._vz = v1._vz + v2._vz ;

      return ( vv ) ;
   }

inline Vector operator- ( const Vector& v1, const Vector& v2 ) 

   {
      Vector vv ;

      vv._vx = v1._vx - v2._vx ;
      vv._vy = v1._vy - v2._vy ;
      vv._vz = v1._vz - v2._vz ;

      return ( vv ) ;
   }

inline Vector operator- ( const Vector& v ) 

   {
      Vector vv ;

      vv._vx = - v._vx ;
      vv._vy = - v._vy ;
      vv._vz = - v._vz ;

      return ( vv ) ;
   }

inline Vector operator* ( const double& c, const Vector& v ) 

   {
      Vector vv ;

      vv._vx = c * v._vx ;
      vv._vy = c * v._vy ;
      vv._vz = c * v._vz ;

      return ( vv ) ;
   }

inline Vector operator* ( const Vector& v, const double& c ) 

   {
      Vector vv ;

      vv._vx = c * v._vx ;
      vv._vy = c * v._vy ;
      vv._vz = c * v._vz ;

      return ( vv ) ;
   }

inline Vector operator/ ( const Vector& v, const double& c ) 

   {
      Vector vv ;

      vv._vx = v._vx / c ;
      vv._vy = v._vy / c ;
      vv._vz = v._vz / c ;

      return ( vv ) ;
   }

inline Vector& Vector :: operator+= ( const Vector& v ) 

   {
      _vx += v._vx ;
      _vy += v._vy ;
      _vz += v._vz ;

      return *this ;
   }

inline Vector& Vector :: operator-= ( const Vector& v ) 

   {
      _vx -= v._vx ;
      _vy -= v._vy ;
      _vz -= v._vz ;

      return *this ;
   }

inline Vector& Vector :: operator*= ( const double& c ) 

   {
      _vx *= c ;
      _vy *= c ;
      _vz *= c ;

      return *this ;
   }

inline Vector& Vector :: operator/= ( const double& c ) 

   {
      _vx /= c ;
      _vy /= c ;
      _vz /= c ;

      return *this ;
   }

/*
   ==========================================================

	       NORMALIZE

   ==========================================================
*/

inline void Vector :: normalize ()  

   {
      double l =  length () ;

	  if ( l != 0 )
	  {
      _vx = _vx / l ;
      _vy = _vy / l ;
      _vz = _vz / l ;
	  }
	  else
	  {
		  _vx =0.0;
		  _vy =0.0;
		  _vz =0.0;
	  }

   }

/*
   ==========================================================

	       LENGTH

   ==========================================================
*/

inline double Vector :: length ()  const

   {
      double		l ;

      l =  sqrt ( _vx * _vx + _vy * _vy + _vz * _vz ) ;
      return ( l ) ;
   }

/*
   ==========================================================

	       DOT

   ==========================================================
*/

inline double dot ( const Vector& v1, const Vector& v2 ) 

   {
      double		d ;

      d =  v1._vx * v2._vx + v1._vy * v2._vy + v1._vz * v2._vz ;

      return ( d ) ;
   }


/*
   ==========================================================

	       CROSS

   ==========================================================
*/

inline Vector cross ( const Vector& v1, const Vector& v2 ) 

   {
      Vector vv ;

      vv._vx = v1._vy * v2._vz - v1._vz * v2._vy ;
      vv._vy = - v1._vx * v2._vz + v1._vz * v2._vx ;
      vv._vz = v1._vx * v2._vy - v1._vy * v2._vx ;

      return ( vv ) ;
   }


inline double distance(const Vector point_a, const Vector point_b)
{

	Vector c;
	
	c = point_b - point_a;

	double d = c.length();

	return d;

}

inline double distancesq(const Vector point_a, const Vector point_b)
{

	Vector c;

	c = point_b - point_a;

	double l = c._vx * c._vx + c._vy * c._vy + c._vz * c._vz  ;

	return l;

}

#endif



