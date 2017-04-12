//mocap.cpp
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "mocap.h"
#include <stdlib.h>
//put in to create cylinders
void mySolidCylinder( GLdouble radius, GLdouble height,
                       GLint slices, GLint stacks )
{
    glPushAttrib(GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    GLUquadricObj* obj;
    obj = gluNewQuadric();
    gluCylinder(obj, radius, radius, height, slices, stacks);
    gluDeleteQuadric(obj);
    glPopAttrib();
}
bool dontaccumulateroot = false;       //whether the root should accumulate motion.
bool usespecialroottranslation = false; //use this when we are using a special root translation
bool usestraightwalkroottranslation = false;
bool isxdirection=false;
bool ispositive=true;			///both of these used for determining which direction to walk (when using straigh line option)
twoints localmins[500];
int localmincounter;
MOCAP::MOCAP()
{
	nchannels = 0;
	parseError = false;
	maxOffset = 0;
	drawScale = 1;
	drawCenter[0] = drawCenter[1] = drawCenter[2] = 0;
	graphwalk[0]=graphwalk[1]=0;
	innerwalk=false;
}

const int TAB=9;

void MOCAP::gnuPlot()
{
	///system("gnuplot graph");
	popen("gnuplot graph","r");
}

void MOCAP::Dotty()
{
	//system("dot test.dot -Tpng >test.png");///creates the image from the dot file which contains the data on the directed graph
	//popen("winview.bat","r");
	popen("dotty test.dot","r");
}

void MOCAP::graphLoop()
{
	///this is a temporary system for doing a looped graph walk
	if(graphwalk[0]==graphwalk[1])
	{
		//do nothing,
	}
	else
	{



		printf("i and j are %d,%d",graphwalk[0],graphwalk[1]);
		if(innerwalk==false)//so frame 1 ->x then y -> the end
		{

			if(graphwalk[1]<graphwalk[0])
			{
					int temp=graphwalk[1];
					graphwalk[1]=graphwalk[0];
					graphwalk[0]=temp;
			}

			int dataframe=graphwalk[0]+1;///the frame the translation data is coming from

			for (int i=0;i<=graphwalk[0];i++)
			{
				glClearColor(0.5f, 0.0f, 0.0f,0);
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
				drawFrame(i);
				glutSwapBuffers();
			}
			for (int j=graphwalk[1];j<nframes;j++)
			{
				usespecialroottranslation = true;

				glClearColor(0.5f, 0.0f, 0.0f,0);
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

				glPushMatrix();///We save the initial start position.
				glScalef(drawScale,drawScale,drawScale);
				glTranslatef(-drawCenter[0],-drawCenter[1],-drawCenter[2]);
				Mocap.background();

				glColor3f(0,0,1);
				glutSolidSphere(sphereSize,20,10);

				int initypos = data[1];
				float *frameData = &data[j*nchannels];
				float *specialdata = &data[dataframe*nchannels];

				root->draw(frameData,data,specialdata,j);///both "data" and "framedata"are passed in so that I can use the data from frame 0 as well as frame j. special data is the translation data for the root during a graph walk.
				lastframedrawn=dataframe;
				glPopMatrix();/// we restore the initial start position for the next frame.
				///end of draw frame code


				glutSwapBuffers();

				dataframe++;
				usespecialroottranslation=false;
			}

		}
		else if(innerwalk==true)//we will loop over an inside section of the data
		{

			if(graphwalk[1]<graphwalk[0])
			{
					int temp=graphwalk[1];
					graphwalk[1]=graphwalk[0];
					graphwalk[0]=temp;
			}

			int frame=graphwalk[0];///so frame starts off at the first graphwalk point

			for (int i=0;i<nframes;i++)
			{
				usespecialroottranslation = true;

				glClearColor(0.5f, 0.0f, 0.0f,0);
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

				glPushMatrix();///We save the initial start position.
				glScalef(drawScale,drawScale,drawScale);
				glTranslatef(-drawCenter[0],-drawCenter[1],-drawCenter[2]);
				Mocap.background();

				glColor3f(0,0,1);
				glutSolidSphere(sphereSize,20,10);

				int initypos = data[1];
				float *frameData = &data[frame*nchannels];
				float *specialdata = &data[i*nchannels];

				root->draw(frameData,data,specialdata,frame);///both "data" and "framedata"are passed in so that I can use the data from frame 0 as well as frame f. special data is the translation data for the root during a graph walk.
				lastframedrawn=i;
				glPopMatrix();/// we restore the initial start position for the next frame.
				///end of draw frame code


				glutSwapBuffers();

				usespecialroottranslation=false;
				frame++;

				///now reset the image data frame number
				if (frame==graphwalk[1])
				{
					frame=graphwalk[0];

				}
			}


		}
	}

}



void MOCAP::graphWalk()
{
	FILE *f = fopen("graphwalk.txt","w");/*This opens the file for writing ("w")*/

	int dataframe=0;
	int startj=0;
	int endi;
	int stoppoint=0;
	//the for here is just a count down
	//for(stoppoint=0;stoppoint<25;stoppoint++)

	while(stoppoint<=nframes)
	{
		int endi=0;
		while(localmins[endi].i<=((localmins[startj].j)+5)&&endi<localmincounter)
		{
			//so we cycle through looking for a point to play up to
			endi++;
		}

		//then we play the segment
		for (int j=localmins[startj].j;j<=localmins[endi].i;j++)
		{
			/*glClearColor(0.5f, 0.0f, 0.0f,0);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			drawFrame(j);
			glutSwapBuffers();*/


			usespecialroottranslation = true;

			glClearColor(0.5f, 0.0f, 0.0f,0);

			//for (int n=0;n<2;n++){//put in to allow slower speed

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			glPushMatrix();///We save the initial start position.
			glScalef(drawScale,drawScale,drawScale);
			glTranslatef(-drawCenter[0],-drawCenter[1],-drawCenter[2]);
			Mocap.background();

			glColor3f(0,0,1);
			glutSolidSphere(sphereSize,20,10);

			int initypos = data[1];
			float *frameData = &data[j*nchannels];///j is the frame number
			///j* nchannels skips through the array until it reaches a j frame times number channels per frame to reach the
			///start of the current frames data in the data aray
			float *specialdata = &data[dataframe*nchannels];

			root->draw(frameData,data,specialdata, j);///both "data" and "framedata"are passed in so that I can use the data from frame 0 as well as frame j. special data is the translation data for the root during a graph walk.
			lastframedrawn=dataframe;
			glPopMatrix();/// we restore the initial start position for the next frame.
			///end of draw frame code


			glutSwapBuffers();
			//}//end of slow motion brackets
			dataframe++;

			///now reset dataframe if we have reached the end
			if (dataframe==(nframes-1))
			{
				dataframe=0;

			}

			stoppoint++;
			usespecialroottranslation=false;



		}

		fprintf(f,"%d %d\n",localmins[startj].j,localmins[endi].i-1);


		//finally we set startj equal to endi to we then take the point we want to jump to
		startj=endi;



	}

	fclose(f);/*And here we close the file*/


}

bool MOCAP::graphCalculated()
{
	if(graphcalculated)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void MOCAP::calculateGraphMethodA()
{
	///Method A is differences of angles

	FILE *f = fopen("results.txt","w");/*This opens the file for writing ("w")*/

	//if (f==NULL) return(1); /*error checker checks if there is no file to output to*/

	///so x is frame x
	for(int x=0;  x<nframes;x++)
	{
		///so for each frame x and for each frame y
		for(int y=0;  y<nframes;y++)
		{
			float temptotal = 0.0;

			///Im making it start at 6 to skip the root joint
			///so we get the sum of the differences between the angles at
			///each channel and add all the differences up
			for(int channelnumber=6;channelnumber<nchannels;channelnumber++)
			{
				float tempterm = fabs(data[x*nchannels+channelnumber]-data[y*nchannels+channelnumber]);

				///now I make sure that we taking the acute angle
				if(tempterm>180)
				{
					tempterm = 360-tempterm;
				}



				///now add the given channel error onto the total
				temptotal += tempterm;
				//fprintf(f,"x: %d y: %d channelnumber: %d xdata(%f)-ydata(%f) = termvalue:%f total at: %f\n",x,y,channelnumber,data[x*nchannels+channelnumber],data[y*nchannels+channelnumber],tempterm,temptotal);
			}

			if(x==0&&y==0)
			{
				///We need to get values for minerror and max error so we
				///initially set them the temptotal if its our first loop.
				minerror=maxerror=temptotal;

			}
			else if(minerror>temptotal)
			{
				minerror=temptotal;
			}
			else if(maxerror<temptotal)
			{
				maxerror=temptotal;
			}


			graphdata[x*nframes+y] = temptotal;
			fprintf(f,"%d %d %f\n",x,y,temptotal);



		}
	///putting in a blank line
	fprintf(f,"\n");
	}

	fclose(f);/*And here we close the file*/

	graphcalculated = true;

	//system("gnuplot graph");

	/*FILE *g = fopen("results2.txt","w");//This opens the file for writing ("w")

	for (int i=0; i<nframes; i++)
	{
		for (int j=0; j<nframes; j++)
		{
			float value;
			float localmin;
			float localmax;

			///first we get the error value for that point.
			value = this->getErrorData(i,j);
			///now we must check it against its neighbours for local max or min.
			///first set them equal to our point.
			localmax = localmin = value;

			///now check nearest neighbours
			for (int k=i-2; k<=i+2; k++)
			{
				for (int l=j-2; l<=j+2; l++)
				{
					float errordata = this->getErrorData(k,l);
					///dont perform the calculation if either j or k are out of bounds.
					if(k>=0&&k<=nframes&&j>=0&&j<=nframes)
					{

						if(localmax < errordata)
						{
							localmax = errordata;
						}
						if(localmin > errordata)
						{
							localmin = errordata;
						}
					}
				}
			}

			///fprintf(f,"i = %d j = %d value = %f localmin = %f localmax = %f \n",i,j,value, localmin,localmax);

			if(value==localmin)
			{

				fprintf(f,"%d %d -1\n",i,j);

			}
			else if(value == localmax)
			{
				fprintf(f,"%d %d +1\n",i,j);
			}
			else
			{
				fprintf(f,"%d %d 0\n",i,j);
			}



		}
	}

	//printf("local max is %d and local min is %d",localmax,localmin);
	fclose(g);//And here we close the file*/



}

void MOCAP::calculateGraphMethodB()
{
	///Method B is sum of squared differences

	FILE *f = fopen("results.txt","w");/*This opens the file for writing ("w")*/

	//if (f==NULL) return(1); /*error checker checks if there is no file to output to*/

	///so x is frame x
	for(int x=0;  x<nframes;x++)
	{
		///so for each frame x and for each frame y
		for(int y=0;  y<nframes;y++)
		{
			float temptotal = 0.0;

			///Im making it start at 6 to skip the root joint
			///so we get the sum of the differences between the angles at
			///each channel and add all the differences up
			for(int channelnumber=6;channelnumber<nchannels;channelnumber++)
			{
				float tempterm = fabs(data[x*nchannels+channelnumber]-data[y*nchannels+channelnumber]);

				///now I make sure that we taking the acute angle
				if(tempterm>180)
				{
					tempterm = 360-tempterm;
				}



				///now add the given channel error onto the total
				temptotal += pow(tempterm,2);
				//fprintf(f,"x: %d y: %d channelnumber: %d xdata(%f)-ydata(%f) = termvalue:%f total at: %f\n",x,y,channelnumber,data[x*nchannels+channelnumber],data[y*nchannels+channelnumber],tempterm,temptotal);
			}
			///were getting the sum of the squared differences
			temptotal = sqrt(temptotal);

			if(x==0&&y==0)
			{
				///We need to get values for minerror and max error so we
				///initially set them the temptotal if its our first loop.
				minerror=maxerror=temptotal;

			}
			else if(minerror>temptotal)
			{
				minerror=temptotal;
			}
			else if(maxerror<temptotal)
			{
				maxerror=temptotal;
			}


			graphdata[x*nframes+y] = temptotal;
			fprintf(f,"%d %d %f\n",x,y,temptotal);



		}
	///putting in a blank line
	fprintf(f,"\n");
	}

	fclose(f);/*And here we close the file*/

	graphcalculated = true;

	//system("gnuplot graph");
}

void MOCAP::calculateGraphMethodC()
{

	///Method C is sum of squared differences (exceptnot squared but to power 4)

	FILE *f = fopen("results.txt","w");/*This opens the file for writing ("w")*/

	//if (f==NULL) return(1); /*error checker checks if there is no file to output to*/

	///so x is frame x
	for(int x=0;  x<nframes;x++)
	{
		///so for each frame x and for each frame y
		for(int y=0;  y<nframes;y++)
		{
			float temptotal = 0.0;

			///Im temporarily making it start at 6 to skip the root joint
			///so we get the sum of the differences between the angles at
			///each channel and add all the differences up
			for(int channelnumber=6;channelnumber<nchannels;channelnumber++)
			{
				float tempterm = fabs(data[x*nchannels+channelnumber]-data[y*nchannels+channelnumber]);

				///now I make sure that we taking the acute angle
				if(tempterm>180)
				{
					tempterm = 360-tempterm;
				}



				///now add the given channel error onto the total
				temptotal += pow(tempterm,4);
				//fprintf(f,"x: %d y: %d channelnumber: %d xdata(%f)-ydata(%f) = termvalue:%f total at: %f\n",x,y,channelnumber,data[x*nchannels+channelnumber],data[y*nchannels+channelnumber],tempterm,temptotal);
			}
			///were getting the sum of the squared differences
			temptotal = pow(temptotal,0.25);

			if(x==0&&y==0)
			{
				///We need to get values for minerror and max error so we
				///initially set them the temptotal if its our first loop.
				minerror=maxerror=temptotal;

			}
			else if(minerror>temptotal)
			{
				minerror=temptotal;
			}
			else if(maxerror<temptotal)
			{
				maxerror=temptotal;
			}


			graphdata[x*nframes+y] = temptotal;
			fprintf(f,"%d %d %f\n",x,y,temptotal);



		}
	///putting in a blank line
	fprintf(f,"\n");
	}

	fclose(f);/*And here we close the file*/

	graphcalculated = true;


}

///METHOD D WAS NOT IMPLEMENTED
/*
void MOCAP::calculateGraphMethodD()
{
	///Method D is differences of angles but with weights based on how far down the chain

	FILE *f = fopen("results.txt","w");//This opens the file for writing ("w")

	//if (f==NULL) return(1); //error checker checks if there is no file to output to

	///so x is frame x
	for(int x=0;  x<nframes;x++)
	{
		///so for each frame x and for each frame y
		for(int y=0;  y<nframes;y++)
		{
			float temptotal = 0.0;
			int modnumber = 0; //this checks if we are the next joint (3 channels per joint)
			int weight = 1;
			int current=1;
			int i,j,k,l,m,n;
			i=j=k=l=m=n=0;
			JOINT *pointer;
			&pointer=root->child[i];
			///Im temporarily making it start at 6 to skip the root joint
			///so we get the sum of the differences between the angles at
			///each channel and add all the differences up
			for(int channelnumber=6;channelnumber<nchannels;channelnumber++)
			{

				//im gonna try and come up with a way of weighting based on number children
				if(modnumber%3=0)//so we have to get the weight
				{
					while(current>0)
					{
						if(root->child[i]->nchildren==0)
						{
							weight=0;
						}
						else
						{

						}

					}

				}
				modnumber +=1;
				current=1;//just to reset it

				float tempterm = fabs(data[x*nchannels+channelnumber]-data[y*nchannels+channelnumber]);

				///now I make sure that we taking the acute angle
				if(tempterm>180)
				{
					tempterm = 360-tempterm;
				}



				///now add the given channel error onto the total
				temptotal += tempterm;
				//fprintf(f,"x: %d y: %d channelnumber: %d xdata(%f)-ydata(%f) = termvalue:%f total at: %f\n",x,y,channelnumber,data[x*nchannels+channelnumber],data[y*nchannels+channelnumber],tempterm,temptotal);
			}

			if(x==0&&y==0)
			{
				///We need to get values for minerror and max error so we
				///initially set them the temptotal if its our first loop.
				minerror=maxerror=temptotal;

			}
			else if(minerror>temptotal)
			{
				minerror=temptotal;
			}
			else if(maxerror<temptotal)
			{
				maxerror=temptotal;
			}


			graphdata[x*nframes+y] = temptotal;
			fprintf(f,"%d %d %f\n",x,y,temptotal);



		}
	///putting in a blank line
	fprintf(f,"\n");
	}

	fclose(f);///And here we close the file

	graphcalculated = true;

}*/

void MOCAP::calculateGraphMethodE()
{
	///Method E;  I will try and bring in velocities

	FILE *f = fopen("results.txt","w");//This opens the file for writing ("w")

	//if (f==NULL) return(1); //error checker checks if there is no file to output to

	///so x is frame x
	for(int x=0;  x<nframes;x++)
	{
		///so for each frame x and for each frame y
		for(int y=0;  y<nframes;y++)
		{
			float temptotal = 0.0;

			///Im making it start at 6 to skip the root joint
			///so we get the sum of the differences between the angles at
			///each channel and add all the differences up
			for(int channelnumber=6;channelnumber<nchannels;channelnumber++)
			{
				float tempterm = fabs(data[x*nchannels+channelnumber]-data[y*nchannels+channelnumber]);

				float tempvelocityx = data[x*nchannels+channelnumber]-data[(x-1)*nchannels+channelnumber];
				float tempvelocityy = data[y*nchannels+channelnumber]-data[(y-1)*nchannels+channelnumber];

				///now I make sure that we taking the acute angle
				if(tempterm>180)
				{
					tempterm = 360-tempterm;
				}



				///now add the given channel error onto the total
				//temptotal += pow(tempterm,2);
				temptotal += pow(((tempvelocityx-tempvelocityy)),2);//so im adding on the change in velocity at a frame weighted by power of 10.
				//fprintf(f,"x: %d y: %d channelnumber: %d xdata(%f)-ydata(%f) = termvalue:%f total at: %f\n",x,y,channelnumber,data[x*nchannels+channelnumber],data[y*nchannels+channelnumber],tempterm,temptotal);
			}
			///were getting the sum of the squared differences
			temptotal = sqrt(temptotal);

			if(x==0&&y==0)
			{
				///We need to get values for minerror and max error so we
				///initially set them the temptotal if its our first loop.
				minerror=maxerror=temptotal;

			}
			else if(minerror>temptotal)
			{
				minerror=temptotal;
			}
			else if(maxerror<temptotal)
			{
				maxerror=temptotal;
			}


			graphdata[x*nframes+y] = temptotal;
			fprintf(f,"%d %d %f\n",x,y,temptotal);



		}
	///putting in a blank line
	fprintf(f,"\n");
	}

	fclose(f);//And here we close the file

	graphcalculated = true;

	//system("gnuplot graph");


}

void MOCAP::calculateGraphMethodF()
{
	///Method E;  I will try and bring in accelerations

	FILE *f = fopen("results.txt","w");//This opens the file for writing ("w")

	//if (f==NULL) return(1); //error checker checks if there is no file to output to

	///so x is frame x
	for(int x=0;  x<nframes;x++)
	{
		///so for each frame x and for each frame y
		for(int y=0;  y<nframes;y++)
		{
			float temptotal = 0.0;

			///Im making it start at 6 to skip the root joint
			///so we get the sum of the differences between the angles at
			///each channel and add all the differences up
			for(int channelnumber=6;channelnumber<nchannels;channelnumber++)
			{
				float tempterm = fabs(data[x*nchannels+channelnumber]-data[y*nchannels+channelnumber]);

				float tempvelocityx1 = data[x*nchannels+channelnumber]-data[(x-1)*nchannels+channelnumber];
				float tempvelocityx2 = data[(x-1)*nchannels+channelnumber]-data[(x-2)*nchannels+channelnumber];
				float tempaccelerationx = tempvelocityx1-tempvelocityx2;

				float tempvelocityy = data[y*nchannels+channelnumber]-data[(y-1)*nchannels+channelnumber];

				float tempvelocityy1 = data[y*nchannels+channelnumber]-data[(y-1)*nchannels+channelnumber];
				float tempvelocityy2 = data[(y-1)*nchannels+channelnumber]-data[(y-2)*nchannels+channelnumber];
				float tempaccelerationy = tempvelocityy1-tempvelocityy2;

				///now I make sure that we taking the acute angle
				if(tempterm>180)
				{
					tempterm = 360-tempterm;
				}



				///now add the given channel error onto the total
				//temptotal += pow(tempterm,2);
				temptotal += pow(((tempaccelerationx-tempaccelerationy)),2);//so im adding on the change in acceleration at a frame weighted by power of 10.
				//fprintf(f,"x: %d y: %d channelnumber: %d xdata(%f)-ydata(%f) = termvalue:%f total at: %f\n",x,y,channelnumber,data[x*nchannels+channelnumber],data[y*nchannels+channelnumber],tempterm,temptotal);
			}
			///were getting the sum of the squared differences
			temptotal = sqrt(temptotal);

			if(x==0&&y==0)
			{
				///We need to get values for minerror and max error so we
				///initially set them the temptotal if its our first loop.
				minerror=maxerror=temptotal;

			}
			else if(minerror>temptotal)
			{
				minerror=temptotal;
			}
			else if(maxerror<temptotal)
			{
				maxerror=temptotal;
			}


			graphdata[x*nframes+y] = temptotal;
			fprintf(f,"%d %d %f\n",x,y,temptotal);



		}
	///putting in a blank line
	fprintf(f,"\n");
	}

	fclose(f);//And here we close the file

	graphcalculated = true;

	//system("gnuplot graph");


}

float MOCAP::getErrorData(int framex, int framey) ///return data from the error graph array
{
	return graphdata[framex*nframes+framey];
}

float MOCAP::getMaxError() ///return data from the error graph array
{
	return maxerror;
}

float MOCAP::getMinError() ///return data from the error graph array
{
	return minerror;
}

int MOCAP::getThreshold() ///threshold
{
	return threshold;
}

void MOCAP::setThreshold(int thresh) ///threshold
{
	threshold=thresh;
}


void MOCAP::computeBbox()
{
	const float LARGE = 1e10;
	xmin = ymin = zmin = LARGE;
	xmax = ymax = zmax = -LARGE;
	for (int f=0; f<nframes; f++) {
		// update bounding box assuming first 3 channels are XYZ global translation
		int i = f*nchannels;
		float x = data[i+0];
		float y = data[i+1];
		float z = data[i+2];
		xmin = (x<xmin ? x : xmin);
		xmax = (x>xmax ? x : xmax);
		ymin = (y<ymin ? y : ymin);
		ymax = (y>ymax ? y : ymax);
		zmin = (z<zmin ? z : zmin);
		zmax = (z>zmax ? z : zmax);
	}
}

int MOCAP::read(const char *fname)///used for reading in the mocap file into the channels etc. Takes in the name of the file
{
	lastframedrawn=0;
	threshold = 0;
	minerror = maxerror = 0;
	graphcalculated = false;

	STR rootname;///String (typedef in mocap.h).

	fp = fopen(fname,"rt");///opens file for reading in text mode. This is called fp
	nextline();///reads in next line into "line"

	expect("HIERARCHY");
	nextline();

	expect("ROOT");
	sscanf(line,"ROOT %s",rootname);///reads from line into rootname skipping ROOT and saving whats after it.
	nextline();
	root = new JOINT(rootname);    // recursive:  parses remainder of header

	lookfor("MOTION");
	nextline();

	expect("Frames:");
	sscanf(line,"Frames:%d",&nframes);
	nextline();

	expect("Frame Time:");
	sscanf(line,"Frame Time:%f",&dt);
//	nextline();

	printf("\n %d channels, %d frames,  dt=%6.4f\n",nchannels,nframes,dt);
	data = (float *) malloc(nframes*nchannels*sizeof(float));///malloc is for memory allocation

	///Im gonna alocate memory for my error graph here:
	graphdata = (float *) malloc(nframes*nframes*sizeof(float));

	for (int n=0, indx=0; n<nframes; n++) {
		for (int c=0; c<nchannels; c++) {
			fscanf(fp,"%f",&data[indx++]);
		}
	}

	computeBbox();           // xyz-bounding box of movement
	drawScale = 5.0/maxOffset;

	///added  from old camera setting method
	drawCenter[0] = 0.5*(xmin+xmax);
	drawCenter[1] = 0.5*(ymin+ymax);
	drawCenter[2] = 0.5*(zmin+zmax);

	sphereSize = 0.06f*maxOffset;

	fclose(fp);
	return 0;
}


void MOCAP::play()
{
	if (!Mocap.getNchannels()) ///ie if the number of channels is zero just return
		return;
	glClearColor(0.5f, 0.0f, 0.0f,0);
	for (int f=0; f<nframes; f++) {
		//for (int n=0; n<2; n++) {//allows a slower speed
		    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			drawFrame(f);
			glutSwapBuffers();
		//}

	}
}

void MOCAP::playframeloop(int f)
{
///Badly named ...... just goes to that frame.
	glClearColor(0.5f, 0.0f, 0.0f,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	drawFrame(f);
	glutSwapBuffers();
}



void MOCAP::drawFrame(int f)
{
	//printf("drawing frame %d\n",f);
	glPushMatrix();///We save the initial start position.
	glScalef(drawScale,drawScale,drawScale);
	glTranslatef(-drawCenter[0],-drawCenter[1],-drawCenter[2]);
	Mocap.background();

	glColor3f(0,0,1);
	glutSolidSphere(sphereSize,20,10);

	int initypos = data[1];
	float *frameData = &data[f*nchannels];///f is the frame number from "play" above
	///f* nchannels skips through the array until it reaches a f frame times number channels per frame to reach the
	///start of the current frames data in the data aray
	root->draw(frameData,data,frameData, f);///ive added in data so that we can use the data from frame 0.  Also here i just put frame data in as the specialdata
	lastframedrawn=f;
	glPopMatrix();/// we restore the initial start position for the next frame.
}

int MOCAP::lastFrameDrawn()
{
	return lastframedrawn;
}


void MOCAP::background()
{
	float x,z;
	int countx, countz;
	const float xmin=-10, xmax=10;
	const float zmin=-10, zmax=10;
	const int nsteps = 20;
	glColor3f(1,1,1);
	float xsize = (xmax - xmin)/nsteps;
	float zsize = (zmax - zmin)/nsteps;

	glPushMatrix();
	float sf = maxOffset;
	glScalef(sf,sf,sf);
	glLineWidth(1);


	float colour = 1;
	for (countz=0; countz<=nsteps; countz++) {
		for (countx=0; countx<=nsteps; countx++) {
			glColor3f(colour,colour,colour);
			glBegin(GL_QUADS);
			x = xmin + countx*xsize;
			z = zmin + countz*xsize;
			glVertex3f(x,0,z);
			glVertex3f(x+xsize,0,z);
			glVertex3f(x+xsize,0,z+zsize);
			glVertex3f(x,0,z+zsize);
			glEnd();

			//This just makes the colours flip on the tiles
			if (colour==1){colour=0;}else{colour=1;}
		}
	}
	glPopMatrix();

}



int MOCAP::nextline()
{
	char *start;///pointer to character
	LONGSTR tmp;///Longstring (whole line)

	while (!feof(fp)) {///Ensures fp has not reached the end of the file
		char *rv = fgets(line,STRLEN,fp);///fgets stores characters read from a stream. It stores them in "line" up to length "STRLEN" from stream "fp". Is also stops when a new line is reached btw
		if (!rv) break;
		  // find first char in line
		for (start=line; *start==' ' || *start==TAB; start++);
		if (*start=='\n') continue;
		if (strlen(start)>0) {
			strcpy(tmp,start);          // get rid of leading white space
			strcpy(line,tmp);
			return 1;
		}
	}
	printf("Hit end-of-file\n");
	return 0;
}



int MOCAP::expect(char *word)
{
	if (!strncmp(word,line,strlen(word))) {///simply compares word to line up to length of word
		return 1;
	}
	printf("Quitting --- unable to find '%s'\n",word);
	printf("line was: '%s'\n",line);
	parseError = true;
	return 0;
}


int MOCAP::lookfor(char *word)
{
	while (!feof(fp)) {
		if (!strncmp(word,line,strlen(word))) {
			return 1;
		}
		nextline();
	}
	printf("Quitting --- unable to find '%s'\n",word);
	parseError = true;
	return 0;
}

JOINT::JOINT(char *joint_name)
{

	STR childName;///String called childname

	strcpy(name,joint_name);///copies contents of joint_name eg Hips to private string name
//	printf("Creating joint '%s'\n",name);
	endSite = 0;
	nchildren = 0;
	transJoint = 0;
	chOffset = 0;
	xprevious = yprevious = zprevious = accumulatedmotion = 0;///used to keep person in straight line. Accumulates the distance in one direction


	Mocap.expect("{");///checks its there
	Mocap.nextline();///reads in next line

	Mocap.expect("OFFSET");///checks its there
	sscanf(Mocap.line,"OFFSET %f %f %f\n",&offset[0], &offset[1], &offset[2]);///reads the offsets into the private offset variables
	updateMaxoffset();///updates the offset for when the screen is being drawn in mocap draw
	Mocap.nextline();///reads the next line

	Mocap.expect("CHANNELS");///checks its there
	sscanf(Mocap.line,"CHANNELS %d %s %s %s %s %s %s",&nchannels,
		 &chtype[0], &chtype[1], &chtype[2], &chtype[3], &chtype[4], &chtype[5]);///store the number of channels and the channel types

	if (nchannels==6) {///this is just to flag the joint as having xyz translations
		if (strcmp(chtype[0],"Xposition") || strcmp(chtype[1],"Yposition") || strcmp(chtype[2],"Zposition")) {
			printf("translation order!!: '%s'\n",Mocap.line);
			Mocap.parseError = true;
			return;
		}///the if is just to ensure the channels are in the right order
		transJoint = true;                // flag this joint as having XYZ translations
	}

	check_channel_names();                // ensure all rotational channels are correctly named
	chOffset = Mocap.nchannels;             // record starting channel number
	///making the channel offset (ie which joint it is we are actually talking about)
	/// equal to the channel we are on right now.

	Mocap.nchannels += nchannels;
	Mocap.nextline();

	while (1) {///reads on and checks if it has any children then recursively
		if (!strncmp(Mocap.line,"JOINT",5)) {///asking if not different then (up to 5 characters
			sscanf(Mocap.line,"JOINT %s",childName);///reads in the childs name
			Mocap.nextline();///reads the next line
			child[nchildren] = new JOINT(childName);///creates a joint in this joints array of children (up to 5 children per joint..... but remember each child can have their own children)
			nchildren++;
			continue;
		} else if (!strncmp(Mocap.line,"End Site",8)) {///if not different to end site
			Mocap.nextline();
			Mocap.expect("{");
			Mocap.nextline();
			Mocap.expect("OFFSET");
			sscanf(Mocap.line,"OFFSET %f %f %f",&endLength[0], &endLength[1], &endLength[2]);
			endSite = 1;
			Mocap.nextline();
			Mocap.expect("}");
			Mocap.nextline();
			break;
		} else {
			break;
		}
	}
	Mocap.expect("}");
//	printf("Completing joint %s\n",name);
	Mocap.nextline();
}


//  updates maximum joint offset value
void JOINT::updateMaxoffset()
{
	for (int d=0; d<3; d++) {
		float absval = float(fabs(offset[d]));
		if (absval > Mocap.maxOffset)
			Mocap.maxOffset = absval;
	}
}




// recursively draws joints
void JOINT::draw(float *frameData,float *data, float *specialdata, int framenumber)
{
	/*
	Basic process is  draw the joint from start posision of where we are at (either
	at the root or at the the start
	 point of the last joint. After we draw the joint
	we then translate out to the end of the joint we just drew

	*/
	if(frameData==data)
	{
	///in this case the frame pointer is the same as the data pointer
	/// we then assume we are at the first frame so we shall reset
	///accumulatedmotion to zero
		accumulatedmotion=0;
	}
	glPushMatrix();
	int nc = 0;       // number of channels already processed

		// draw line from parent joint to current joint
	glLineWidth(3);
	glColor3f(0,0,1);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(offset[0],offset[1],offset[2]);
	glEnd();

      // apply joint offset
	glTranslatef(offset[0],offset[1],offset[2]);
	  // apply translation channels (if any)
	if (transJoint) {/// means the joint has translations from their starting positions as well as rotations
		float x;
		float y;
		float z;

		if(usespecialroottranslation)
		{///here we are doing a graph loop and we need a special root translation
		///I could actually remove this and simply place it at the bottom instead of
		///frameData as they are equal when the special data isnt being used
			x = specialdata[0];
			y = specialdata[1];
			z = specialdata[2];

		}

		if(usestraightwalkroottranslation)
		{
				///this overrides usespecialroottranslation above.
				///Used to make character walk in straight line

			if(frameData==data)
			{
				///here the pointers are equal so we are at the first frame.
				///we shall just use the data from the first frame
				x = data[0];
				y = data[1];
				z = data[2];
				xprevious=data[0];
				yprevious=data[1];
				zprevious=data[2];

			}
			else
			{
				///Im just gonna use specialdata here as it is equal to frameData when it isnt being used anyway
				x = specialdata[chOffset];
				z = specialdata[chOffset+2];
				accumulatedmotion += sqrt(pow((x-xprevious),2)+pow((z-zprevious),2));

				///we now set xprevious ans zprevious equal to the values from the current frame data
				xprevious = x;
				zprevious = z;

				//now set x,y and z equal to their final values
				if(ispositive&&!isxdirection)
				{
					///this case is +ve z
					z = accumulatedmotion;
					x = data[0];//fix x to what it was in the first instance.
					y = specialdata[chOffset+1]; ///y should be as normal
				}
				else if(ispositive&&isxdirection)
				{
					///this case is +ve x
					z = data[2];//fix z to what it was at first
					x = accumulatedmotion;
					y = specialdata[chOffset+1]; ///y should be as normal
				}
				else if(!ispositive&&isxdirection)
				{
					///this case is -ve x
					z = data[2];
					x = -accumulatedmotion;
					y = specialdata[chOffset+1]; ///y should be as normal
				}
				else if(!ispositive&&!isxdirection)
				{
					///this case is -ve z
					z = -accumulatedmotion;
					x = data[0];//fix x to what it was in the first instance.
					y = specialdata[chOffset+1]; ///y should be as normal
				}

			}

		}
		else if(dontaccumulateroot)
		{

		///in this case we are not going to have the root accumulate translations
		///so we always take
			x = data[0];
			y = data[1];
			z = data[2];
		}
		else if(!usespecialroottranslation)
		{
			x = frameData[chOffset];
			y = frameData[chOffset+1];
			z = frameData[chOffset+2];

		}
		glTranslatef(x,y,z);
		nc += 3;


	}
	  // apply rotations
	for (int r=nc; r<nchannels; r++) {

		if(transJoint&&dontaccumulateroot)
		{///transjoint implies a joint with translations from the starting position, so if this is the case
		///we assume if "dontaccumulateroot" is true then we dont want rotations accumulating.
			///this is the angle to be rotated
			///as you can see, because we dont want it acummulating we take the angle from data
			/// and not frame data
			float th = data[chOffset+r];

			switch(chtype[r][0])
			{
			case 'X':
				glRotatef(th,1,0,0);
				break;
			case 'Y':
				glRotatef(th,0,1,0);
				break;
			case 'Z':
				glRotatef(th,0,0,1);
				break;
			default:
				printf("ch type '%c': error\n",chtype[r][0]);
				return;
			}


		}
		else
		{
			float th = frameData[chOffset+r];///this is the angle to be rotated
			switch(chtype[r][0])
			{
			case 'X':
				glRotatef(th,1,0,0);
				break;
			case 'Y':
				glRotatef(th,0,1,0);
				break;
			case 'Z':
				glRotatef(th,0,0,1);
				break;
			default:
				printf("ch type '%c': error\n",chtype[r][0]);
				return;
			}
		}
	}

	glColor3f(1,0,0);

	///Here I have put an X at a joint
	glPushMatrix();
	mySolidCylinder( 0.4, 2, 2, 3 );
	glRotatef(90,1,0,0);
	mySolidCylinder( 0.4, 2, 2, 3 );
	glRotatef(90,1,0,0);
	mySolidCylinder( 0.4, 2, 2, 3 );
	glRotatef(90,1,0,0);
	mySolidCylinder( 0.4, 2, 2, 3 );
	glPopMatrix();

	  // draw line from joint to end site (if necessary)
	if (endSite) {
		glLineWidth(3);
		glColor3f(0,0,1);
		glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(endLength[0],endLength[1],endLength[2]);
		glEnd();
	}

	  // recursively draw all child joints
	for (int n=0; n<nchildren; n++)
		child[n]->draw(frameData, data, specialdata, framenumber);

	glPopMatrix();///we dont pop until **after** we have drawn all the child joints.
}




void JOINT::check_channel_names()
{
	int startChannel = 0;
	if (transJoint)                // translations already checked
		startChannel = 3;
	for (int c=startChannel; c<nchannels; c++) {
		if (strcmp(chtype[c],"Xrotation") &&
			strcmp(chtype[c],"Yrotation") &&
			strcmp(chtype[c],"Zrotation"))
			printf("Unrecognized channel type: '%s'\n",chtype[c]);
			Mocap.parseError = true;///only way the if is true, is if all of them return a number. But a number is only returned if they are different, so at least 1 will be 0
	}
}
