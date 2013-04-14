/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* Class for accessing pfbus data info
	The class is static for pfbus data

	Brad Garton, 12/2012
*/

#define NPFBUSSES 1024

#include <PField.h>

class PFBusData {
public:
	PFBusData() {};
	~PFBusData() {};

	static int drawflag[];
	static const PField *thepfield[];
	static double val[];
	static double percent[];
	static double theincr[];
	static int dqflag[];
	static int pfbus_is_connected[];
	static int dq_now[];
	static int connect_val; // for the bus_link() thing
};
