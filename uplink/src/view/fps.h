
/*
  
  Frame rate checker utility class
  By Christopher Delay
  
  */

#ifndef _included_fps_h
#define _included_fps_h



#define NUMFRAMES 30				// Higher = more accurate, less frequent updates


class FrameRate
{
    
protected:
    
    int starttime;
    int numframesremaining;        // The number still to be timed
    float fps;                     
    
public:
    
    FrameRate ();
    ~FrameRate ();
    
	void Start ();
    void Tick ();
    
    float FPS ();
    
};

#endif
