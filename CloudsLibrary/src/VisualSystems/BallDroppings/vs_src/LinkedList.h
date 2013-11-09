#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_



/**
	*LinkedList* somewhat more convenient
	than STL deque in replacing and deleting items.
	This is also more stripped down and does not
	use templates, making it more portable.
	@author jtnimoy
*/

#include "LinkedListItem.h"

class LinkedList{

private:

	LinkedListItem* head;  ///< Ptr to head of list
	LinkedListItem* tail;  ///< Ptr to tail of list
	long count; ///< internal item count

public:
	
	LinkedList();
	~LinkedList();
	
	long get(long index); ///< get an item at a certain index
	void set(long index, long value);  ///< replace the contents of an item at a certain index
	
	int push( long );   ///< Add item to tail of list
	long pop();   ///< get item from tail and delete it
	long unshift();   ///< get item from head and delete it
	int shift( long );   ///< Add item to head of list
	
	void deleteMatch(long match); ///< delete any matches found
	int size(); ///< get the number of items
};


#endif
