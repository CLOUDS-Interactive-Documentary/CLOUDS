#include <stdio.h>
#include <string.h>
#include "Config.h"


#define CONFIG_FILE ".rtcmixrc"

void
printBoolItem(Config *conf, char *key)
{
   bool val = false;
   if (conf->getValue(key, val) != kConfigNoErr)
      fprintf(stderr, "%s: %s.\n", conf->getLastErrorText(), key);
   else
      printf("val=%s\n", val? "true" : "false");
}

void
printDoubleItem(Config *conf, char *key)
{
   double val = 0.0;
   if (conf->getValue(key, val) != kConfigNoErr)
      fprintf(stderr, "%s: %s.\n", conf->getLastErrorText(), key);
   else
      printf("val=%f\n", val);
}

void
printStringItem(Config *conf, char *key)
{
   char *str = NULL;
   if (conf->getValue(key, str) != kConfigNoErr)
      fprintf(stderr, "%s: %s.\n", conf->getLastErrorText(), key);
   else
      printf("val=\"%s\"\n", str);
}

int
main()
{
   Config *conf = new Config();
// conf->setDebug();

#if 0
   conf->parseStream(stdin);
   printDoubleItem(conf, "x");
   printStringItem(conf, "y");
   printBoolItem(conf, "z");
#else
   conf->parseFile(CONFIG_FILE);
   conf->dump();
#endif

   delete conf;
   return 0;
}

