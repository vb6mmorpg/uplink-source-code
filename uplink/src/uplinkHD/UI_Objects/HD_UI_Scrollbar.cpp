#include "HD_UI_Scrollbar.h"

#include "../HD_ColorPalettes.h"
#include "../HD_Screen.h"

#include "HD_UI_Button.h"

//Protected
//=========
void HD_UI_Scrollbar::baseClick()
{
	std::shared_ptr<HD_UI_Object> handle = getChildByName("handle");

	if (isVertical)
	{
		if (HDScreen->mouse->GetState()->y > handle->GlobalY())
			moveHandle(stepSize);
		else
			moveHandle(-stepSize);
	}
	else
	{
		if (HDScreen->mouse->GetState()->x > handle->GlobalX())
			moveHandle(stepSize);
		else
			moveHandle(-stepSize);
	}
}

void HD_UI_Scrollbar::moveHandle(float distance, bool useAnimation)
{
	std::shared_ptr<HD_UI_Object> handle = getChildByName("handle");
	std::shared_ptr<HD_UI_Object> base = getChildByName("btnBase");
	CDBTweener::CTween *moveTween;

	float endPos = isVertical ? distance + handle->y : distance + handle->x;

	//clamp the distance
	if (isVertical)
	{
		if (endPos < base->y)
			endPos = base->y;
		else if (endPos + handle->height > base->y + base->height)
			endPos = base->y + base->height - handle->height;
	}
	else
	{
		if (endPos < base->x)
			endPos = base->x;
		else if (endPos + handle->width > base->x + base->width)
			endPos = base->x + base->width - handle->width;
	}

	if (useAnimation)
	{
		if (isVertical)
			moveTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.25f, &handle->y, endPos);
		else
			moveTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.25f, &handle->x, endPos);

		handle->addAnimation(moveTween, true);
	}
	else
	{
		if (isVertical)
			handle->y = endPos;
		else
			handle->x = endPos;
	}
}

//Public
//=========
void HD_UI_Scrollbar::CreateScrollbar(const char* objectName, float fX, float fY, float size, bool vertical)
{
	isVertical = vertical;

	if (isVertical)
	{
		setObjectProperties(objectName, fX, fY, 12.0f, size);

		//buttons
		std::shared_ptr<HD_UI_Button> btnUp = std::make_shared<HD_UI_Button>();
		btnUp->CreateImageButton("btnUp", "scrollbarArrowVU_s", "hud_atlas.png", "", 0.0f, 0.0f);
		addChild(btnUp);

		std::shared_ptr<HD_UI_Button> btnDown = std::make_shared<HD_UI_Button>();
		btnDown->CreateImageButton("btnDown", "scrollbarArrowVD_s", "hud_atlas.png", "", 0.0f, size - 12.0f);
		addChild(btnDown);

		std::shared_ptr<HD_UI_Button> btnBase = std::make_shared<HD_UI_Button>();
		btnBase->CreateRectangleButton("btnBase", "", "", 0.0f, 13.0f, 12.0f, size - 26.0f, palette->btnColors_blueSat, true);
		addChild(btnBase);
		btnBase->setCallback(std::bind(&HD_UI_Scrollbar::baseClick, this));

		//handle - the gfx gets added to a container for tidiness
		std::shared_ptr<HD_UI_GraphicsObject> handleTop = std::make_shared<HD_UI_GraphicsObject>();
		std::shared_ptr<HD_UI_GraphicsObject> handleMiddle = std::make_shared<HD_UI_GraphicsObject>();
		std::shared_ptr<HD_UI_GraphicsObject> handleBottom = std::make_shared<HD_UI_GraphicsObject>();
		handleTop->CreateImageObject("handleTop", "scrollbarHandleV_T", "hud_atlas.png", 0.0f, 0.0f);
		handleMiddle->CreateRectangleObject("handleMiddle", 0.0f, 8.0f, 8.0f, 8.0f, -1.0f, palette->bluesSat.cBlue2);
		handleBottom->CreateImageObject("handleBottom", "scrollbarHandleV_B", "hud_atlas.png", 0.0f, 16.0f);

		std::shared_ptr<HD_UI_Container> handleContainer = std::make_shared<HD_UI_Container>("handle", 2.0f, 13.0f, 8.0f, 24.0f);
		addChild(handleContainer);

		handleContainer->addChild(handleTop);
		handleContainer->addChild(handleMiddle);
		handleContainer->addChild(handleBottom);

		stepSize = btnBase->height * 0.1f;
		btnUp->setCallback(std::bind(&HD_UI_Scrollbar::moveHandle, this, -stepSize, true));
		btnDown->setCallback(std::bind(&HD_UI_Scrollbar::moveHandle, this, stepSize, true));
	}
	else
	{
		setObjectProperties(objectName, fX, fY, size, 12.0f);

		//buttons
		std::shared_ptr<HD_UI_Button> btnLeft = std::make_shared<HD_UI_Button>();
		btnLeft->CreateImageButton("btnLeft", "scrollbarArrowHL_s", "hud_atlas.png", "", 0.0f, 0.0f);
		addChild(btnLeft);

		std::shared_ptr<HD_UI_Button> btnRight = std::make_shared<HD_UI_Button>();
		btnRight->CreateImageButton("btnRight", "scrollbarArrowHR_s", "hud_atlas.png", "", 0.0f, size - 12.0f);
		addChild(btnRight);

		std::shared_ptr<HD_UI_Button> btnBase = std::make_shared<HD_UI_Button>();
		btnBase->CreateRectangleButton("btnBase", "", "", 13.0f, 0.0f, size - 26.0f, 12.0f, palette->btnColors_blueSat, true);
		addChild(btnBase);
		btnBase->setCallback(std::bind(&HD_UI_Scrollbar::baseClick, this));

		//handle - the gfx gets added to a container for tidiness
		std::shared_ptr<HD_UI_GraphicsObject> handleLeft = std::make_shared<HD_UI_GraphicsObject>();
		std::shared_ptr<HD_UI_GraphicsObject> handleMiddle = std::make_shared<HD_UI_GraphicsObject>();
		std::shared_ptr<HD_UI_GraphicsObject> handleRight = std::make_shared<HD_UI_GraphicsObject>();
		handleLeft->CreateImageObject("handleLeft", "scrollbarHandleV_T", "hud_atlas.png", 0.0f, 0.0f);
		handleMiddle->CreateRectangleObject("handleMiddle", 8.0f, 0.0f, 8.0f, 8.0f, -1.0f, palette->bluesSat.cBlue2);
		handleRight->CreateImageObject("handleRight", "scrollbarHandleV_B", "hud_atlas.png", 16.0f, 0.0f);

		std::shared_ptr<HD_UI_Container> handleContainer = std::make_shared<HD_UI_Container>("handle", 13.0f, 2.0f, 24.0f, 8.0f);
		addChild(handleContainer);

		handleContainer->addChild(handleLeft);
		handleContainer->addChild(handleMiddle);
		handleContainer->addChild(handleRight);

		stepSize = btnBase->width * 0.1f;
		btnLeft->setCallback(std::bind(&HD_UI_Scrollbar::moveHandle, this, -stepSize, true));
		btnRight->setCallback(std::bind(&HD_UI_Scrollbar::moveHandle, this, stepSize, true));
	}

	
}

