//========================================
// Layout test used for debugging and
// what not.
//========================================

#ifndef HD_UI_TESTLAYOUT_H
#define HD_UI_TESTLAYOUT_H

#include "../UI_Objects/HD_UI_Container.h"

class HD_UI_TestLayout : public HD_UI_Container
{
public:
	HD_UI_TestLayout();
	~HD_UI_TestLayout();

	void Create();
};

#endif