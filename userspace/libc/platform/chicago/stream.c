// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 17:56 BRT
// Last edited on January 05 of 2020, at 18:01 BRT

#include <chicago/list.h>

static PList __file_list = &((List){ 0 });

int __add_stream(void *stream) {
	return ListAdd(__file_list, stream);					// libchicago already have a List implementation!
}

void __remove_stream(void *stream) {
	UIntPtr idx = 0;										// We need to find it in the list... here we go!
	Boolean found = False;
	
	ListForeach(__file_list, i) {
		if (i->data == stream) {
			found = True;									// Ok, we found it!
			break;
		}
		
		idx++;
	}
	
	if (found) {
		ListRemove(__file_list, idx);
	}
}

void *__get_stream_list(void) {
	return __file_list->head;								// Return the head of the list
}

void *__get_stream(void *cur) {
	return ((PListNode)cur)->data;							// Return the item data
}

void *__get_next_stream(void *cur) {
	return ((PListNode)cur)->next;							// Return the next item (or NULL, in case we are at the end)
}
