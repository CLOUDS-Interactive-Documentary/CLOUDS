/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/*
 * addrtInst()
 *	Function: Introduce a new rt creation func/object to CMIX 
 *	Args:	rt_item structure
 *	Calls:	nothing
 *	Return:	0 on success, -1 on failure
 *  -------> BGG: add these and "mixerr.h" later
 *		MX_FEXIST - ugen by that name already exists
 *		MX_EMEM	  - No memory to create ug_item
 *	Side Effects:
 */
#include <RTcmix.h>
#include "rt.h"
#include "Instrument.h"
#include "mixerr.h"
#include <string.h>

int
addrtInst(rt_item *rt_p)
{
	return RTcmix::addrtInst(rt_p);
}

int
RTcmix::addrtInst(rt_item *rt_p)
{
	rt_item *rt_tail;

	// printf("ENTERING addrtInst() FUNCTION -----\n");
	/*  Seek end of rt_list	*/
	rt_tail = rt_list;
	
	if (!rt_tail) { // first one on the list
	  rt_list = rt_p;
	  rt_p->rt_next = NULL;
	  // printf("EXITING addrtInst() FUNCTION (0)-----\n");
	  return 0;
	}
	for (rt_tail=rt_list; rt_tail->rt_next; rt_tail=rt_tail->rt_next)
	{
		if (!strcmp((char *)rt_tail,rt_p->rt_name))	{
			mixerr = MX_FEXIST;
			// printf("EXITING addrtInst() FUNCTION (MX_FEXIST)-----\n");
			return (-1);
		}
	}
	rt_tail->rt_next = rt_p;
	rt_p->rt_next = NULL;
	// printf("EXITING addrtInst() FUNCTION (0)-----\n");
	return (0);
}





