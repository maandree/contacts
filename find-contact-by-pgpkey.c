/* See LICENSE file for copyright and license details. */
#define CATEGORY pgpkeys

#define LIST_PARAMS(X)\
	X('c', "c", context, "context")

#include "common.h"
IMPLEMENT_FIND_ON_LIST(pgpkey, id, "fingerprint")
