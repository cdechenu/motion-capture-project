//mocap.h
#include "def.h"
class MOCAP {
private:
	float dt, xmin,xmax,ymin,ymax,zmin,zmax,maxOffset,drawScale,drawCenter[3],sphereSize, maxerror,*data;
	float *graphdata;	///this will contain the data for the motiongraph
	float minerror;		///contain the min error value
	bool parseError;
	int nchannels,drawRepeat,threshold;		///the threshold for accepting a local min
	int lastframedrawn;	///stored as the last frame that was drawn for the display callback
	bool graphcalculated; ///has data for the graph been calculated, need for threshold controls.
	LONGSTR line;
	STR name;
	FILE *fp;
public:
	MOCAP();                      // constructor
	void graphWalk();
	void graphLoop();
	void calculateGraphMethodA();	    /// calculate the data for the error graph.
	void calculateGraphMethodB();	    /// calculate the data for the error graph.
	void calculateGraphMethodC();
	void calculateGraphMethodE();
	void calculateGraphMethodF();
	void gnuPlot();				///plot the graph with gnuplot
	void Dotty();				///plot the actual motion graph (directed graph)
	float getErrorData(int framex, int framey); ///return data from the error graph array
	float getMaxError();
	float getMinError();
	int getThreshold();
	bool graphCalculated();   ///has the graph data been calculated
	void setThreshold(int thresh); ///threshold
	void computeBbox();         // computes bbox for data
	void drawFrame(int f);      // draws the given frame
	int lastFrameDrawn();	    ///returns the last frame drawn
	void background();          // draws ground plane
	char currView;            // current viewing axis ('x', 'y', or 'z')
	class JOINT *root;        // root joint
	int nframes;              // number of frames in sequence
	int read(const char *fname);      // reads in a MOCAP file
	int expect(char *word);     // verifies first word on the line
	int lookfor(char *word);    // scans until line is found with the given first word
	int graphwalk[2];	///temporary graph walk stores the two int frames we want to walk
	int nextline();             // read in next line
	bool innerwalk;			///checks if the graph walk should be a loop of inside data
	friend class JOINT;
	void play();                // plays back the file
	void playframeloop(int x);   ///plays frame on a loop
	int getNchannels() { return nchannels; };
	inline int errorSize() const; ///used to get the size of the error graph
};

inline int MOCAP::errorSize() const
{
	return nframes;
}

const int MAX_CHILDREN = 5;

class JOINT {
private:
	STR name;
	float offset[3];
	int nchannels;
	STR chtype[6];
	int chOffset;
	bool transJoint;
	bool endSite;
	float endLength[3];
	float accumulatedmotion;	///for accumulating motion in one direction.
	float xprevious;
	float yprevious;
	float zprevious;			///all of these used to calculate vector for straight line movement

public:
	JOINT(char *name);
	JOINT *child[MAX_CHILDREN];
	int nchildren;
	void check_channel_names();
	void updateMaxoffset();
	void draw(float *frameData, float *data,float *specialdata, int framenumber);
};
struct twoints {
  int i;
  int j;
} ;

extern MOCAP Mocap;

