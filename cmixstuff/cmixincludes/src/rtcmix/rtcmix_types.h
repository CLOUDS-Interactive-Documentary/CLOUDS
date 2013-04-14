#ifndef _RTCMIX_TYPES_H_
#define _RTCMIX_TYPES_H_ 1

#include <stdio.h>

typedef enum {
   VoidType = 0,
   DoubleType,
   StringType,
   HandleType,
   ArrayType
} RTcmixType;

typedef enum {
   PFieldType,
   InstrumentPtrType,
   AudioStreamType
} RTcmixHandleType;

typedef struct _handle {
   RTcmixHandleType type;
   void *ptr;
   int refcount;
} *RtcHandle;

typedef struct {
   unsigned int len;    // number of elements in <data> array
   double *data;
} Array;

typedef union {
   double		number;
   const char	*string;
   RtcHandle		handle;
   Array		*array;
} Value;

// Arg operates as a struct in C, and a class in C++.

#ifdef __cplusplus
class PField;
class Instrument;
struct Arg {
#else
typedef struct {
#endif
   RTcmixType _type;
   Value _val;
#ifdef __cplusplus
   Arg() : _type(VoidType) { _val.number = 0.0; }
   ~Arg();
   RTcmixType type() const { return this->_type; }
   bool isType(RTcmixType type) const { return _type == type; }
   void operator = (double d) { _type = DoubleType; _val.number = d; }
   void operator = (const char *c) { _type = StringType; _val.string = c; }
   void operator = (const RtcHandle h);
   void operator = (Array *a) { _type = ArrayType; _val.array = a; }
   operator double () const { return _val.number; }
   operator float () const { return (float) _val.number; }
   operator int () const { return (int) _val.number; }
   operator unsigned int () const { return (unsigned int) _val.number; }
   const char * string() const { return _val.string; }
   operator const char * () const { return string(); }
   bool operator == (const char *str) const;
   bool operator != (const char *str) const { return !(*this == str); }
   operator RtcHandle () const { return _val.handle; }
   operator Array *() const { return _val.array; }
   operator PField *() const {
	   return (isType(HandleType) && _val.handle->type == PFieldType) ?
		   (PField *) _val.handle->ptr : NULL;
   }
   operator Instrument *() const {
	   return (isType(HandleType) && _val.handle->type == InstrumentPtrType) ?
		   (Instrument *) _val.handle->ptr : NULL;
   }
   void printInline(FILE *) const;
};
#else
} Arg;
#endif

#endif /* _RTCMIX_TYPES_H_ */
