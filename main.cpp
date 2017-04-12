#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
//#include "matrix.h"
//#include "mocap.h"

// FOR GLOW: header files for glowmech
#include "glow.h"
GLOW_NAMESPACE_USING
#include "opencontrol.h"

static const float WINSIZE = 300;
MOCAP Mocap;
int windowID;   // FOR GLOW: Keep track of drawing window ID
MOCAP* mocappointer;

double rotAngle = 10;



double i=50.0,j=0.0,l=0.0,a=0.0,b=0.0,c=0.0,r=0.0,t=1.0,q=0.0;

Vector v(a,b,c);//use this for calculations later
Vector CamUP(r,t,q);
Vector CamForward(a-i,b-j,c-l);
Vector CamRight=cross(CamForward,CamUP);
//Vector CamUP=cross(CamRight,CamForward);







//some of john dingliana's matrix functions
///////////////////////////////////////////////////////////////////////
// converting between radians and degrees
///////////////////////////////////////////////////////////////////////

double degtoradians(double v)
{
	double PI = acos (-1.0);
	return (double)(PI/180) * v;
}

double radtodegrees(double v)
{
 double PI = acos (-1.0);
 return (180/PI)*v;
}

//////////////////////////////////////////////////////////////////////
// utility func: returns matrix for rotation about y axis
//////////////////////////////////////////////////////////////////////

matrixn3 xRotation(const double degrees)
{
	double c = cos (degtoradians(degrees));
	double s = sin (degtoradians(degrees));

	return matrixn3( 1, 0, 0,
				0, c, s,
				0, -s, c );

};

//////////////////////////////////////////////////////////////////////
// utility func: returns matrix for rotation about y axis
//////////////////////////////////////////////////////////////////////
matrixn3 yRotation(const double degrees)
{
	double c = cos (degtoradians(degrees));
	double s = sin (degtoradians(degrees));

	return matrixn3( c, 0, -s,
				0, 1, 0,
				s, 0, c );

};
//////////////////////////////////////////////////////////////////////
// utility func: returns matrix for rotation about z axis
//////////////////////////////////////////////////////////////////////
matrixn3 zRotation(const double degrees)
{
	double c = cos (degtoradians(degrees));
	double s = sin (degtoradians(degrees));

	return matrixn3( c, s, 0,
				-s, c, 0,
				0, 0, 1 );

};

//////////////////////////////////////////////////////////////////////
// utility func: return mat for rot about _axis_ by _parameter_
//////////////////////////////////////////////////////////////////////

matrixn3 rotation(const double parameter, const Vector axis)
{
	matrixn3 m;

  //if the axis is already the z axis then we don't need to do all this

	if (axis!=Vector(0,0,0))
	{
      double length1 = sqrt ( axis.x() * axis.x() + axis.z() * axis.z() ) ;
      double theta = radtodegrees(atan2 ( axis.x(), axis.z() ));// * 180.0 / pi ;
      double phi = radtodegrees(atan2 ( axis.y(), length1 ));// * 180.0 / pi ;

     m = yRotation ( -theta )
	     * xRotation ( phi )
	     * zRotation ( parameter )
	     * xRotation ( -phi )
	     * yRotation ( theta ) ;
	}
	else
		m.loadIdentity();

	return m;
}


void init()///just set up lighting etc.
{
      // initialize matrix stacks
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
	gluPerspective(30,1,1,100);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
	gluLookAt(0,50,0,  0,0,0,  0,0,-1);
    glViewport( 0, 0, WINSIZE, WINSIZE );
    glEnable( GL_DEPTH_TEST );
	glClearColor(0.8f,0.8f,1.0f,0);

   // Default values for material and light properties.
   GLfloat mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0 };
   GLfloat mat_diffuse[] = { 0.6f,0.6f,0.6f, 1.0 };
   GLfloat mat_specular[] = { 0,0,10, 1.0 };
   GLfloat mat_shininess[] = { 100.0 };
   GLfloat light_ambient[] = { 1, 1, 1, 1.0 };
   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_specular[] = { 10.0, 10.0, 10.0, 1.0 };
   glShadeModel (GL_SMOOTH);

//   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

   glEnable(GL_LIGHT0);
   glEnable(GL_NORMALIZE);
}

/// the glut display callback for redrawing the window

void display()
{
	glClearColor(0.5f, 0.0f, 0.0f,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);

glLoadIdentity();

CamUP.set(r,t,q);
CamForward.set(a-i,b-j,c-l);
CamRight=cross(CamForward,CamUP);
CamUP=cross(CamRight,CamForward);
CamForward.normalize();
CamRight.normalize();
CamUP.normalize();
r=CamUP.x();
t=CamUP.y();
q=CamUP.z();

gluLookAt(i, j, l,

          a, b, c,

          r, t, q);




	if (Mocap.getNchannels()) {
		Mocap.drawFrame(Mocap.lastFrameDrawn());
	}
    glutSwapBuffers();
}



int main(int argc, char **argv)
{
    Glow::Init(argc, argv);    // FOR GLOW: need to replace glutInit with this
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( WINSIZE, WINSIZE );
    windowID = glutCreateWindow( "mocap demo" );
    glutDisplayFunc( display );

    init();
  mocappointer = &Mocap;
  new OpenControl(windowID,mocappointer);

    Glow::MainLoop();    // FOR GLOW: need to replace glutMainLoop with this
    return 0;
}
