#include <iostream>
#include <cmath>
#include <fstream>

#include "opencontrol.h"
//#include "mocap.h"

extern void DisplayCallback();


// Constructor

OpenControl::OpenControl(
	int mainWindowID,
	MOCAP *Mocappointer)
{
	GLOW_DEBUGSCOPE("OpenControl::OpenControl");

	_mainWindowID = mainWindowID;
	_mocappointer = Mocappointer;


   new GlowTextFieldWindow("Open File", GlowWindow::autoPosition,
			GlowWindow::autoPosition, "Enter file name:", 300, "bvh/a.bvh",
			"OK\tCancel",this);
}


// Destructor

OpenControl::~OpenControl()
{
	//delete _controlWindow;
}



// Receive events from the save dialog
void OpenControl::OnMessage(
	const GlowTextFieldWindowMessage& message)
{
	// Only one text field window was created-- assume it's the save dialog

	// Bail out if the user pressed the cancel button
	if (message.response == 1) return;

	// Otherwise OK button (response == 0) was pressed...
	_mocappointer->read(message.text.c_str());

	new Controls(_mainWindowID,_mocappointer);
}



