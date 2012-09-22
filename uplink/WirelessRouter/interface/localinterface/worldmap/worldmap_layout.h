// ============================================================================

#define WORLDMAPOBJECT_NONE			0
#define WORLDMAPOBJECT_LABEL		1
#define WORLDMAPOBJECT_LOCATION		2
#define WORLDMAPOBJECT_GATEWAY		3

#include "rectangle.h"
#include "tosser.h"

class WorldMapInterfaceObject
{

public:

    int TYPE;
    int x;
    int y;

    int baseX;                  // The point I am attached to
    int baseY;

    bool tempForConnection;     // To display missing servers in a connection

public: 

    WorldMapInterfaceObject ();
    virtual ~WorldMapInterfaceObject ();

    void SetTYPE ( int newTYPE );
    void SetPosition ( int newx, int newy );
    void SetBasePosition ( int newx, int newy );
    void SetIP ( const char *newip );
	void CheckIP ();

    virtual void Draw ( int xOffset = 0, int yOffset = 0, float zoom = 1.0 );
    virtual MapRectangle GetExtent() const;
    const char *GetIP() const;

protected:

    char *ip;
    bool isMission;             // If this object is related to a mission
	bool isColored;             // If this object is colored

};

class WorldMapInterfaceLabel : public WorldMapInterfaceObject {
public:
    
    WorldMapInterfaceLabel(const MapRectangle &mapRect, 
			   WorldMapInterfaceObject *newFeaturePoint,
			   const char *newCaption);
    ~WorldMapInterfaceLabel();
    
    void SetCaption ( const char *newcaption );
    void SetLabelPosition(int n);
    void SetRandomLabelPosition();

    int GetLabelPosition() const;
    virtual MapRectangle GetExtent() const;
    bool Overlaps(WorldMapInterfaceObject *label) const;
    
    virtual void Draw( int xOffset = 0, int yOffset = 0, float zoom = 1.0  );
    const char *GetCaption() const;
    
protected:
    
    void CalculateWidth();
    void CalculatePossibleLabelPositions(const MapRectangle &mapRect);

protected:

    char *caption;
    WorldMapInterfaceObject *featurePoint; // Does not own featurePoint

    int labelPos;
    int labelWidth;

    int numPossLabelPos;
    int possLabelPos[8];
};

class WorldMapObjectiveFunction {
public:
    
    WorldMapObjectiveFunction(const MapRectangle &mapRect);
    ~WorldMapObjectiveFunction();
    
    int GetCost() const;
    
    void AddObject( const WorldMapInterfaceObject *m );
    void SubObject( const WorldMapInterfaceObject *m );
    
    void AddRect( const MapRectangle& r );
    void SubRect( const MapRectangle& r );
    
    void Reset();
    
protected:
    
    int cost;
    int mapWidth, mapHeight;
    int *shadeScreen;
    MapRectangle clipRect;
    
};

class WorldMapLayout {
public:
    
    WorldMapLayout(const MapRectangle &newMapRectangle);
    ~WorldMapLayout();
    
    void Reset ();
    void ResetTemp ();
    void AddLocation ( int x, int y, const char *name, const char *ip = NULL, bool tempForConnection = false );
    void DeleteLocationsTemp ();
    
    void PartialLayoutLabels();
    void FullLayoutLabels();
    
    bool IsLayoutComplete() const;
    
    LList <WorldMapInterfaceObject *> & GetLocations();
    LList <WorldMapInterfaceLabel *> & GetLabels();

    int GetCountLocationTemp();

	void CheckIPs();
    
protected:
    
    void StartLayout();
    void DeleteLocations();
    void ResetLayoutParameters();
    
protected:
    
    int iteration;
    int moveNumber;
    int numGoodMoves;
    float T;                                                // temperature
    float E;                                                // current value of the objective function
    
    bool layoutComplete;
    bool layoutStarted;
    
    MapRectangle mapRectangle;
    LList <WorldMapInterfaceObject *> locations;
    LList <WorldMapInterfaceLabel *> labels;
    WorldMapObjectiveFunction objective;
    
};

