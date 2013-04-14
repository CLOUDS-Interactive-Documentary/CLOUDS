/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef IOS
#include <iostream>
#endif
#include "heap.h"
#include <ugens.h> // for rtcmix_warn()

using namespace std;

rtqueue::~rtqueue()
{
//	cout << "rtqueue::~rtqueue()\n";
	if (tail != head) {
	    delete tail;
	}
	delete head;
// 	while (head) {
// 		qElt *next = head->next;
// 		delete head;
// 		head = next; 
// 	}
}

void 
rtqueue::pushTail(heapslot *newHeapElt)
{
  // create new qElt
  qElt *newElt = new qElt(newHeapElt);

  if (head == NULL)  // if first item on queue
    head = tail = newElt;
  else {  // append to the end of the queue
    tail->next = newElt;
    newElt->prev = tail;
    tail = newElt;
  }
}

void 
rtqueue::push(heapslot *newHeapElt)
{
  qElt *newElt = new qElt(newHeapElt);
  
  if (head == NULL)
    head = tail = newElt;
  else {  // push to front of queue
    newElt->next = head;
    head->prev = newElt;
    head = newElt;
  }
}

heapslot *
rtqueue::pop() 
{
  qElt *tQelt;
  heapslot *retHeap;
  tQelt = head;
  if (!head) {
    rtcmix_warn("heap","attempt to pop empty rtqueue\n");
    return NULL;
  }
  retHeap = head->heap;
  head = head->next;
  delete tQelt;
  return retHeap;
}

heapslot *
rtqueue::popTail()
{
  qElt *tQelt;
  heapslot *retHeap;
  tQelt = tail;
  if (!tail) {
    rtcmix_warn("heap", "attempt to popTail empty rtqueue\n");
    return NULL;
  }
  retHeap = tail->heap;
  tail = tail->prev;
  delete tQelt;
  return retHeap;
}





