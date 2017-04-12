#include "controls.h"
extern bool dontaccumulateroot;       //whether the root should accumulate motion.
extern bool usestraightwalkroottranslation;
extern bool isxdirection;
extern bool ispositive;
extern bool discardOutliers;
extern double i,j,l,a,b,c,r,t,q;
extern Vector v, CamUP, CamForward, CamRight;
extern matrixn3 rotation(const double parameter, const Vector axis);


Controls::Controls(
	int mainWindowID,
	MOCAP *Mocappointer)
{
	GLOW_DEBUGSCOPE("Controls::Controls");

	_mainWindowID = mainWindowID;
	_mocappointer = Mocappointer;
	previousyawvalue=0;

	// Create a control panel window
	_controlWindow = new GlowQuickPaletteWindow("Controls");

	_controlWindow->AddLabel(
		"Motion Graphs\n"
		"Christian de Chenu");

	GlowQuickPanelWidget* hpanel = _controlWindow->AddArrangingPanel(
		GlowQuickPanelWidget::horizontal);

	GlowQuickPanelWidget* panel1 = hpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "Controls",
		GlowQuickPanelWidget::vertical);

	GlowQuickPanelWidget* panel1z = panel1->AddPanel(
		GlowQuickPanelWidget::plainStyle, "",
		GlowQuickPanelWidget::horizontal);

	_upButton = panel1z->AddPushButton("Up", this);
	_downButton = panel1z->AddPushButton("Down", this);
	GlowQuickPanelWidget* panel1x = panel1->AddPanel(
		GlowQuickPanelWidget::plainStyle, "",
		GlowQuickPanelWidget::horizontal);
	_leftButton = panel1x->AddPushButton("Left", this);
	_rightButton = panel1x->AddPushButton("Right", this);


	GlowQuickPanelWidget* panel1y = panel1->AddPanel(
		GlowQuickPanelWidget::plainStyle, "",
		GlowQuickPanelWidget::horizontal);
	_forwardButton = panel1y->AddPushButton("Forward", this);
	_backButton = panel1y->AddPushButton("Back", this);


	GlowQuickPanelWidget* panel3 = panel1->AddPanel(
		GlowQuickPanelWidget::plainStyle, "Camera",
		GlowQuickPanelWidget::vertical);

	_yawSlider = panel3->AddSlider(-1,1, 0,
    GlowSliderWidget::defaultOptions, 2, "%.0f", "Yaw:\n%.0f", this);
    	_pitchSlider = panel3->AddSlider(-1,1, 0,
    GlowSliderWidget::defaultOptions, 2, "%.0f", "Pitch:\n%.0f", this);
    	_rollSlider = panel3->AddSlider(-1,1, 0,
    GlowSliderWidget::defaultOptions, 2, "%.0f", "Roll:\n%.0f", this);

    	GlowQuickPanelWidget* panel3x = panel3->AddPanel(
			GlowQuickPanelWidget::loweredStyle, "Camera",
		GlowQuickPanelWidget::horizontal);

    	_xButton = panel3x->AddPushButton("x", this);
		_yButton = panel3x->AddPushButton("y", this);
		_zButton = panel3x->AddPushButton("z", this);


	GlowQuickPanelWidget* panel2 = hpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "Main",
		GlowQuickPanelWidget::vertical);

	// Quit button
	_quitButton = hpanel->AddPushButton("Quit", this);
	_playButton = panel2->AddPushButton("Play", this);
	_resetButton = panel2->AddPushButton("Reset", this);





	_frameSlider = panel2->AddSlider(0, ((_mocappointer->nframes)-1), 0,
    GlowSliderWidget::defaultOptions, 2, "%.0f", "Frame number:\n%.0f", this);

    	GlowQuickPanelWidget* panel2y = panel2->AddPanel(
			GlowQuickPanelWidget::plainStyle, "calculations",
			GlowQuickPanelWidget::horizontal);

	_AcalcButton = panel2y->AddPushButton("Diff", this);
    _BcalcButton = panel2y->AddPushButton("SqrDiff", this);
    _CcalcButton = panel2y->AddPushButton("Diff^4", this);
    _EcalcButton = panel2y->AddPushButton("SqrDiff + Vel", this);
    _FcalcButton = panel2y->AddPushButton("SqrDiff + Acc", this);



	GlowQuickPanelWidget* panel2x = panel2->AddPanel(
		GlowQuickPanelWidget::plainStyle, "graphs",
		GlowQuickPanelWidget::horizontal);


    _drawgraphButton = panel2x->AddPushButton("Plot", this);
    _gnuplotButton = panel2x->AddPushButton("GnuPlot", this);
    _graphloopButton = panel2x->AddPushButton("GraphLoop", this);
    _graphwalkButton = panel2x->AddPushButton("GraphWalk", this);
    _dottyButton = panel2x->AddPushButton("Directed Graph", this);


    	/*_frameSlider = panel->AddSlider(_mocappointer->getMinError(), _mocappointer->getMaxError(),_mocappointer->getThreshold(),
    GlowSliderWidget::defaultOptions, 2, "%.0f", "Threshold:\n%.0f", this);*/

    	// A separator
	//panel->AddSeparator();

	// Toggle animation checkbox
	_rootCheckbox = panel2->AddCheckBox(
		"Accumulate Root Rotations and Translations", GlowCheckBoxWidget::on, this);

	_rootCheckbox2 = panel2->AddCheckBox(
		"Walk in Straight line", GlowCheckBoxWidget::off, this);

		GlowQuickPanelWidget* panel2z = panel2->AddPanel(
				GlowQuickPanelWidget::plainStyle, "Walk Direction:",
		GlowQuickPanelWidget::horizontal);

    _zdirButton = panel2z->AddPushButton("z dir", this);
    _xdirButton = panel2z->AddPushButton("x dir", this);
	_directionCheckbox = panel2z->AddCheckBox(
		"positive", GlowCheckBoxWidget::on, this);

		_discardOutliers = panel2->AddCheckBox(
		"Discard Outliers", GlowCheckBoxWidget::off, this);

	// Arrange controls and show the control panel window
	_controlWindow->Pack();
}


