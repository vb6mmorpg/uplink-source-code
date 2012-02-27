
#ifndef _included_numbergenerator_h
#define _included_numbergenerator_h


class NumberGenerator
{

public:

	static void Initialise ();


	static int RandomNumber ( int range );								
				// result ~ U ( 0, range ),      0 <= result < range
	

	static float RandomNormalNumber ( float mean, float range );		
				// result ~ N ( mean, range/3 ), mean - range < result < mean + range	
  
	static float RandomUniformNumber ();

	static int ApplyVariance ( int num, int variance );					
				// Applies +-percentage variance to num
		

};


#endif
