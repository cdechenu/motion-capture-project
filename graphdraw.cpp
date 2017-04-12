#include "graphdraw.h"
#include <math.h>


extern twoints localmins[500];
extern int localmincounter;
bool discardOutliers=false;

GraphDraw::GraphDraw(
	MOCAP* mocappointer) :
// We first construct the window itself.
GlowWindow("Graph", GlowWindow::autoPosition, GlowWindow::autoPosition,
	mocappointer->errorSize(), mocappointer->errorSize(), Glow::rgbBuffer | Glow::doubleBuffer, Glow::mouseEvents | Glow::dragEvents)///I have removed no events thing
{
	// Get data
	_mocappointer = mocappointer;
	_image = 0;
	calcmethod='a';
	_mocappointer = mocappointer;

	// Create a control panel window
	_controlWindow = new GlowQuickPaletteWindow("Controls");

	_controlWindow->AddLabel(
		"Threshold");

	GlowQuickPanelWidget* hpanel = _controlWindow->AddArrangingPanel(
		GlowQuickPanelWidget::horizontal);

	GlowQuickPanelWidget* panel = hpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "Main",
		GlowQuickPanelWidget::vertical);

	_quitButton = panel->AddPushButton("Close", this);

	_calcA = panel->AddPushButton("Nearest Neighbor", this);
	_calcB = panel->AddPushButton("Partial", this);

	_createDirectedGraph = panel->AddPushButton("Create Directed Graph", this);

        	_thresholdSlider = panel->AddSlider(_mocappointer->getMinError(), _mocappointer->getMaxError(),_mocappointer->getThreshold(),
    GlowSliderWidget::defaultOptions, 2, "%.0f", "Threshold:\n%.0f", this);

    	// A separator
	panel->AddSeparator();


	// Arrange controls and show the control panel window
	_controlWindow->Pack();




	//int errorsize = _mocappointer->errorSize();
	//_data->Recalc();
	// Make image
	//_image = new unsigned char[errorsize*errorsize*4];
	//const int* rawimage = _data->Data();
	//int mx = errorsize*errorsize;
	/*for (int i=0; i<mx; ++i)
	{
			if(i<(mx/2)){
			// black color
			_image[i*4] = 0;
			_image[i*4+1] = 0;
			_image[i*4+2] = 0;
			_image[i*4+3] = 0;
			}else
			{//red
			_image[i*4] = 255;
			_image[i*4+1] = 0;
			_image[i*4+2] = 0;
			_image[i*4+3] = 0;


			}
	}*/

	/*///do some calculations for normalisation
	float errorlength = _mocappointer->getMaxError()-_mocappointer->getMinError();

	printf("the errorlength is: %f which is %f - %f",errorlength,_mocappointer->getMaxError(),_mocappointer->getMinError());

	for (int i=0; i<errorsize; ++i)
	{
		for (int j=0; j<errorsize; ++j)
		{


			float value;
			float localmin;
			float localmax;

			///first we get the error value for that point.
			value = _mocappointer->getErrorData(i,j);
			///now we must check it against its neighbours for local max or min.
			///first set them equal to our point.
			localmax = localmin = value;

			///now check nearest neighbours
			for (int k=i-2; k<=i+2; k++)
			{
				for (int l=j-2; l<=j+2; l++)
				{
					float errordata = _mocappointer->getErrorData(k,l);
					///dont perform the calculation if either j or k are out of bounds.
					if(k>=0&&k<=errorsize&&j>=0&&j<=errorsize)
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

			int imagevalue = round((value-_mocappointer->getMinError())*(255.00/errorlength));

			///sp that black is max:

			imagevalue= 255-imagevalue;
			if(value==localmin&&value<_mocappointer->getThreshold())
			{

			_image[(j*errorsize+i)*4] = 255;
			_image[(j*errorsize+i)*4+1] = 0;
			_image[(j*errorsize+i)*4+2] = 0;
			_image[(j*errorsize+i)*4+3] = 0;


			}
			else
			{

			_image[(j*errorsize+i)*4] = imagevalue;
			_image[(j*errorsize+i)*4+1] = imagevalue;
			_image[(j*errorsize+i)*4+2] = imagevalue;
			_image[(j*errorsize+i)*4+3] = imagevalue;
			}



			/*if(value==localmax)
			{
			// black color
			_image[(j*errorsize+i)*4] = 0;
			_image[(j*errorsize+i)*4+1] = 0;
			_image[(j*errorsize+i)*4+2] = 0;
			_image[(j*errorsize+i)*4+3] = 0;

			}
			else if(value==localmin)
			{

			// white color
			_image[(j*errorsize+i)*4] = 255;
			_image[(j*errorsize+i)*4+1] = 255;
			_image[(j*errorsize+i)*4+2] = 255;
			_image[(j*errorsize+i)*4+3] = 255;

			}
			else
			{
			// red color
			_image[(j*errorsize+i)*4] = 255;
			_image[(j*errorsize+i)*4+1] = 0;
			_image[(j*errorsize+i)*4+2] = 0;
			_image[(j*errorsize+i)*4+3] = 0;


			}*/













		/*
			//if((i*errorsize+j)<(mx/2)&&((i*errorsize+j)>100))
			if((i<errorsize/4)&&(j<errorsize/2)&&(j%2==0))
			{
			// red color
			_image[(j*errorsize+i)*4] = 255;
			_image[(j*errorsize+i)*4+1] = 0;
			_image[(j*errorsize+i)*4+2] = 0;
			_image[(j*errorsize+i)*4+3] = 0;
			}else
			{
			_image[(j*errorsize+i)*4] = 255;
			_image[(j*errorsize+i)*4+1] = 255;
			_image[(j*errorsize+i)*4+2] = 255;
			_image[(j*errorsize+i)*4+3] = 255;


			}*/



		//}

	//}
}


