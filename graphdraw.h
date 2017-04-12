#ifndef GRAPHDRAW__H
#define GRAPHDRAW__H
#include "glow.h"
#include "mocap.h"
#include "glowQuickPalette.h"

GLOW_NAMESPACE_USING

class MandelData;

// Notice the class inherits from GlowWindow. All toplevel window objects
// must inherit from this class.
class GraphDraw :
	public GlowWindow,
	public GlowPushButtonReceiver,
	public GlowSliderReceiver
	//public GlowCheckBoxReceiver
{
	public:

		GraphDraw(
			MOCAP* mocappointer);
		virtual ~GraphDraw();

	protected:
		virtual void OnMessage(
			const GlowPushButtonMessage& message);
		virtual void OnMessage(
			const GlowSliderMessage& message);


		// This method is called when the window needs to be redrawn.
		virtual void OnEndPaint();


		virtual void OnMouseDown(
					Glow::MouseButton button,
					int x,
					int y,
					Glow::Modifiers modifiers);
				virtual void OnMouseUp(
					Glow::MouseButton button,
					int x,
					int y,
					Glow::Modifiers modifiers);
				virtual void OnMouseDrag(
					int x,
			int y);

	private:

		MOCAP* _mocappointer;
		unsigned char* _image;
		char calcmethod;

		// Pointer to the control window itself
		GlowQuickPaletteWindow* _controlWindow;

		// Pointers to selected widgets. We store these so that we can tell
		// what widget was hit when we receive events.
		GlowPushButtonWidget* _quitButton;
		GlowPushButtonWidget* _calcA;
		GlowPushButtonWidget* _calcB;
		GlowPushButtonWidget* _createDirectedGraph;
		//GlowPushButtonWidget* _playButton;
		GlowSliderWidget* _thresholdSlider;
		//GlowCheckBoxWidget* _rootCheckbox;
};



#endif
