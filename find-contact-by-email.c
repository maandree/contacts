/* See LICENSE file for copyright and license details. */
#define CATEGORY emails

#define LIST_PARAMS(X)\
	X('c', "c", context, "context")

#include "common.h"
IMPLEMENT_FIND_ON_LIST(email, address, "address")
