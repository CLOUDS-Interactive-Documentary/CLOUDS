/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef _RT_TYPES_H_
#define _RT_TYPES_H_

/* type of buffer used for internal buses */
#define BUFTYPE float           /* could be double some day */
typedef BUFTYPE *BufPtr;

#if BUFTYPE == float
#define ZERO 0.0f
#else
#define ZERO 0.0
#endif

/* type for frame counts (Inst start and end points) */
#define FRAMETYPE long long

/* This should probably go someplace else in this file? */
typedef enum {
  NO = 0,
  YES
} Bool;

#ifndef PI
#define      PI     3.141592654
#endif
#ifndef PI2
#define      PI2    6.2831853
#endif

#define FLOAT (sizeof(float))   /* nbytes in floating point word*/
#define INT   (sizeof(int))   /* nbytes in integer word */
#define SHORT (sizeof(short))
#define LONG  (sizeof(long))

#ifdef __cplusplus

#ifdef MULTI_THREAD

#ifdef MACOSX

#include <libkern/OSAtomic.h>

class AtomicInt
{
    int32_t val;
public:
    AtomicInt(int inVal=0) : val(inVal) {}
    operator int () const { return val; }
	void increment() { OSAtomicIncrement32(&val); }
    bool incrementAndTest() { return OSAtomicIncrement32(&val) == 0; }
    bool decrementAndTest() { return OSAtomicDecrement32(&val) == 0; }
    int operator = (int rhs) { return (val = rhs); }
    
};

#elif defined(LINUX)

#include <alsa/iatomic.h>

class AtomicInt
{
    atomic_t val;
public:
    AtomicInt(int inVal=0) { atomic_set(&val, inVal); }
    operator int () const { return atomic_read(&val); }
    void increment() { atomic_inc(&val); }
    bool incrementAndTest() { return atomic_inc_and_test(&val); }
    bool decrementAndTest() { return atomic_dec_and_test(&val); }
    int operator = (int rhs) { atomic_set(&val, rhs); return atomic_read(&val); }
};

#else

#error Tell Doug Scott you are trying to compile MULTI_THREAD for this platform.
typedef int AtomicInt;

#endif

#else	// !MULTI_THREAD

class AtomicInt
{
    int val;
public:
    AtomicInt(int inVal=0) : val(inVal) {}
    operator int () const { return val; }
    void increment() { ++val; }
    bool incrementAndTest() { return ++val == 0; }
    bool decrementAndTest() { return --val == 0; }
    int operator = (int rhs) { return (val = rhs); }
};

#endif  // !MULTI_THREAD

#endif  // __cplusplus

#endif	// _RT_TYPES_H_
