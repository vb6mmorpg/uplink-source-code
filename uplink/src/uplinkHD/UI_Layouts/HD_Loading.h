// Loading screen before the HUD.
// Kinda primitive.
//===============================

#ifndef HD_LOADING_H
#define HD_LOADING_H

#include "../UI_Objects/HD_UI_Container.h"

#include "../UI_Objects/HD_UI_GraphicsObject.h"
#include "../UI_Objects/HD_UI_TextObject.h"

class HD_Loading : public HD_UI_Container
{
protected:
	std::shared_ptr<HD_UI_TextObject> captionObject = nullptr;
	std::string caption;
	std::string nextLayout;

	bool stopTimer = false;
	float screenTimer = 0.0f;
	float loadingTime = 3.0f;

	HD_Loading() {}

public:
	HD_Loading(const char* textCaption, const char* newLayout, float wait = 3.0f);
	~HD_Loading() {}

	void Create();
	void Update();

	//loading animation listener. USED ONLY HERE!
	class LoadingComplete : public CDBTweener::IListener
	{
	public:
		void onTweenFinished(CDBTweener::CTween *pTween);
	} loadingComplete;
};

#endif