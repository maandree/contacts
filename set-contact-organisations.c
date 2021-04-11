/* See LICENSE file for copyright and license details. */
#define CATEGORY organisations

#define LIST_PARAMS(X)\
	X('O', "O", 'o', "o", organisation, "organisation")\
	X('T', "T", 't', "t", title, "title")

#include "common.h"
IMPLEMENT_SET_ON_LIST(organisation)
