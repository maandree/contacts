/* See LICENSE file for copyright and license details. */
#define CATEGORY chats

#define LIST_PARAMS(X)\
	X('A', "A", 'a', "a", address, "address")\
	X('S', "S", 's', "s", service, "service")\
	X('C', "C", 'c', "c", context, "context")

#include "common.h"
IMPLEMENT_SET_ON_LIST(chat)