// Destructor

Controls::~Controls()
{
	delete _controlWindow;
}


// Receive pushbutton events
void Controls::OnMessage(
	const GlowPushButtonMessage& message)
{
	GLOW_DEBUGSCOPE("Controls::OnMessage(pushbutton)");


	if (message.widget == _quitButton)
	{
		exit(0);
	}


	if (message.widget == _playButton)
	{
		glutSetWindow(_mainWindowID);
		_mocappointer->play();

	}
	if (message.widget == _resetButton)
	{
		glutSetWindow(_mainWindowID);
		_mocappointer->drawFrame(0);

	}



	if (message.widget == _drawgraphButton)
	{
		new GraphDraw(_mocappointer);
	}
	if (message.widget == _gnuplotButton)
	{
			_mocappointer->gnuPlot();
	}

	if (message.widget == _dottyButton)
	{
			_mocappointer->Dotty();
	}




	if (message.widget == _upButton)
	{
		i+=CamUP.x();
		j+=CamUP.y();
		l+=CamUP.z();
		a+=CamUP.x();
		b+=CamUP.y();
		c+=CamUP.z();

	}
	if (message.widget == _downButton)
	{
		i-=CamUP.x();
		j-=CamUP.y();
		l-=CamUP.z();
		a-=CamUP.x();
		b-=CamUP.y();
		c-=CamUP.z();

	}
	if (message.widget == _leftButton)
	{
		i-=CamRight.x();
		j-=CamRight.y();
		l-=CamRight.z();
		a-=CamRight.x();
		b-=CamRight.y();
		c-=CamRight.z();

	}
	if (message.widget == _rightButton)
	{
		i+=CamRight.x();
		j+=CamRight.y();
		l+=CamRight.z();
		a+=CamRight.x();
		b+=CamRight.y();
		c+=CamRight.z();

	}

	if (message.widget == _forwardButton)
	{
		i+=5*CamForward.x();
		j+=5*CamForward.y();
		l+=5*CamForward.z();
		a+=5*CamForward.x();
		b+=5*CamForward.y();
		c+=5*CamForward.z();

	}
	if (message.widget == _backButton)
	{
		i-=5*CamForward.x();
		j-=5*CamForward.y();
		l-=5*CamForward.z();
		a-=5*CamForward.x();
		b-=5*CamForward.y();
		c-=5*CamForward.z();

	}
	if (message.widget == _xButton)
	{
		i=50.0,j=0.0,l=0.0,a=0.0,b=0.0,c=0.0,r=0.0,t=1.0,q=0.0;
	}
	if (message.widget == _yButton)
	{
		i=0.0,j=50.0,l=0.0,a=0.0,b=0.0,c=0.0,r=0.0,t=0.0,q=-1.0;
	}
	if (message.widget == _zButton)
	{
		i=0.0,j=0.0,l=50.0,a=0.0,b=0.0,c=0.0,r=0.0,t=1.0,q=0.0;
	}
	if (message.widget == _AcalcButton)
	{
		_mocappointer->calculateGraphMethodA();
	}
	if (message.widget == _BcalcButton)
	{
		_mocappointer->calculateGraphMethodB();
	}
	if (message.widget == _CcalcButton)
	{
		_mocappointer->calculateGraphMethodC();
	}
	if (message.widget == _graphloopButton)
	{
		glutSetWindow(_mainWindowID);
		_mocappointer->graphLoop();
	}
	if (message.widget == _graphwalkButton)
	{
		glutSetWindow(_mainWindowID);
		_mocappointer->graphWalk();
	}
	if (message.widget == _EcalcButton)
	{
		_mocappointer->calculateGraphMethodE();
	}

	if (message.widget == _FcalcButton)
	{
		_mocappointer->calculateGraphMethodF();
	}

	if (message.widget == _zdirButton)
	{
		isxdirection= false;
	}
	if (message.widget == _xdirButton)
	{
		isxdirection= true;
	}




	// Redraw scene
	Glow::RefreshGlutWindow(_mainWindowID);

}

