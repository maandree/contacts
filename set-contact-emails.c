/* See LICENSE file for copyright and license details. */
#define CATEGORY emails

#define LIST_PARAMS(X)\
	X('A', "A", 'a', "a", address, "address")\
	X('C', "C", 'c', "c", context, "context")

#include "common.h"
IMPLEMENT_SET_ON_LIST(email)
