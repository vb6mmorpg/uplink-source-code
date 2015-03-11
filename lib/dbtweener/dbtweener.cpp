#include "dbtweener.h"

// declare the static equation classes
CDBTweener::CLinear CDBTweener::TWEQ_LINEAR;
CDBTweener::CQuadratic CDBTweener::TWEQ_QUADRATIC;
CDBTweener::CCubic CDBTweener::TWEQ_CUBIC;
CDBTweener::CQuartic CDBTweener::TWEQ_QUARTIC;
CDBTweener::CQuintic CDBTweener::TWEQ_QUINTIC;
CDBTweener::CSinusoidal CDBTweener::TWEQ_SINUSOIDAL;
CDBTweener::CExponential CDBTweener::TWEQ_EXPONENTIAL;
CDBTweener::CCircular CDBTweener::TWEQ_CIRCULAR;
CDBTweener::CBounce CDBTweener::TWEQ_BOUNCE;
CDBTweener::CElastic CDBTweener::TWEQ_ELASTIC;
CDBTweener::CBack CDBTweener::TWEQ_BACK;

#ifdef DB_USE_BUILTIN_LOOKUP
// create tables for fast sin/cos lookup
static struct __CDBMathLookup
{
public:
	__CDBMathLookup()
	{
		for (int i=0;i<1000;i++)
			m_afsin[i] = (float)::sin(((M_PI * 2.0f) / 1000.0f) * (float)i);
	}

	float sin(float fRad)
	{
		float fIn = mod(fRad, M_PI * 2.0f);
		int iIndex = (int)(fIn * (1000.0f / (M_PI * 2.0f)));
		return m_afsin[iIndex];
	}

	float cos(float fRad) { return sin((M_PI/2.0f)-fRad); }

private:
	float mod(float x, float y)	{ return x - y * floor(x/y); }
	float m_afsin[1000];
} __g_oDBMathLookup;
#endif

CDBTweener::CTween::CTween()
{
	m_fElapsedSec = 0.0f;
	m_fDurationSec = 0.0f;
	m_pEquation = &TWEQ_LINEAR;
	m_eEasing = TWEA_IN;
	m_pUserData = NULL;
}

CDBTweener::CTween::CTween(CEquation *pEquation, EEasing eEasing, float fDuration, float *fpValue, float fTarget)
{
	m_fElapsedSec = 0.0f;
	m_fDurationSec = fDuration;
	m_pEquation = pEquation;
	m_eEasing = eEasing;
	m_pUserData = NULL;
	addValue(fpValue, fTarget);
}

CDBTweener::CTween::~CTween()
{
	for (TValuePts::iterator i = m_vValues.begin(); i != m_vValues.end(); i++)
	{
		SValue *pValue = *i;
		delete pValue;
	}
}

void CDBTweener::CTween::setEquation(CEquation *pEquation, EEasing eEasing, float fDuration)
{
	m_pEquation = pEquation;
	m_eEasing = eEasing;
	m_fDurationSec = fDuration;
}

void CDBTweener::CTween::addValue(float *fpValue, float fTarget)
{
	m_vValues.push_back(new SValue(fpValue, fTarget));
}

void CDBTweener::CTween::step(float fDeltaTimeSec)
{
	// increase elapsed time
	float fBeforeStep = m_fElapsedSec;
	m_fElapsedSec += fDeltaTimeSec;

	// tween all values
	for (TValuePts::iterator i = m_vValues.begin(); i != m_vValues.end(); i++)
	{
		SValue *pValue = *i;
			
		// initialize the start position if no time has elapsed yet.
		if (fBeforeStep <= FLT_EPSILON) pValue->m_fStart = *pValue->m_fpValue;

		float *fpValue = pValue->m_fpValue;
		float fStart = pValue->m_fStart;
		float fChange = pValue->m_fTarget - fStart;
		switch (m_eEasing)
		{
		case TWEA_IN:
			*fpValue = m_pEquation->easeIn(m_fElapsedSec, fStart, fChange, m_fDurationSec);
			break;
		case TWEA_OUT:
			*fpValue = m_pEquation->easeOut(m_fElapsedSec, fStart, fChange, m_fDurationSec);
			break;
		case TWEA_INOUT:
			*fpValue = m_pEquation->easeInOut(m_fElapsedSec, fStart, fChange, m_fDurationSec);
			break;
		}

		if (m_fElapsedSec >= m_fDurationSec)
			*fpValue = pValue->m_fTarget;		// don't overshoot
	}

	// if we're done, notify all listeners of the fact
	if (m_fElapsedSec >= m_fDurationSec)
	{
		for (TListenerPts::iterator j = m_sListeners.begin(); j != m_sListeners.end(); j++)
		{
			IListener *pListener = *j;
			pListener->onTweenFinished(this);
		}
	}
}

CDBTweener::CDBTweener()
{

}

CDBTweener::~CDBTweener()
{
	for (TTweenPts::iterator i = m_sTweens.begin(); i != m_sTweens.end(); i++)
	{
		CTween *pTween = *i;
		delete pTween;
	}
}

