/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
 */
#ifndef _INSTRUMENT_H_
#define _INSTRUMENT_H_ 1

#include <RefCounted.h>
#include <bus.h>
#include <Locked.h>
#include <rt_types.h>
#include <sys/types.h>
#include "rtdefs.h"


#define MAXNUMPARAMS 100

class heap;
class PFieldSet;
class PField;
class BusSlot;

struct InputState {
   InputState();
   int            fdIndex;         // index into unix input file desc. table
   off_t          fileOffset;      // current offset in file for this inst
   double         inputsr;		   // SR of input file
   int            inputchans;	   // Chans of input file
#ifdef MAXMSP
	mm_buf         *my_mm_buf;    // local copy of the active [buffer~]
#endif
};

class Instrument : public RefCounted {
protected:

	// These replace the old globals
   static int	  RTBUFSAMPS;
   static int     NCHANS;
   static float   SR;

   float          _start;
   float          _dur;
   int            cursamp;
   int            chunksamps;
   FRAMETYPE      i_chunkstart;   // we need this for rtperf
   Locked<FRAMETYPE>    endsamp;
   int            output_offset;

   int            sfile_on;        // a soundfile is open (for closing later)

   InputState     _input;		   // collected state for input source

   int            outputchans;

   int            mytag;           // for note tagging/rtupdate() 

   BUFTYPE        *outbuf;         // private interleaved buffer

   BusSlot        *_busSlot;
   PFieldSet	  *_pfields;
   static double  s_dArray[];

private:
   char 		  *_name;	// the name of this instrument
   BUFTYPE        *obufptr;
   bool           bufferWritten[MAXBUS];
   bool           needs_to_run;
   int            _skip;
   int            _nsamps;
	// CHAINED INSTRUMENT SUPPORT
	BUFTYPE *		inputChainBuf;			// buffer used as input by rtgetin()
	// BGG -- for pfbus connection (dynamic PFields)
	int				my_pfbus;

public:
	// Instruments should use these to access variables.
	int				currentFrame() const { return cursamp; }
	int				framesToRun() const { return chunksamps; }
	int				nSamps() const { return _nsamps; }
	int				inputChannels() const { return _input.inputchans; }
	int				outputChannels() const { return outputchans; }
	int				getSkip() const { return _skip; }
    FRAMETYPE       get_ichunkstart() const { return i_chunkstart; }
	// Use this to increment cursamp inside single-frame run loops.
	void			increment() { ++cursamp; }
	// Use this to increment cursamp inside block-based run loops.
	void	    	increment(int amount) { cursamp += amount; }
	void			setendsamp(FRAMETYPE end) { endsamp = end; }
	bool			needsToRun() const { return needs_to_run; }
	// These inlines are declared at bottom of this header.
	inline float	getstart() const;
	inline float	getdur() const;
	inline FRAMETYPE	getendsamp() const;
	inline void		setchunk(int);
	inline void		set_ichunkstart(FRAMETYPE);
	inline void		set_output_offset(int);
	inline const BusSlot *	getBusSlot() const;

	void 			schedule(heap *rtHeap);
	void			set_bus_config(const char *);
	virtual int		setup(PFieldSet *);				// Called by checkInsts()
	virtual int		init(double *, int);			// Called by setup()
	int				configure(int bufsamps);		// Called by inTraverse
	int				run(bool needsTo);
	virtual int		update(double *, int, unsigned fields=0);	// Called by run()
	double			update(int, int totframes=0);

	int				exec(BusType bus_type, int bus);
	void			addout(BusType bus_type, int bus);
	bool			isDone() const { return cursamp >= _nsamps; }
	const char *	name() const { return _name; }

	// These are called by the base class methods declared above.

	virtual int		configure();	// Sometimes overridden in derived class.
	virtual int		run() = 0;		// Always redefined in derived class.

// BGG -- added this for Ortgetin object support (see lib/Ortgetin.C)
   friend			class Ortgetin;

protected:
   // Methods which are called from within other methods
	Instrument();
	virtual		~Instrument();	// never called directly -- use unref()
   
	// This is called by set_bus_config() ONLY.
    void			setName(const char *name);
	// Methods for chaining instruments
	friend			class CHAIN;
	int				setChainedInputBuffer(BUFTYPE *inputBuf, int inputChannels);
	bool			hasChainedInput() const { return inputChainBuf != NULL; }
	
	static int		rtsetoutput(float, float, Instrument *);
	static int		rtsetinput(float, Instrument *);
	static int		rtinrepos(Instrument *, int, int);
	static int		rtgetin(float *, Instrument *, int);
	int				rtgetin(float *, int);
	int				rtaddout(BUFTYPE samps[]);  			// replacement for old rtaddout
	int				rtbaddout(BUFTYPE samps[], int length);	// block version of same

	const PField &	getPField(int index) const;
	const double *	getPFieldTable(int index, int *tableLen) const;

private:
   void				gone(); // decrements reference to input soundfile
};

/* ------------------------------------------------------------- getstart --- */
inline float Instrument::getstart() const
{
   return _start;
}

/* --------------------------------------------------------------- getdur --- */
inline float Instrument::getdur() const
{
   return _dur;
}

/* ----------------------------------------------------------- getendsamp --- */
inline FRAMETYPE Instrument::getendsamp() const
{
   return endsamp;
}


/* ------------------------------------------------------------- setchunk --- */
inline void Instrument::setchunk(int csamps)
{
   chunksamps = csamps;
}

/* ------------------------------------------------------ set_ichunkstart --- */
inline void Instrument::set_ichunkstart(FRAMETYPE csamps)
{
   i_chunkstart = csamps;
}

/* ---------------------------------------------------- set_output_offset --- */
inline void Instrument::set_output_offset(int offset)
{
   output_offset = offset;
}

inline const BusSlot *	Instrument::getBusSlot() const
{
	return _busSlot;
}

#endif /* _INSTRUMENT_H_  */

