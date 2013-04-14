#ifndef _SPRAY_H
#define _SPRAY_H 1

#define NUM_SPRAY_ARRAYS 32
#define MAX_SPRAY_SIZE   512

struct slist {
	int size;
	int current;
	int array[MAX_SPRAY_SIZE];
};

#endif /* _SPRAY_H */

