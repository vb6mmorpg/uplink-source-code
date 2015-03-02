#if !defined __RECTANGLE_H
#define __RECTANGLE_H

class MapRectangle {
public:
    
    int x1, y1, width, height;
    
public:
    
    MapRectangle(int x, int y, int width, int height)
	: x1(x), y1(y), width(width), height(height)
	{
	}
    
    int x2() const { return x1 + width - 1; }
    int y2() const { return y1 + height - 1; }
    
    int Min( int a, int b) const {
	if (a < b) return a; 
	return b;
    }
    
    int Max( int a, int b) const {
	if (a > b) return a;
	return b;
    }
    
    MapRectangle intersection(const MapRectangle &o) const
	{
	    int newX1 = Max(x1, o.x1);
	    int newY1 = Max(y1, o.y1);
	    int newX2 = Min(o.x2(), x2());
	    int newY2 = Min(o.y2(), y2());
	    
	    return MapRectangle(newX1, newY1, newX2 - newX1 + 1, newY2 - newY1 + 1);
	}
    
    bool isNull() const { return (width < 0 || height < 0); };
    
    bool intersects(const MapRectangle &o) const
	{
	    return !intersection(o).isNull();
	}
    
    bool operator == (const MapRectangle &o) const 
	{
	    return (x1 == o.x1) && (y1 == o.y1) && (width == o.width) && (height == o.height);
	}
    
    // Returns true if o is fully contained by this rectangle
    bool contains(const MapRectangle &o) const
	{
	    return intersection(o) == o;
	}
    
};

// inline ostream &operator << (ostream &o, const MapRectangle& r)
// {
//     o << "MapRectangle(x1=" << r.x1 << ", y1=" << r.y1 << ", width=" << r.width << ", height=" << r.height << ")";
//     return o;
// }



#endif