void CDBTweener::addTween(CTween *pTween)
{
	pTween->addListener(&m_oTweenRelay);
	m_sTweens.insert(pTween);
}

void CDBTweener::addTween(CEquation *pEquation, EEasing eEasing, float fDuration, float *fpValue, float fTarget)
{
	CTween *pTween = new CTween(pEquation, eEasing, fDuration, fpValue, fTarget);
	pTween->addListener(&m_oTweenRelay);
	m_sTweens.insert(pTween);
}

void CDBTweener::removeTween(CTween *pTween)
{
	TTweenPts::iterator iterT = m_sTweens.find(pTween);
	if (iterT != m_sTweens.end())
	{
		CTween *pTween = *iterT;
		delete pTween;
		m_sTweens.erase(iterT);
	}
}

void CDBTweener::step(float fDeltaTimeSec)
{
	for (TTweenPts::iterator i = m_sTweens.begin(); i != m_sTweens.end();)
	{
		CTween *pTween = *i;

		pTween->step(fDeltaTimeSec);
		
		if (pTween->isFinished())
		{
			m_sTweens.erase(i++);
			delete pTween;
		} else ++i;
	}
}

float CDBTweener::CLinear::easeIn(float t, float b, float c, float d)
{
	return c*t/d + b;
}

float CDBTweener::CLinear::easeOut(float t, float b, float c, float d)
{
	return easeIn(t, b, c, d);
}

float CDBTweener::CLinear::easeInOut(float t, float b, float c, float d)
{
	return easeIn(t, b, c, d);
}

float CDBTweener::CQuadratic::easeIn(float t, float b, float c, float d)
{
	t /= d;
	return c*t*t + b;		
}

float CDBTweener::CQuadratic::easeOut(float t, float b, float c, float d)
{
	t /= d;
	return -c * t*(t-2.0f) + b;
}

float CDBTweener::CQuadratic::easeInOut(float t, float b, float c, float d)
{
	t /= d/2.0f;
	if (t < 1.0f) return c/2.0f*t*t + b;
	t--;
	return -c/2.0f * (t*(t-2.0f) - 1.0f) + b;	
}

float CDBTweener::CCubic::easeIn(float t, float b, float c, float d)
{
	t /= d;
	return c*t*t*t + b;
}

float CDBTweener::CCubic::easeOut(float t, float b, float c, float d)
{
	t /= d;
	t--;
	return c*(t*t*t + 1.0f) + b;
}

float CDBTweener::CCubic::easeInOut(float t, float b, float c, float d)
{
	t /= d/2.0f;
	if (t < 1.0f) return c/2.0f*t*t*t + b;
	t -= 2.0f;
	return c/2.0f*(t*t*t + 2.0f) + b;	
}

float CDBTweener::CQuartic::easeIn(float t, float b, float c, float d)
{
	t /= d;
	return c*t*t*t*t + b;
}

float CDBTweener::CQuartic::easeOut(float t, float b, float c, float d)
{
	t /= d;
	t--;
	return -c * (t*t*t*t - 1.0f) + b;
}

float CDBTweener::CQuartic::easeInOut(float t, float b, float c, float d)
{
	t /= d/2.0f;
	if (t < 1.0f) return c/2.0f*t*t*t*t + b;
	t -= 2.0f;
	return -c/2.0f * (t*t*t*t - 2.0f) + b;
}

float CDBTweener::CQuintic::easeIn(float t, float b, float c, float d)
{
	t /= d;
	return c*t*t*t*t*t + b;
}

float CDBTweener::CQuintic::easeOut(float t, float b, float c, float d)
{
	t /= d;
	t--;
	return c*(t*t*t*t*t + 1.0f) + b;
}

float CDBTweener::CQuintic::easeInOut(float t, float b, float c, float d)
{
	t /= d/2.0f;
	if (t < 1.0f) return c/2.0f*t*t*t*t*t + b;
	t -= 2.0f;
	return c/2.0f*(t*t*t*t*t + 2.0f) + b;	
}

float CDBTweener::CSinusoidal::easeIn(float t, float b, float c, float d)
{
	return -c * DB_COS_FUNC(t/d * (M_PI/2.0f)) + c + b;
}

float CDBTweener::CSinusoidal::easeOut(float t, float b, float c, float d)
{
	return c * DB_SIN_FUNC(t/d * (M_PI/2.0f)) + b;
}

float CDBTweener::CSinusoidal::easeInOut(float t, float b, float c, float d)
{
	return -c/2.0f * (DB_COS_FUNC(M_PI*t/d) - 1.0f) + b;
}

float CDBTweener::CExponential::easeIn(float t, float b, float c, float d)
{
	return c * DB_POW_FUNC( 2.0f, 10.0f * (t/d - 1.0f) ) + b;
}

