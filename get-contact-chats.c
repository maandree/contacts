/* See LICENSE file for copyright and license details. */
#define CATEGORY chats

#define LIST_PARAMS(X)\
	X('C', "C", 'c', "c", context, "context")\
	X('S', "S", 's', "s", service, "service")\
	X('A', "A", 'a', "a", address, "address")

#include "common.h"
IMPLEMENT_GET_ON_LIST(chat)
