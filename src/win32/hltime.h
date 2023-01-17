#ifndef hltime_h
#define hltime_h

#include "GameEngineConfig.h"

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef WXWIDGETS
#include "wx/msw/winundef.h"
#endif

struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};

int gettimeofday(struct timeval *, struct timezone *);
#endif