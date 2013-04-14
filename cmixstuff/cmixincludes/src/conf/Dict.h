/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
/* Dictionary class, for use by the Config class.   -JGG, 6/24/04 */

#ifndef _DICT_H_
#define _DICT_H_ 1

#include <stdio.h>

typedef enum {
   kDictNoErr = 0,
   kDictNotBool,
   kDictNotDouble,
   kDictNotString
} DictErrorCode;

class DictItem {

public:
   DictItem() : _key(NULL), _next(NULL), _err(kDictNoErr) {}
   DictItem(const char *key)
      : _key((char *) key), _next(NULL), _err(kDictNoErr) {}
   virtual ~DictItem() { delete [] _key; };

   char *getKey() const { return _key; }
   void setKey(const char *key) { _key = (char *) key; }

   virtual bool getBoolValue() { return false; };
   virtual void setValue(const bool value) {};

   virtual double getDoubleValue() { return 0.0; };
   virtual void setValue(const double value) {};

   virtual char *getStringValue() { return NULL; };
   virtual void setValue(const char *value) {};

   DictItem *getNext() { return _next; }
   void setNextItem(DictItem *next) { _next = next; }

   DictErrorCode lastError() const { return _err; }   

   virtual void dump(FILE *stream) const {}

protected:
   void setErr(DictErrorCode err) { _err = err; }
private:
   char *_key;
   DictItem *_next;
   DictErrorCode _err;
};

class DictBoolItem : public DictItem {
public:
   DictBoolItem() : _value(false) {}
   DictBoolItem(const char *key, const bool value)
      : DictItem(key), _value(value) {}
   ~DictBoolItem() {};
   bool getBoolValue() { return _value; }
   double getDoubleValue() { setErr(kDictNotDouble); return 0.0; }
   char *getStringValue() { setErr(kDictNotString); return NULL; }
   void setValue(const bool value) { _value = value; }
   void dump(FILE *stream) const
   {
      fprintf(stream, "%s: %s\n", getKey(), _value ? "true" : "false");
   }
private:
   bool _value;
};

class DictDoubleItem : public DictItem {
public:
   DictDoubleItem() : _value(0.0) {}
   DictDoubleItem(const char *key, const double value)
      : DictItem(key), _value(value) {}
   ~DictDoubleItem() {};
   double getDoubleValue() { return _value; }
   bool getBoolValue() { setErr(kDictNotBool); return false; }
   char *getStringValue() { setErr(kDictNotString); return NULL; }
   void setValue(const double value) { _value = value; }
   void dump(FILE *stream) const
   {
      fprintf(stream, "%s: %f\n", getKey(), _value);
   }
private:
   double _value;
};

class DictStringItem : public DictItem {
public:
   DictStringItem() : _value(NULL) {}
   DictStringItem(const char *key, const char *value)
      : DictItem(key), _value((char *) value) {}
   ~DictStringItem() { delete [] _value; };     // NB: str alloc'd in conf.ypp
   char *getStringValue() { return _value; }
   bool getBoolValue() { setErr(kDictNotBool); return false; }
   double getDoubleValue() { setErr(kDictNotDouble); return 0.0; }
   void setValue(const char *value) { _value = (char *) value; }
   void dump(FILE *stream) const
   {
      fprintf(stream, "%s: \"%s\"\n", getKey(), _value);
   }
private:
   char *_value;
};

#endif /* _DICT_H_ */

