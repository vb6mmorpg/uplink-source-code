#ifndef HD_HUD_LOGOBJECT_H
#define HD_HUD_LOGOBJECT_H

#include "../../../UI_Objects/HD_UI_Container.h"

class HD_HUD_LogObject : public HD_UI_Container
{
private:
	std::string sDate;
	std::string sDescription;
	bool textColorsSet = false;

protected:
	void logClick();

public:
	HD_HUD_LogObject() {}
	~HD_HUD_LogObject() {}

	void Create(const char* objectName, char* date, char* description);
	void Update();
};

#endif