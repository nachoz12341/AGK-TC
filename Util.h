#ifndef _H_UTIL
#define _H_UTIL

static float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

static int Sign(float val)
{
	return (float(0) < val) - (val < float(0));
}

#endif
