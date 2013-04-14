/* Some stuff pulled out of sound.c and audio.c to avoid compiling those files.
   Since we use sndlib only for reading and writing headers, we don't really
   need large parts of the library.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "sndlib.h"

#define HAVE_VPRINTF 1

static mus_error_handler_t *mus_error_handler = NULL;

mus_error_handler_t *mus_error_set_handler(mus_error_handler_t *new_error_handler) 
{
  mus_error_handler_t *old_handler;
  old_handler = mus_error_handler;
  mus_error_handler = new_error_handler;
  return(old_handler);
}

#define MUS_ERROR_BUFFER_SIZE 1024
static char *mus_error_buffer = NULL;

void mus_error(int error, const char *format, ...)
{
#if HAVE_VPRINTF
  va_list ap;
  if (mus_error_buffer == NULL)
    mus_error_buffer = (char *)CALLOC(MUS_ERROR_BUFFER_SIZE, sizeof(char));
  va_start(ap, format);
#if HAVE_VSNPRINTF
  vsnprintf(mus_error_buffer, MUS_ERROR_BUFFER_SIZE, format, ap);
#else
  vsprintf(mus_error_buffer, format, ap);
#endif
  va_end(ap);
  if (mus_error_handler)
    (*mus_error_handler)(error, mus_error_buffer);
  else 
    {
      fprintf(stderr, "%s", mus_error_buffer);
      fputc('\n', stderr);
    }
#else
  if (error == 0) /* this case mainly for CLM */
    fprintf(stderr, format);
  else fprintf(stderr, "error: %d %s\n", error, format);
#endif
}

static mus_print_handler_t *mus_print_handler = NULL;

mus_print_handler_t *mus_print_set_handler(mus_print_handler_t *new_print_handler) 
{
  mus_print_handler_t *old_handler;
  old_handler = mus_print_handler;
  mus_print_handler = new_print_handler;
  return(old_handler);
}

void mus_print(const char *format, ...)
{
#if HAVE_VPRINTF
  va_list ap;
  if (mus_error_buffer == NULL)
    mus_error_buffer = (char *)CALLOC(MUS_ERROR_BUFFER_SIZE, sizeof(char));
  if (mus_print_handler)
    {
      va_start(ap, format);
#if HAVE_VSNPRINTF
      vsnprintf(mus_error_buffer, MUS_ERROR_BUFFER_SIZE, format, ap);
#else
      vsprintf(mus_error_buffer, format, ap);
#endif
      va_end(ap);
      (*mus_print_handler)(mus_error_buffer);
    }
  else
    {
      va_start(ap, format);
      vfprintf(stdout, format, ap);
      va_end(ap);
    }
#endif
}


/* from audio.c */
int mus_audio_write(int line, char *buf, int bytes)
{
  return(MUS_NO_ERROR);
}