void HD_UI_Scrollbar::Update()
{
	HD_UI_Container::Update();

	std::shared_ptr<HD_UI_Container> handle = getContainerByName("handle");
	std::shared_ptr<HD_UI_Object> base = getChildByName("btnBase");

	//mouse control
	if (handle->hasMouseFocus())
		followMouse = true;
	else
		followMouse = false;

	if (followMouse && isVertical)
		moveHandle(HDScreen->mouse->GetState()->y - handle->GlobalY(), true);
	else if (followMouse && !isVertical)
		moveHandle(HDScreen->mouse->GetState()->x - handle->GlobalX(), true);

	//scrollwheel
	if (HDScreen->mouse->GetState()->z > 0)
	{
		moveHandle(-stepSize);
		al_set_mouse_z(0);
	}
	else if (HDScreen->mouse->GetState()->z < 0)
	{
		moveHandle(stepSize);
		al_set_mouse_z(0);
	}

	//update position
	position = isVertical ? (handle->y - 13.0f) / (base->height - handle->height) : (handle->x - 13.0f) / (base->width - handle->width);
}

void HD_UI_Scrollbar::SetHandleSize(float size)
{
	std::shared_ptr<HD_UI_Container> handle = getContainerByName("handle");
	std::shared_ptr<HD_UI_Object> base = getChildByName("btnBase");

	//clamp it
	if (size <= 0.1f) size = 0.1f;

	//set it
	if (isVertical)
	{
		std::shared_ptr<HD_UI_Object> hM = handle->getChildByName("handleMiddle");
		std::shared_ptr<HD_UI_Object> hB = handle->getChildByName("handleBottom");
		hM->height = base->height * size > base->height - 16.0f ? base->height - 16.0f : base->height * size;
		hB->y = hM->y + hM->height;
		handle->height = hB->y + 8.0f;
	}
	else
	{
		std::shared_ptr<HD_UI_Object> hM = handle->getChildByName("handleMiddle");
		std::shared_ptr<HD_UI_Object> hR = handle->getChildByName("handleRight");
		hM->width = base->width * size > base->width - 16.0f ? base->width - 16.0f : base->width * size;
		hR->x = hM->x + hM->width;
		handle->width = hR->x + 8.0f;
	}
}