float CDBTweener::CExponential::easeOut(float t, float b, float c, float d)
{
	return c * ( -DB_POW_FUNC( 2.0f, -10.0f * t/d ) + 1.0f ) + b;
}

float CDBTweener::CExponential::easeInOut(float t, float b, float c, float d)
{
	t /= d/2.0f;
	if (t < 1.0f) return c/2.0f * DB_POW_FUNC( 2.0f, 10.0f * (t - 1.0f) ) + b;
	t--;
	return c/2.0f * ( -DB_POW_FUNC( 2.0f, -10.0f * t) + 2.0f ) + b;
}

float CDBTweener::CCircular::easeIn(float t, float b, float c, float d)
{
	t /= d;
	return -c * (DB_SQRT_FUNC(1.0f - t*t) - 1.0f) + b;
}

float CDBTweener::CCircular::easeOut(float t, float b, float c, float d)
{
	t /= d;
	t--;
	return c * DB_SQRT_FUNC(1.0f - t*t) + b;
}

float CDBTweener::CCircular::easeInOut(float t, float b, float c, float d)
{
	t /= d/2.0f;
	if (t < 1) return -c/2.0f * (DB_SQRT_FUNC(1.0f - t*t) - 1.0f) + b;
	t -= 2.0f;
	return c/2.0f * (DB_SQRT_FUNC(1.0f - t*t) + 1.0f) + b;
}

float CDBTweener::CBounce::easeIn(float t, float b, float c, float d)
{
	return c - easeOut(d-t, 0.0f, c, d) + b;
}

float CDBTweener::CBounce::easeOut(float t, float b, float c, float d)
{
	if ((t/=d) < (1.0f/2.75f)) {
		return c*(7.5625f*t*t) + b;
	} else if (t < (2.0f/2.75f)) {
		return c*(7.5625f*(t-=(1.5f/2.75f))*t + .75f) + b;
	} else if (t < (2.5f/2.75f)) {
		return c*(7.5625f*(t-=(2.25f/2.75f))*t + .9375f) + b;
	} else {
		return c*(7.5625f*(t-=(2.625f/2.75f))*t + .984375f) + b;
	}	
}

float CDBTweener::CBounce::easeInOut(float t, float b, float c, float d)
{
	if (t < d/2.0f) return easeIn (t*2.0f, 0.0f, c, d) * .5f + b;
	else return easeOut (t*2.0f-d, 0.0f, c, d) * .5f + c*.5f + b;
}

float CDBTweener::CElastic::easeIn(float t, float b, float c, float d)
{
	if (DB_FLT_EQ(t, 0.0f)) return b;
	if (DB_FLT_EQ((t/=d), 1.0f)) return b+c;  
	
	float p = d*.3f;
	float a = c;
	float s = p/4.0f;

	return -(a*DB_POW_FUNC(2.0f,10.0f*(t-=1.0f)) * DB_SIN_FUNC( (t*d-s)*(2.0f*M_PI)/p )) + b;
}

float CDBTweener::CElastic::easeOut(float t, float b, float c, float d)
{
	if (DB_FLT_EQ(t, 0.0f)) return b;
	if (DB_FLT_EQ((t/=d), 1.0f)) return b+c;

	float p = d*.3f;
	float a = c;
	float s = p/4.0f;

	return (a*DB_POW_FUNC(2.0f,-10.0f*t) * DB_SIN_FUNC( (t*d-s)*(2.0f*M_PI)/p ) + c + b);
}

float CDBTweener::CElastic::easeInOut(float t, float b, float c, float d)
{
	if (DB_FLT_EQ(t, 0.0f)) return b;
	if (DB_FLT_EQ((t/=d/2.0f), 2.0f)) return b+c;
	
	float p = d*(.3f*1.5f);
	float a = c;
	float s = p/4.0f;

	if (t < 1.0f) return -.5f*(a*DB_POW_FUNC(2.0f,10.0f*(t-=1.0f)) * DB_SIN_FUNC( (t*d-s)*(2.0f*M_PI)/p )) + b;
	return a*DB_POW_FUNC(2.0f,-10.0f*(t-=1.0f)) * DB_SIN_FUNC( (t*d-s)*(2.0f*M_PI)/p )*.5f + c + b;
}

float CDBTweener::CBack::easeIn(float t, float b, float c, float d)
{
	s = 1.70158f;
	return c*(t/=d)*t*((s+1.0f)*t - s) + b;
}

float CDBTweener::CBack::easeOut(float t, float b, float c, float d)
{
	s = 1.70158f;
	return c*((t=t/d-1.0f)*t*((s+1.0f)*t + s) + 1.0f) + b;
}

float CDBTweener::CBack::easeInOut(float t, float b, float c, float d)
{
	s = 1.70158f;
	if ((t/=d/2.0f) < 1.0f) return c/2.0f*(t*t*(((s*=(1.525f))+1.0f)*t - s)) + b;
	return c/2.0f*((t-=2.0f)*t*(((s*=(1.525f))+1.0f)*t + s) + 2.0f) + b;
}
