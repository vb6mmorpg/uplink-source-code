//========================================
// Layout test used for debugging and
// what not.
//========================================

#ifndef HD_TESTLAYOUT_H
#define HD_TESTLAYOUT_H

#include "../UI_Objects/HD_UI_Container.h"

class HD_TestLayout : public HD_UI_Container
{
public:
	HD_TestLayout();
	~HD_TestLayout();

	void Create();
};

#endif