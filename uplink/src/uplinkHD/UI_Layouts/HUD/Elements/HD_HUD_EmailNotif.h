//=======================================
// The E-mail notification.
// Shows how many e-mails the player has.
// Shows the last received e-mail in
// short form.
//=======================================

#ifndef HD_HUD_EMAILNOTIF_H
#define HD_HUD_EMAILNOTIF_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "../../../UI_Objects/HD_UI_TextObject.h"

class HD_HUD_EmailNotif : public HD_UI_Container
{
private:
	int messageCount = 0;
	int missionsCount = 0;

	float notifStayOnScreen = 5.0f; //how much does the notification stay on the screen (incl. animations)
	float notifTimeOnScreen = 0.0f;

	float iconStayStill = 5.0f; //how much does the notif icon stay still before doing a bounce
	float iconStayTimer = 0.0f;

	void setNotifTexts(bool isMission);
	void showNotification(bool show);
	void bounceIcon();

public:
	HD_HUD_EmailNotif();
	~HD_HUD_EmailNotif() {}

	void Create();
	void Update();
};

#endif