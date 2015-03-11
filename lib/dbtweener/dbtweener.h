#define DBTWEENER_VERSION "1.2"

/*****************************************************************************
  
  [DBTweener]
  ----------------

  Sirp Potijk
  http://www.deadbugprojects.com

  Inspired by Tweener and CPPTweener.
  http://code.google.com/p/tweener
  http://code.google.com/p/cpptweener

  Built-in tweener equations from Robert Penner
  Spotted at gizma.com
  http://gizma.com/easing/
  http://flashblog.robertpenner.com/

*****************************************************************************/
#ifndef DBTWEENER_H
#define DBTWEENER_H

#include <set>
#include <vector>
#include <float.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

// by default this uses a lookup table defined in dbtweener.cpp
// to use a different lookup table or sin/cos functions comment
// out the 'USE_BUILTIN_LOOKUP' macro and substitute the 'SIN_FUNC'
// and 'COS_FUNC' macro's with the correct function with signature:
// float sin(float radians)
#define DB_USE_BUILTIN_LOOKUP 1
#define DB_SIN_FUNC(r) __g_oDBMathLookup.sin(r)
#define DB_COS_FUNC(r) __g_oDBMathLookup.cos(r)

// pow and sqrt functions; performance may depend on CPU architecture
// if a faster alternative is required these macro's can be substituted.
// by default this uses the standard math.h functions.
#define DB_POW_FUNC(x, y) powf(x, y)
#define DB_SQRT_FUNC(x) sqrtf(x)

// test if two float values are the same
#define DB_FLT_EQ(a, b) (fabs(a - b) <= FLT_EPSILON)

// use this class to maintain a collection of tweens and step through all of them 
// at once. alternatively, CDBTweener::CTween can also be used 'stand-alone'.
class CDBTweener
{
public:
	enum EEasing
	{
		TWEA_IN, TWEA_OUT, TWEA_INOUT
	};

	// base class for creating tweening equations. ease functions should 
	// return the current position given the arguments:
	// t: current time
	// b: begin position
	// c: change position (end position - begin position)
	// d: duration
	class CEquation
	{
	public:
		virtual ~CEquation() {}
		virtual float easeIn(float t, float b, float c, float d) = 0;
		virtual float easeOut(float t, float b, float c, float d) = 0;
		virtual float easeInOut(float t, float b, float c, float d) = 0;
	};

	// default tweening implementations; take from Robert Penner (http://flashblog.robertpenner.com/)
	static class CLinear: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_LINEAR;

	static class CQuadratic: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_QUADRATIC;

	static class CCubic: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_CUBIC;

	static class CQuartic: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_QUARTIC;

	static class CQuintic: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_QUINTIC;

	static class CSinusoidal: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_SINUSOIDAL;

	static class CExponential: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_EXPONENTIAL;

	static class CCircular: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_CIRCULAR;

	static class CBounce: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_BOUNCE;

	static class CBack: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
		float s;
	} TWEQ_BACK;

	static class CElastic: public CEquation
	{
	public:
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	} TWEQ_ELASTIC;

	// representation of a single value 'to be tweened'. this is always a float pointer.
	// these values are used in CTween.
	struct SValue
	{
		SValue(float *fpValue, float fTarget)
		{
			m_fpValue = fpValue;
			m_fTarget = fTarget;

			// the start value is only initialized at the very last moment to make sure
			// we're not working on an old value if the tweens where prepared before
			// starting.
			m_fStart = 0.0f;
		}
		float *m_fpValue;
		float m_fTarget;
		float m_fStart;
	};

	typedef std::vector<SValue *> TValuePts;

	class IListener;

	// this is where the magic happens. this class can be used 'stand-alone' to tween a set of 
	// parameters using a single tweening equation or it can be used in an instance of CDBTweener 
	class CTween
	{
	public:
		CTween();
		CTween(CEquation *pEquation, EEasing eEasing, float fDuration, float *fpValue, float fTarget);
		~CTween();
		
		void setEquation(CEquation *pEquation, EEasing eEasing, float fDuration);
		CEquation *getEquation() const { return m_pEquation; }
		EEasing getEasing() const { return m_eEasing; }
		float getDurationSec() const { return m_fDurationSec; }

		bool isFinished() const { return m_fElapsedSec >= m_fDurationSec; }

		void addValue(float *fpValue, float fTarget);
		TValuePts &getValues() { return m_vValues; }
		void setElapsedSec(float fValue) { m_fElapsedSec = fValue; }
		float getElapsedSec() const { return m_fElapsedSec; }
		void setUserData(void *pData) { m_pUserData = pData; }	// Custom pointer; free to use
		void *getUserData() { return m_pUserData; }

		void addListener(IListener *pListener) { m_sListeners.insert(pListener); }
		void removeListener(IListener *pListener) { m_sListeners.erase(pListener); }

		void step(float fDeltaTimeSec);
	private:
		typedef std::set<IListener *> TListenerPts;

		TListenerPts m_sListeners;
		TValuePts m_vValues;
		CEquation *m_pEquation;
		EEasing m_eEasing;
		float m_fElapsedSec;
		float m_fDurationSec;
		void *m_pUserData;
	};

	typedef std::set<CTween *> TTweenPts;

	// the listener class is used in CDBTweener and CTween to listen for finished tweens.
	class IListener
	{
	public:
		virtual ~IListener() {}
		virtual void onTweenFinished(CTween *pTween) = 0;
	};

	CDBTweener();
	~CDBTweener();

	void addTween(CTween *pTween);		// transferring ownership; tween will be deleted by CDBTweener
	void addTween(CEquation *pEquation, EEasing eEasing, float fDuration, float *fpValue, float fTarget);
	void removeTween(CTween *pTween);

	const TTweenPts &getTweens() const { return m_sTweens; }

	void addListener(IListener *pListener) { m_oTweenRelay.m_sListeners.insert(pListener); }
	void removeListener(IListener *pListener) { m_oTweenRelay.m_sListeners.erase(pListener); } 

	void step(float fDeltaTimeSec);
private:
	typedef std::set<IListener *> TListenerPts;

	class CRelay: public IListener
	{
	public:
		void onTweenFinished(CTween *pTween)
		{
			for (TListenerPts::iterator i = m_sListeners.begin(); i != m_sListeners.end(); i++)
			{
				IListener *pListener = *i;
				pListener->onTweenFinished(pTween);
			}
		}
		TListenerPts m_sListeners;
	} m_oTweenRelay;	
	
	TTweenPts m_sTweens;
};

#endif // DBTWEENER_H