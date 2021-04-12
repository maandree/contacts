/* See LICENSE file for copyright and license details. */
#define CATEGORY pgpkeys

#define LIST_PARAMS(X)\
	X('C', "C", 'c', "c", context, "context")\
	X('F', "F", 'f', "f", id, "fingerprint")

#include "common.h"
IMPLEMENT_GET_ON_LIST(pgpkey)
