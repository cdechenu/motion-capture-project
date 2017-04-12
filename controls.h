#ifndef CONTROLS__H
#define CONTROLS__H
#include "glow.h"
#include "glowQuickPalette.h"
#include "Vector.h"
#include "matrix.h"
//#include "mocap.h"

//#include "thresholdcontrols.h"
#include "graphdraw.h"
GLOW_NAMESPACE_USING




class Controls :
	public GlowPushButtonReceiver,
	public GlowSliderReceiver,
	public GlowCheckBoxReceiver
{
	public:

		Controls(
			int mainWindowID,
			MOCAP *mocappointer);
		virtual ~Controls();

	protected:


		virtual void OnMessage(
			const GlowPushButtonMessage& message);

		virtual void OnMessage(
			const GlowSliderMessage& message);


		virtual void OnMessage(
			const GlowCheckBoxMessage& message);

	private:


		int _mainWindowID;
		MOCAP* _mocappointer;

		// Pointer to the control window itself
		GlowQuickPaletteWindow* _controlWindow;

		GlowPushButtonWidget* _quitButton;
		GlowPushButtonWidget* _playButton;
		GlowPushButtonWidget* _upButton;
		GlowPushButtonWidget* _downButton;
		GlowPushButtonWidget* _leftButton;
		GlowPushButtonWidget* _rightButton;
		GlowPushButtonWidget* _forwardButton;
		GlowPushButtonWidget* _backButton;
		GlowPushButtonWidget* _xButton;
		GlowPushButtonWidget* _yButton;
		GlowPushButtonWidget* _zButton;
		GlowPushButtonWidget* _gnuplotButton;
		GlowPushButtonWidget* _AcalcButton;
		GlowPushButtonWidget* _BcalcButton;
		GlowPushButtonWidget* _CcalcButton;
		GlowPushButtonWidget* _EcalcButton;
		GlowPushButtonWidget* _FcalcButton;
		GlowPushButtonWidget* _graphwalkButton;
		GlowPushButtonWidget* _graphloopButton;
		GlowPushButtonWidget* _resetButton;
		GlowPushButtonWidget* _zdirButton;
		GlowPushButtonWidget* _xdirButton;
		GlowPushButtonWidget* _dottyButton;


		int previousyawvalue;///lets me know whether we are yawing up or down.



		GlowPushButtonWidget* _drawgraphButton;
		GlowSliderWidget* _frameSlider;
		GlowSliderWidget* _yawSlider;
		GlowSliderWidget* _pitchSlider;
		GlowSliderWidget* _rollSlider;
		GlowCheckBoxWidget* _rootCheckbox;
		GlowCheckBoxWidget* _rootCheckbox2;
		GlowCheckBoxWidget* _directionCheckbox;
		GlowCheckBoxWidget* _discardOutliers;

};


#endif
