//========================================
// The base class of a UI Object. GFX,
// buttons, text, containers, windows, all
// inherit from this.
// Not really used in actual layout
// creation.
//========================================

#ifndef HD_UI_OBJECT_H
#define HD_UI_OBJECT_H

class HD_UI_Layout;

class HD_UI_Object
{

protected:
	HD_UI_Layout *parent;

public:
	//constructor/destructor
	HD_UI_Object();
	~HD_UI_Object();

	//members
	float x;		//both x and y are relative to the parent
	float y;
	float width;	//even width and height?
	float height;

	char *name;	//the name of this object
	int index;	//the layer this object is on

	//functions
	void virtual Update();
	void virtual Draw();
	void virtual Clear();

	//parent functions
	HD_UI_Layout* getParent();
	void setParent(HD_UI_Layout *newParent, int index);

};

#endif