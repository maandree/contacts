/* See LICENSE file for copyright and license details. */
#define CATEGORY numbers

#define LIST_PARAMS(X)\
	X('C', "C", 'c', "c", context, "context")\
	X('N', "N", 'n', "n", number, "number")

#define LIST_BOOL_PARAMS(X)\
	X('F', "F", 'f', "f", facsimile, "facsimile")\
	X('M', "M", 'm', "m", mobile, "mobile")

#include "common.h"
IMPLEMENT_SET_ON_LIST(number)
