// Tosser implementation file
// Part of tosser
// By Christopher Delay

#ifndef _included_tosser_darray
#define _included_tosser_darray

#include <stdlib.h>
#include <iostream>
#include <assert.h>
using namespace std;

#include "tosser.h"

template <class T>
DArray <T> :: DArray ()
{

    stepsize = 1;
    arraysize = 0;
    dynarray = NULL;
    shadow = NULL;
    
}

template <class T>
DArray <T> :: DArray ( const DArray<T>& da )
{

    stepsize = da.stepsize;
    arraysize = da.arraysize;

	if ( da.dynarray && da.shadow ) {
		dynarray = new T [ arraysize ];
		shadow = new char [ arraysize ];

		for ( int a = 0; a < arraysize; ++a ) {
			dynarray [a] = da.dynarray [a];
			shadow [a] = da.shadow [a];
		}
	}
	else {
		dynarray = NULL;
		shadow = NULL;
	}
    
}

template <class T>
DArray <T> :: DArray ( int newstepsize )
{

    stepsize = newstepsize;
    arraysize = 0;
    dynarray = NULL;
    shadow = NULL;
    
}

template <class T>
DArray <T> :: ~DArray ()
{

    Empty ();

}

template <class T>
void DArray <T> :: SetSize ( int newsize )
{

	if ( newsize <= 0 ) {

		arraysize = 0;

		if ( dynarray ) delete [] dynarray;
		if ( shadow ) delete [] shadow;

		dynarray = NULL;
		shadow = NULL;

	}
	else if ( newsize > arraysize ) {

		int oldarraysize = arraysize;

		arraysize = newsize;
		T *temparray = new T [ arraysize ];
		char *tempshadow = new char [ arraysize ];
		
		int a;

		for ( a = 0; a < oldarraysize; ++a ) {
			
			temparray [a] = dynarray [a];
			tempshadow [a] = shadow [a];
			
		}
		
		for ( a = oldarraysize; a < arraysize; ++a )
			tempshadow [a] = 0;
		
		if ( dynarray ) delete [] dynarray;
		if ( shadow ) delete [] shadow;
		
		dynarray = temparray;
		shadow = tempshadow;

	}
	else if ( newsize < arraysize ) {

		arraysize = newsize;
		T *temparray = new T [arraysize];
		char *tempshadow = new char [arraysize];

		for ( int a = 0; a < arraysize; ++a ) {

			temparray [a] = dynarray [a];
			tempshadow [a] = shadow [a];

		}

		if ( dynarray ) delete [] dynarray;
		if ( shadow ) delete [] shadow;
		
		dynarray = temparray;
		shadow = tempshadow;

	}
	else if ( newsize == arraysize ) {

		// Do nothing

	}

}

template <class T>
void DArray <T> :: SetStepSize ( int newstepsize )
{

	stepsize = newstepsize;

}

template <class T>
int DArray <T> :: PutData ( const T &newdata )
{
    
    int freespace = -1;				 // Find a free space
    
    for ( int a = 0; a < arraysize; ++a ) {
	
		if ( shadow [a] == 0 ) {
			
			freespace = a;
			break;
			
		}
		
    }
    
    if ( freespace == -1 ) {			 // Must resize the array
			
		freespace = arraysize;		
		SetSize ( arraysize + stepsize );
   
    }

    dynarray [freespace] = newdata;
    shadow [freespace] = 1;
    
    return freespace;

}

template <class T>
void DArray <T> :: PutData ( const T &newdata, int index )
{

    assert ( index < arraysize && index >= 0 );       

    dynarray [index] = newdata;
    shadow [index] = 1;

}

template <class T>
void DArray <T> :: Empty ()
{

    if ( dynarray != NULL )
		delete [] dynarray;
    
    if ( shadow != NULL )
		delete [] shadow;
    
    dynarray = NULL;
    shadow = NULL;
    
    arraysize = 0;

}

template <class T>
T DArray <T> :: GetData ( int index ) const
{

    assert ( index < arraysize && index >= 0 );       

    if ( shadow [index] == 0 )
	   cout << "DArray::GetData called, referenced unused data.  (Index = " << index << ")\n";
    
    return dynarray [index];

}

template <class T>
T& DArray <T> :: operator [] (int index)
{

    assert ( index < arraysize && index >= 0 );

    if ( shadow [index] == 0 )
		cout << "DArray error : DArray::[] called, referenced unused data.  (Index = " << index << ")\n";
    
    return dynarray [index];    
}


template <class T>
void DArray <T> :: ChangeData ( const T &newdata, int index )
{
    
	assert ( index < arraysize && index >= 0 );
        
    if ( shadow [index] == 0 )
		cout << "Warning : DArray::ChangeData called, referenced unused data.  (Index = " << index << ")\n";
    
    PutData ( newdata, index );
    shadow [index] = 1;
    
}

template <class T>
void DArray <T> :: RemoveData ( int index )
{
    
    assert ( index < arraysize && index >= 0 );
    
    if ( shadow [index] == 0 )
		cout << "Warning : DArray::RemoveData called, referenced unused data.  (Index = " << index << ")\n";
    
    shadow [index] = 0;
    
}

template <class T>
int DArray <T> :: NumUsed () const
{

    int count = 0;
    
    for ( int a = 0; a < arraysize; ++a )
		if ( shadow [a] == 1 )
			++count;
		
    return count;
		
}

template <class T>
int DArray <T> :: Size () const 
{
	       
    return arraysize;
		
}

template <class T>
bool DArray <T> :: ValidIndex ( int index ) const 
{
    
    if (index >= arraysize || index < 0 )
		return false;
    
    if (!shadow [index])
		return false;
    
    return true;
    
}

template <class T>
int DArray <T> :: FindData ( const T &newdata ) const 
{
    
    for ( int a = 0; a < arraysize; ++a )
		if ( shadow [a] )
		    if ( dynarray [a] == newdata )
				return a;
    
    return -1;    
    
}

template <class T>
void DArray <T> :: Sort ( Comparator comp )
{

	// First compact the array

	int nextIndex = 0, validItem = 0;
    for ( int a = 0; a < arraysize; ++a ) {
		if ( shadow [a] ) {
			validItem++;
			if ( nextIndex == a )
				nextIndex++;
		}
		else
			for ( ; nextIndex < arraysize; nextIndex++ )
				if ( shadow [nextIndex] ) {
					validItem++;
					shadow [a] = 1;
					dynarray [a] = dynarray [nextIndex];
					shadow [nextIndex] = 0;
					nextIndex++;
					break;
				}
	}

	if ( validItem > 0 )
		qsort ( dynarray, validItem, sizeof(T), ( int (*)(const void *,const void *) )comp );

}

#endif
