/* See LICENSE file for copyright and license details. */
#define CATEGORY chats
#define SUBCATEGORY service

#define LIST_PARAMS(X)\
	X('c', "c", context, "context")\
	X('s', "s", service, "service")

#include "common.h"
IMPLEMENT_FIND_ON_LIST(chat, address, "address")
