// precompiled header

#ifndef PRECOMPILED_H
#define PRECOMPILED_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <windows.h>
#include <direct.h>
#include <tchar.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#define RELEASE
#endif

#include <assert.h>

#include <ctime> 
#include <cstdlib>
#include <cmath>

#include <GL/glut.h>
#include <GL/glui.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <time.h>
#include <utility>

// this isn't precompiled but I don't feel like including it everywhere XD
#include "MathUtil.h"

#endif//PRECOMPILED_H
