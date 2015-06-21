// A link Button.
// Gets used in the Links Screen.
//===============================

#ifndef HD_HUD_LINKOBJECT_H
#define HD_HUD_LINKOBJECT_H

#include "../../../UI_Objects/HD_UI_Container.h"

class HD_HUD_LinkObject : public HD_UI_Container
{
private:
	char* ip;
	bool textColorsSet = false;
	void connectClick();
	void addToFavoritesClick();
	void addToMapClick();
	void colorOnMapClick();
	void removeClick();

	HD_HUD_LinkObject() {} // don't want this to be created without the needed info

	void createGenericGfx();

public:
	HD_HUD_LinkObject(const char* objectName, char* ip);
	~HD_HUD_LinkObject() {}

	void CreatePlayerLink();
	void CreateGenericLink();
	void Update();

	char* GetIP(){ return ip; }
};

#endif