#ifndef _HEAP_H_
#define _HEAP_H_ 1

#include <rt_types.h>
#include <Lockable.h>

class Instrument;

// class for heap element structure

class heapslot {
public:
  FRAMETYPE chunkStart; // start samp for chunk
  Instrument *inst;
  heapslot *left, *right, *parent;

  heapslot();
  ~heapslot();
  void dump(int);
};

// class for queue used to hold heap end

class qElt {
public:
  qElt(heapslot *hs) : next(NULL), prev(NULL), heap(hs) {}
  friend class rtqueue;
  friend class heap;
  qElt *next;
  qElt *prev;
  heapslot *heap;
};

// class for main queue structure

class rtqueue {
private:
  friend class heap;
  qElt *head;
  qElt *tail;
public:
  rtqueue() : head(NULL), tail(NULL) {}
  ~rtqueue();
  void pushTail(heapslot*);
  void push(heapslot*);
  heapslot *pop();
  heapslot *popTail();
  void print();
};

// class for main heap structure

class heap : public Lockable {
private:
  rtqueue leaves;  // queue used to hold next insertion point
public:
  heapslot* bot;
  heapslot* top;
  heap() : bot(NULL), top(NULL), size(0) {}
  ~heap();
  FRAMETYPE getTop();
  long getSize() const { return size; }
  void insert(Instrument*, FRAMETYPE chunkStart);
  Instrument *deleteMin(FRAMETYPE maxChunkStart, FRAMETYPE *pChunkStart);
  void dump(heapslot*,int);
  long size;
};

// class for queue used to hold Instruments

class rtQElt {
public:
  rtQElt(Instrument *inst, FRAMETYPE start);
  ~rtQElt();
private:
  friend class RTQueue;
  rtQElt *next;
  rtQElt *prev;
  Instrument *Inst;
  unsigned long chunkstart;
};

// class for main queue structure

class RTQueue {
private:
  rtQElt *head;
  rtQElt *tail;
  int size;
public:
  RTQueue() : head(NULL), tail(NULL), size(0) {}
  ~RTQueue();
  void push(Instrument*, FRAMETYPE);
  Instrument *pop(FRAMETYPE *pChunkStart);
  FRAMETYPE nextChunk();
  // Return the number of elements on the RTQueue
  int getSize() const { return size; }
  void print();  // For debugging
};


#endif /* _HEAP_H_ */

