/**
@author jtnimoy
*/

#include "LinkedList.h"

LinkedList::LinkedList(){
  head = 0;
  tail = 0;
  count = 0;
}

//-----------------------------------------------------------------

LinkedList::~LinkedList(){
  LinkedListItem* current;
  current = head;
  if (current==0);
  int foundOne = 1;
  while(foundOne){
    foundOne = 0;
    for(int i=0;i<this->count;i+=2){
      LinkedListItem *c = current;
      current = current->next;
      delete c;
    }
  }
}

//-----------------------------------------------------------------

int LinkedList::size(){
	return count;
}

//-----------------------------------------------------------------

void LinkedList::set(long index, long value){
  LinkedListItem* current;
  current = head;
  
  if (current==0)return;
  
  for(int i=0;i<index;i++){
      current = current->next;
  }
  current->item = value;
}

//-----------------------------------------------------------------

long LinkedList::get(long index){
  LinkedListItem* current;
  current = head;
  if (current==0)return 0;
  for(int i=0;i<index;i++){
      current = current->next;
  }
  return current->item;
}

//-----------------------------------------------------------------
int LinkedList::push( long data ){
  LinkedListItem* new_item;
  
  new_item = new LinkedListItem();
  if ( new_item == 0 ) return false;
  new_item->next = 0;
  new_item->previous = 0;
  
  new_item->item = data;
  
  if ( head == 0 || tail == 0){
    tail = new_item;
    head = new_item;
    new_item -> next = 0;
    new_item -> previous = 0;
  } else {
    tail -> next = new_item;
    new_item -> previous = tail;
    new_item -> next = 0;
    tail = new_item;
  }
  count++;
  return true;
}

//-----------------------------------------------------------------

long LinkedList::pop(){
  if(count>0){
    LinkedListItem* n=tail;
    tail=tail->previous;
    count--;
    long theItem = n->item;
    delete n;
    return theItem;
  }else return 0;
}

//-----------------------------------------------------------------

long LinkedList::unshift(){
  LinkedListItem* n=head;
  head=head->next;
  count--;
  long theItem = n->item;
  delete n;
  return theItem;
}

//-----------------------------------------------------------------

int LinkedList::shift( long data ){
  LinkedListItem* new_item;

  new_item = new LinkedListItem;
  if ( new_item == 0 ) return false;
  new_item->item = data;
  if ( head == 0 || tail==0 )
    {
      tail = new_item;
      head = new_item;
      new_item -> next = 0;
      new_item -> previous = 0;
    }
  else
    {
      head -> previous = new_item;
      new_item -> next = head;
      new_item -> previous = 0;
      head = new_item;
    }
  count++;
  return true;
}

//------------------------------------------------------------------


void LinkedList::deleteMatch(long match){
  LinkedListItem* current;
  if(head==0||tail==0)return;
  int foundOne = 1;
  while(foundOne){
    current = head;
    foundOne = 0;
    for(int i=0;i<this->count;i++){
      if(current->item==match){
	if(current==head){
	  if(count>1){
	    current->next->previous = 0;
	  }
	  head = current->next;
	  delete current;
	  foundOne = 1;
	  this->count--;
	  break;
	}else if(current==tail){
	  current->previous->next = 0;
	  tail = current->previous;
	  delete current;
	  foundOne = 1;
	  this->count--;
	  break;	
	}else{//for everyone else in between
	  current->previous->next = current->next;
	  current->next->previous = current->previous;
	  delete current;
	  foundOne = 1;
	  this->count--;
	  break;	
	}
      }
      //advance if needed
      current = current->next;
    }
    
  }
}



