#ifndef _LINKEDLISTITEM_H_
#define _LINKEDLISTITEM_H_
/**
	individual item for the linked list
	for use within LinkedList class
*/
class LinkedListItem{

	public:

		long item; ///< holds the data being stored
		LinkedListItem* next; ///< pointer to the next one in the chain
		LinkedListItem* previous; ///< pointer to previous one in the chain

};

#endif
