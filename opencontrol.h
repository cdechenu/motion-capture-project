
#ifndef OPENCONTROL__H
#define OPENCONTROL__H

#include "glow.h"
#include "glowQuickPalette.h"
#include "controls.h"
#include "glowTextFieldWindow.h"
GLOW_NAMESPACE_USING




class OpenControl :
	public GlowTextFieldWindowReceiver
{
	public:

		OpenControl(
			int mainWindowID,
			MOCAP *mocappointer);
		virtual ~OpenControl();

	protected:
		virtual void OnMessage(
			const GlowTextFieldWindowMessage& message);

	private:

		// Main drawing window id
		int _mainWindowID;
		MOCAP* _mocappointer;

		// Pointer to the control window itself
		GlowQuickPaletteWindow* _controlWindow;


};


#endif
