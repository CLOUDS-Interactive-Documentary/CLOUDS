/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _CONFIG_H_
#define _CONFIG_H_ 1

#include <stdio.h>
#include "Dict.h"

/* A Config object manages a dictionary of settings read from a text I/O
   stream.  It offers methods to retrieve settings from the dictionary,
   and to add new settings to it.

   Each dictionary item is a key/value pair.  The key is a string; the
   value can be one of three types: bool, double or string.  The text
   format of these items in the I/O stream is:

      key = false | FALSE | true | TRUE
      key = double_value
      key = "string"

   So, strings are double or single-quoted, but the key text and the
   true and false Boolean values are not.  Simple arithmetic can be
   performed for double values.  So you can have:

      time_to_wait = 1000 * 3    # 3000 seconds

   Shell-style comments (anything on a line following '#') are ignored.

   Assume "myconfigfile" contains these lines:

      default_directory = "/home/me/finances/taxes"
      my_bank_balance = 1000000 * .00001
      pay_estimated_tax = false

   Code to read all the key = value pairs would look like this.

      Config *config = new Config();
      config->parseFile("myconfigfile");

   Then, code to retrieve one of these values could look like this.

      char *key = "default_directory";
      char *dirname = NULL;
      if (conf->getValue(key, dirname) != kConfigNoErr)
         fprintf(stderr, "%s: %s.\n", conf->getLastErrorText(), key);

   If there was no error, <dirname> now points to a string, allocated
   by the Config object, whose contents is "/home/me/finances/taxes".
   Do not delete this string; this will happen when you delete the Config
   object; so if you need the string to persist beyond Config deletion,
   make a copy of it.

   If there was an error, getValue returns a non-zero value.  You can
   print an error message using the getLastErrorText method.  If there
   were no "default_directory" key in the dictionary, then the code
   above would print:

      Can't find a configuration value for key: default_directory.

   If there is a key, but you've passed a type to getValue that does
   not match the type of the key's value, you would see:

      Configuration value is wrong type for key: default_directory.

   After using the Config object, deleting it will free all storage
   associated with it, including all dictionary strings and parser
   structures.

   Note that nothing is ever deleted from the dictionary.  If you
   store (or the input file contains) a new value for a key, that
   is the value that will be retrieved by getValue.  The former
   key/value pair remains in the dictionary, but it will be
   inaccessible.

                                             - John Gibson, 6/26/04
*/

typedef enum {
   kConfigNoErr = 0,
   kConfigFileMissingErr,
   kConfigFileNoAccessErr,
   kConfigOpenFileErr,
   kConfigParseFileErr,
   kConfigParseStreamErr,
   kConfigNoValueForKey,
   kConfigValueWrongType
} ConfigErrorCode;

class Config {

public:
   Config() : _head(NULL), _err(kConfigNoErr), _debug(false) {}
   ~Config();

   ConfigErrorCode parseFile(const char *fileName);
   ConfigErrorCode parseStream(FILE *stream);

   ConfigErrorCode getValue(const char *key, bool& value);
   ConfigErrorCode setValue(const char *key, const bool value);

   ConfigErrorCode getValue(const char *key, double& value);
   ConfigErrorCode setValue(const char *key, const double value);

   ConfigErrorCode getValue(const char *key, char *& value);
   ConfigErrorCode setValue(const char *key, const char *value);

   ConfigErrorCode getLastErrorCode() { return _err; }
   const char *getLastErrorText();

   void setDebug(bool doit) { _debug = doit; }
   void setDebug() { _debug = true; }
   void dump(FILE *stream);
   void dump() { dump(stdout); }

private:
   void insertDictItem(DictItem *item);
   DictItem *findDictItem(const char *key);
   inline ConfigErrorCode setErrorCode(const ConfigErrorCode code)
      { _err = code; return code; }

   DictItem *_head;
   ConfigErrorCode _err;
   bool _debug;
};

#endif /* _CONFIG_H_ */

