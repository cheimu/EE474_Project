#include "structures.h"

void insert(TCB* node){
	if (NULL == head) {
		head = node; 
		tail = node;
	} else {
		tail -> next = node;
		node -> prev = tail; 
		tail = node;
	}
}

void deleteNode(TCB* node) {
	if (NULL == head) {
		return;
	} else if (head == tail) {
		head = NULL;
		tail = NULL;
	} else if (head == node) {
		head = head->next;
		node->next->prev = NULL;
		node->next = NULL;
	} else if (tail == node) {
		tail = tail->prev;
		node->prev->next = NULL;
		node->prev = NULL;
	} else {
		node->prev->next  = node->next;
		node->next->prev = node->prev;
		node->prev = NULL;
		node->next = NULL;
	}
}