// Receive slider events
void Controls::OnMessage(
	const GlowSliderMessage& message)
{
	GLOW_DEBUGSCOPE("Controls::OnMessage(slider)");


	if (message.widget == _frameSlider)
	{

		glutSetWindow(_mainWindowID);
		_mocappointer->playframeloop(int(message.value));
	}

	if (message.widget == _yawSlider)
	{

		//glutSetWindow(_mainWindowID);

		if(int(message.value)==-1)
		{
			a=a-i;
			b=b-j;
			c=c-l;
			v.set(a,b,c);
			v=rotation(-0.5,CamUP)*v;
			a=v.x();
			b=v.y();
			c=v.z();
			a=a+i;
			b=b+j;
			c=c+l;
		}
		else if(int(message.value)==1)
		{
			a=a-i;
			b=b-j;
			c=c-l;
			v.set(a,b,c);
			v=rotation(0.5,CamUP)*v;
			a=v.x();
			b=v.y();
			c=v.z();
			a=a+i;
			b=b+j;
			c=c+l;
		}

	}

	if (message.widget == _pitchSlider)
	{

		//glutSetWindow(_mainWindowID);

		if(int(message.value)==-1)
		{
			a=a-i;
			b=b-j;
			c=c-l;
			v.set(a,b,c);
			v=rotation(0.5,CamRight)*v;
			a=v.x();
			b=v.y();
			c=v.z();
			a=a+i;
			b=b+j;
			c=c+l;
		}
		else if(int(message.value)==1)
		{
			a=a-i;
			b=b-j;
			c=c-l;
			v.set(a,b,c);
			v=rotation(-0.5,CamRight)*v;
			a=v.x();
			b=v.y();
			c=v.z();
			a=a+i;
			b=b+j;
			c=c+l;
		}

	}

	if (message.widget == _rollSlider)
	{

		//glutSetWindow(_mainWindowID);

		if(int(message.value)==-1)
		{
			v.set(r,t,q);
			v=rotation(+0.5,CamForward)*v;
			r=v.x();
			t=v.y();
			q=v.z();
			CamUP.set(r,t,q);
		}
		else if(int(message.value)==1)
		{
			v.set(r,t,q);
			v=rotation(-0.5,CamForward)*v;
			r=v.x();
			t=v.y();
			q=v.z();
			CamUP.set(r,t,q);
		}

	}
	// Redraw scene
	Glow::RefreshGlutWindow(_mainWindowID);
}

// Receive checkbox events
void Controls::OnMessage(
	const GlowCheckBoxMessage& message)
{
	GLOW_DEBUGSCOPE("Controls::OnMessage(checkbox)");

	// Was it the toggle dontaccumlateroot checkbox?
	if (message.widget == _rootCheckbox)
	{
		if (message.state == GlowCheckBoxWidget::on)
		{
			dontaccumulateroot=false;
		}
		else
		{
			dontaccumulateroot=true;
		}
	}

	if (message.widget == _rootCheckbox2)
	{
		if (message.state == GlowCheckBoxWidget::on)
		{
			usestraightwalkroottranslation=true;
		}
		else
		{
			usestraightwalkroottranslation=false;
		}
	}
	if (message.widget == _directionCheckbox)
	{
		if (message.state == GlowCheckBoxWidget::on)
		{
			ispositive=true;
		}
		else
		{
			ispositive=false;
		}
	}
		if (message.widget == _discardOutliers)
		{
			if (message.state == GlowCheckBoxWidget::on)
			{
				discardOutliers=true;
			}
			else
			{
				discardOutliers=false;
			}
	}







}




