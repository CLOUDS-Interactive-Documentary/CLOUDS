/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
/* written by John Gibson, 6/26/04 */

#include <stdlib.h>
#include <string.h>
#include "Config.h"

#define MAX_STRLEN 1024

Config::~Config()
{
   DictItem *item = _head;
   while (item != NULL) {
      DictItem *next = item->getNext();
      delete item;
      item = next;
   }
}

void Config::insertDictItem(DictItem *item)
{
   item->setNextItem(_head);
   _head = item;
}

DictItem *Config::findDictItem(const char *key)
{
   DictItem *item = _head;
   while (item != NULL) {
      if (strncmp(item->getKey(), key, MAX_STRLEN) == 0)
         return item;
      item = item->getNext();
   }
   return NULL;
}

ConfigErrorCode Config::getValue(const char *key, bool& value)
{
   DictItem *item = findDictItem(key);
   if (item == NULL)
      return setErrorCode(kConfigNoValueForKey);
   value = item->getBoolValue();
   if (item->lastError() != kDictNoErr)
      return setErrorCode(kConfigValueWrongType);
   return setErrorCode(kConfigNoErr);
}

ConfigErrorCode Config::setValue(const char *key, const bool value)
{
   DictBoolItem *item = new DictBoolItem(key, value);
   insertDictItem(item);
   return setErrorCode(kConfigNoErr);
}

ConfigErrorCode Config::getValue(const char *key, double& value)
{
   DictItem *item = findDictItem(key);
   if (item == NULL)
      return setErrorCode(kConfigNoValueForKey);
   value = item->getDoubleValue();
   if (item->lastError() != kDictNoErr)
      return setErrorCode(kConfigValueWrongType);
   return setErrorCode(kConfigNoErr);
}

ConfigErrorCode Config::setValue(const char *key, const double value)
{
   DictDoubleItem *item = new DictDoubleItem(key, value);
   insertDictItem(item);
   return setErrorCode(kConfigNoErr);
}

ConfigErrorCode Config::getValue(const char *key, char *& value)
{
   DictItem *item = findDictItem(key);
   if (item == NULL)
      return setErrorCode(kConfigNoValueForKey);
   value = item->getStringValue();
   if (item->lastError() != kDictNoErr)
      return setErrorCode(kConfigValueWrongType);
   return setErrorCode(kConfigNoErr);
}

ConfigErrorCode Config::setValue(const char *key, const char *value)
{
   DictStringItem *item = new DictStringItem(key, value);
   insertDictItem(item);
   return setErrorCode(kConfigNoErr);
}

void Config::dump(FILE *stream)
{
   DictItem *item = _head;
   while (item != NULL) {
      item->dump(stream);
      item = item->getNext();
   }
}

const char *Config::getLastErrorText()
{
   if (_err == kConfigFileMissingErr)
      return "Configuration file doesn't exist";
   else if (_err == kConfigFileNoAccessErr)
      return "No read permission for configuration file";
   else if (_err == kConfigOpenFileErr)
      return "Can't open configuration file";
   else if (_err == kConfigParseFileErr)
      return "Trouble parsing configuration file";
   else if (_err == kConfigParseStreamErr)
      return "Trouble parsing configuration stream";
   else if (_err == kConfigNoValueForKey)
      return "Can't find a configuration value for key";
   else if (_err == kConfigValueWrongType)
      return "Configuration value is wrong type for key";
   return NULL;
}

extern int confparse(void *);
extern FILE *confin;
extern int confdebug;

ConfigErrorCode Config::parseStream(FILE *stream)
{
   confin = stream;
   confdebug = _debug ? 1 : 0;
   int status = confparse(this);
   if (status != 0)
      return setErrorCode(kConfigParseStreamErr);
   return setErrorCode(kConfigNoErr);
}

#include <errno.h>

ConfigErrorCode Config::parseFile(const char *fileName)
{
   FILE *stream = fopen(fileName, "r");
   if (stream == NULL) {
      if (errno == ENOENT || errno == ENOTDIR)
         return setErrorCode(kConfigFileMissingErr);
      else if (errno == EACCES)
         return setErrorCode(kConfigFileNoAccessErr);
      return setErrorCode(kConfigOpenFileErr);
   }
   ConfigErrorCode status = parseStream(stream);
   if (status == kConfigParseStreamErr)
      status = setErrorCode(kConfigParseFileErr);
   fclose(stream);
   return status;
}