GraphDraw::~GraphDraw()
{
	delete _controlWindow;
}



void GraphDraw::OnEndPaint()
{
	//first set the counter to zero.
	localmincounter=0;

	int errorsize = _mocappointer->errorSize();
	delete[] _image;
	_image = new unsigned char[errorsize*errorsize*4];

	///do some calculations for normalisation
	float errorlength = _mocappointer->getMaxError()-_mocappointer->getMinError();

	//printf("the errorlength is: %f which is %f - %f",errorlength,_mocappointer->getMaxError(),_mocappointer->getMinError());

	for (int i=0; i<errorsize; ++i)
	{
		for (int j=0; j<errorsize; ++j)
		{


			float value;
			float localmin;
			float localmax;

			///first we get the error value for that point.
			value = _mocappointer->getErrorData(i,j);

			if(calcmethod=='a')
			{
				///this is the nearest neighbour method controlled from within the error graphs controls

				///now we must check it against its neighbours for local max or min.
				///first set them equal to our point.
				localmax = localmin = value;

				///now check nearest neighbours
				for (int k=i-2; k<=i+2; k++)
				{
					for (int l=j-2; l<=j+2; l++)
					{
						float errordata = _mocappointer->getErrorData(k,l);
						///dont perform the calculation if either j or k are out of bounds.
						if(k>=0&&k<=errorsize&&j>=0&&j<=errorsize)
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
			}
			else if(calcmethod=='b')
			{
				//this is the partials method

				if(i>0&&i<errorsize&&j>0&&j<errorsize)///ie withing the bounds
				{
					float gradient1,gradient2,gradient3,gradient4;
					gradient1=_mocappointer->getErrorData(i,j)-_mocappointer->getErrorData(i-1,j);
					gradient2=_mocappointer->getErrorData(i+1,j)-_mocappointer->getErrorData(i,j);
					gradient3=_mocappointer->getErrorData(i,j)-_mocappointer->getErrorData(i,j-1);
					gradient4=_mocappointer->getErrorData(i,j+1)-_mocappointer->getErrorData(i,j);

					if((gradient1>0&&gradient2<0)&&(gradient3>0&&gradient4<0))///ie from +ve to -ve
					{
						localmin = value;
					}
					else
					{
						localmin=value+1;///just so it != value
					}

				}
			}
			int imagevalue;

			if(!discardOutliers)
			{
				imagevalue = round((value-_mocappointer->getMinError())*(255.00/errorlength));
			}else{
				imagevalue = round((value-_mocappointer->getMinError())*(255.00/20));
			}


			///so that black is max:
			imagevalue= 255-imagevalue;
			if(value==localmin&&value<_mocappointer->getThreshold())
			{

				_image[(j*errorsize+i)*4] = 255;
				_image[(j*errorsize+i)*4+1] = 0;
				_image[(j*errorsize+i)*4+2] = 0;
				_image[(j*errorsize+i)*4+3] = 0;

				if((localmincounter<500-2)&&(i!=j)&&i!=0&&j!=0)
				{
					/// now to add value for the drawing of the graph.
					localmins[localmincounter].i=i;
					localmins[localmincounter].j=j;
					localmincounter++;
				}
			}
			else if(imagevalue<256)//the if part of this is only used when we are discarding outliers for the graph
			{

				_image[(j*errorsize+i)*4] = imagevalue;
				_image[(j*errorsize+i)*4+1] = imagevalue;
				_image[(j*errorsize+i)*4+2] = imagevalue;
				_image[(j*errorsize+i)*4+3] = imagevalue;
			}
			else //else we just make it black as this is an outlier
			{

				_image[(j*errorsize+i)*4] = 0;
				_image[(j*errorsize+i)*4+1] = 0;
				_image[(j*errorsize+i)*4+2] = 0;
				_image[(j*errorsize+i)*4+3] = 0;
			}



		}

	}

	localmins[localmincounter].i=_mocappointer->errorSize();//this is put into cap off the array with the last frame

	::glRasterPos2f(-1.0f, -1.0f);
	::glDrawPixels(_mocappointer->errorSize(), _mocappointer->errorSize(), GL_RGBA, GL_UNSIGNED_BYTE, _image);
}

// Respond to mouse down events

void GraphDraw::OnMouseDown(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("GraphDraw::OnMouseDown");

	printf("x is %d and y is %d",x,y);

	if(button==GLUT_RIGHT_BUTTON)
	{
		_mocappointer->innerwalk=true;
		_mocappointer->graphwalk[0]=x;
		_mocappointer->graphwalk[1]=_mocappointer->errorSize()-y;//as the y direction is bottom to top not top to bottom

		FILE *f = fopen("test.dot","w");/*This opens the file for writing ("w")*/
		fprintf(f,"digraph motiongraph\n{\n%d -> %d;\n%d -> %d;\n}",_mocappointer->graphwalk[0],_mocappointer->graphwalk[1],_mocappointer->graphwalk[1],_mocappointer->graphwalk[0]);

		fclose(f);/*And here we close the file*/

	}
	else if(button==GLUT_LEFT_BUTTON)
	{
		_mocappointer->innerwalk=false;
		_mocappointer->graphwalk[0]=x;
		_mocappointer->graphwalk[1]=_mocappointer->errorSize()-y;//as the y direction is bottom to top not top to bottom

		///swap them around for the motion graph
		if(_mocappointer->graphwalk[1]<_mocappointer->graphwalk[0])
		{
			int temp = _mocappointer->graphwalk[0];
			_mocappointer->graphwalk[0]=_mocappointer->graphwalk[1];
			_mocappointer->graphwalk[1]=temp;

		}

		FILE *f = fopen("test.dot","w");/*This opens the file for writing ("w")*/
		fprintf(f,"digraph motiongraph\n{\n0 -> %d;\n%d -> %d;\n}",_mocappointer->graphwalk[0],_mocappointer->graphwalk[1],_mocappointer->errorSize());

		fclose(f);/*And here we close the file*/
	}

}


// Respond to mouse up events

void GraphDraw::OnMouseUp(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("GraphDraw::OnMouseUp");

//do nothing at the moment
}


// Respond to mouse drag events

void GraphDraw::OnMouseDrag(
	int x,
	int y)
{
	GLOW_DEBUGSCOPE("GraphDraw::OnMouseDrag");

//do nothing
}





// Receive slider events
void GraphDraw::OnMessage(
	const GlowSliderMessage& message)
{
	GLOW_DEBUGSCOPE("GraphDraw::OnMessage(slider)");

	// Check central sliders

	// Hip rotate
	if (message.widget == _thresholdSlider)
	{

		//glutSetWindow(_mainWindowID);
		_mocappointer->setThreshold(int(message.value));
		Refresh();
	}

	// Redraw scene
	//Glow::RefreshGlutWindow(_mainWindowID);
}

// Receive pushbutton events
void GraphDraw::OnMessage(
	const GlowPushButtonMessage& message)
{
	GLOW_DEBUGSCOPE("GraphDraw::OnMessage(pushbutton)");


	if (message.widget == _quitButton)
	{
		this->~GraphDraw();
	}
	if (message.widget == _calcA)
	{
		calcmethod='a';
	}
	if (message.widget == _calcB)
	{
		calcmethod='b';
	}
	if (message.widget == _createDirectedGraph)
	{
		FILE *f = fopen("test.dot","w");/*This opens the file for writing ("w")*/
		fprintf(f,"digraph motiongraph\n{\n");


		//First we wish to include the segment zeroth frame up to the first jump segment.

		// this is the point we wish to jump to
		int tempend=0;

		//If the point we are jumping to is listed twice (like 12,23 and 12,50) then we move onto the last
		//instance of it
		while(localmins[tempend].i==localmins[tempend+1].i)
		{
			tempend++;
		}
		fprintf(f,"\"%d,%d\"->\"%d,%d\";\n",0,localmins[0].i-1,localmins[tempend].i,localmins[tempend+1].i-1);




		for(int i=0;i<localmincounter-2;i++)
		{
			//the following ifstatement rules out segments of 0 distance like (12,12) etc
			if(localmins[i].i!=localmins[i+1].i)
			{
				int tempendi=i;
				while(localmins[tempendi+1].i==localmins[tempendi+2].i)
				{
					tempendi++;
					//the cycle the end of the segment up the chain to avoid jumps like 10,12->12,12
				}

				fprintf(f,"\"%d,%d\"->\"%d,%d\";\n",localmins[i].i,localmins[i+1].i-1,localmins[tempendi+1].i,localmins[tempendi+2].i-1);

			}
		}

		//Finally we add in the last segment
		fprintf(f,"\"%d,%d\"->\"%d,%d\";\n",localmins[localmincounter-2].i,localmins[localmincounter-1].i-1,localmins[localmincounter-1].i,_mocappointer->errorSize()-1);

		//The basic sequence of segments have now been output to a file.
		//Now we put in the jumps as defined by the local minima
		for(int i=0;i<localmincounter-1;i++)
		{
			//locate find the end of the segment we are jumping to
			int locate=localmincounter-1; //we set it = to localmincounter-1 as we will search backwards to get the last mention of the number
			while(((localmins[i+1].j)!=(localmins[locate].i))&&locate>-1)
			{
				locate--;
			}
			int start = i;

			while(localmins[start].i==localmins[start+1].i)
			{//so in this case there are >1 jumps from this frame, so we must find the correct segment to jump from
				start++;
			}

			//start should now be the begining of the first segment
			//we should now have located it. we want to jump to locate and locate+1 segement
			//note that i+1.j and locate.i are equal

			fprintf(f,"\"%d,%d\"->\"%d,%d\";\n",localmins[start].i,localmins[start+1].i-1,localmins[i+1].j,localmins[locate+1].i-1);

		}

		fprintf(f,"\n}");
		fclose(f);/*And here we close the file*/
	}


}

