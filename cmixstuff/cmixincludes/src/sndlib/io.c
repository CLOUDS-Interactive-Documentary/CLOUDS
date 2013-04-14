/* IO handlers */
/*
 * --------------------------------
 * int mus_file_read(int fd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs)
 * int mus_file_write(int tfd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs)
 * long mus_file_seek(int tfd, long offset, int origin)
 * int mus_file_open_read(const char *arg) 
 * int mus_file_open_write(const char *arg)
 * int mus_file_create(const char *arg)
 * int mus_file_reopen_write(const char *arg)
 * int mus_file_close(int fd)
 * int mus_file_probe(const char *arg)
 * int mus_file_set_descriptors (int tfd, char *name, int df, int ds, int dl, int chns, int typ)
 * int mus_file_close_descriptors(int tfd)
 * char *mus_file_full_name(char *tok)
 * char *mus_format(const char *format, ...)
 * --------------------------------
 */

#if defined(HAVE_CONFIG_H)
  #include "config.h"
#else
  #ifndef HAVE_VPRINTF
    #define HAVE_VPRINTF 1
  #endif
#endif

#if USE_SND
  #include "snd.h"
#else
  #define PRINT_BUFFER_SIZE 512
  #define LABEL_BUFFER_SIZE 64
#endif

#include <math.h>
#include <stdio.h>
#if (!defined(HAVE_CONFIG_H)) || (defined(HAVE_FCNTL_H))
  #include <fcntl.h>
#endif
#if (!defined(HAVE_CONFIG_H)) || (defined(HAVE_LIMITS_H))
  #include <limits.h>
#endif
#include <errno.h>
#include <stdlib.h>

#if (defined(NEXT) || (defined(HAVE_LIBC_H) && (!defined(HAVE_UNISTD_H))))
  #include <libc.h>
#else
  #if (!(defined(_MSC_VER))) && (!(defined(MPW_C)))
    #include <unistd.h>
  #endif
#endif
#if (!defined(HAVE_CONFIG_H)) || (defined(HAVE_STRING_H))
  #include <string.h>
#endif
#if HAVE_VPRINTF
  #include <stdarg.h>
#endif

#include "sndlib.h"

/* data translations for big/little endian machines
 *   the m_* forms are macros where possible for speed (dating back to 1991 -- probably no longer needed -- gcc O3 switch?)
 */

void mus_bint_to_char(unsigned char *j, int x)
{
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if MUS_LITTLE_ENDIAN
  j[0] = ox[3]; j[1] = ox[2]; j[2] = ox[1]; j[3] = ox[0];
#else
  j[0] = ox[0]; j[1] = ox[1]; j[2] = ox[2]; j[3] = ox[3];
#endif
}

int mus_char_to_bint (const unsigned char *inp)
{
  int o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if MUS_LITTLE_ENDIAN
  outp[0] = inp[3]; outp[1] = inp[2]; outp[2] = inp[1]; outp[3] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1]; outp[2] = inp[2]; outp[3] = inp[3];
#endif
  return(o);
}

void mus_lint_to_char(unsigned char *j, int x)
{
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if (!MUS_LITTLE_ENDIAN)
  j[0] = ox[3]; j[1] = ox[2]; j[2] = ox[1]; j[3] = ox[0];
#else
  j[0] = ox[0]; j[1] = ox[1]; j[2] = ox[2]; j[3] = ox[3];
#endif
}

int mus_char_to_lint (const unsigned char *inp)
{
  int o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if (!MUS_LITTLE_ENDIAN)
  outp[0] = inp[3]; outp[1] = inp[2]; outp[2] = inp[1]; outp[3] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1]; outp[2] = inp[2]; outp[3] = inp[3];
#endif
  return(o);
}

int mus_char_to_uninterpreted_int (const unsigned char *inp)
{
  int o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
  outp[0] = inp[0]; outp[1] = inp[1]; outp[2] = inp[2]; outp[3] = inp[3];
  return(o);
}

unsigned int mus_char_to_ubint (const unsigned char *inp)
{
  unsigned int o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if MUS_LITTLE_ENDIAN
  outp[0] = inp[3]; outp[1] = inp[2]; outp[2] = inp[1]; outp[3] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1]; outp[2] = inp[2]; outp[3] = inp[3];
#endif
  return(o);
}

unsigned int mus_char_to_ulint (const unsigned char *inp)
{
  unsigned int o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if (!MUS_LITTLE_ENDIAN)
  outp[0] = inp[3]; outp[1] = inp[2]; outp[2] = inp[1]; outp[3] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1]; outp[2] = inp[2]; outp[3] = inp[3];
#endif
  return(o);
}


void mus_bfloat_to_char(unsigned char *j, float x)
{
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if MUS_LITTLE_ENDIAN
  j[0] = ox[3]; j[1] = ox[2]; j[2] = ox[1]; j[3] = ox[0];
#else
  j[0] = ox[0]; j[1] = ox[1]; j[2] = ox[2]; j[3] = ox[3];
#endif
}

float mus_char_to_bfloat (const unsigned char *inp)
{
  float o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if MUS_LITTLE_ENDIAN
  outp[0] = inp[3]; outp[1] = inp[2]; outp[2] = inp[1]; outp[3] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1]; outp[2] = inp[2]; outp[3] = inp[3];
#endif
  return(o);
}

void mus_lfloat_to_char(unsigned char *j, float x)
{
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if (!MUS_LITTLE_ENDIAN)
  j[0] = ox[3]; j[1] = ox[2]; j[2] = ox[1]; j[3] = ox[0];
#else
  j[0] = ox[0]; j[1] = ox[1]; j[2] = ox[2]; j[3] = ox[3];
#endif
}

float mus_char_to_lfloat (const unsigned char *inp)
{
  float o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if (!MUS_LITTLE_ENDIAN)
  outp[0] = inp[3]; outp[1] = inp[2]; outp[2] = inp[1]; outp[3] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1]; outp[2] = inp[2]; outp[3] = inp[3];
#endif
  return(o);
}

void mus_bshort_to_char(unsigned char *j, short x)
{
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if MUS_LITTLE_ENDIAN
  j[0] = ox[1]; j[1] = ox[0];
#else
  j[0] = ox[0]; j[1] = ox[1];
#endif
}

short mus_char_to_bshort (const unsigned char *inp)
{
  short o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if MUS_LITTLE_ENDIAN
  outp[0] = inp[1]; outp[1] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1];
#endif
  return(o);
}

void mus_lshort_to_char(unsigned char *j, short x)
{
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if (!MUS_LITTLE_ENDIAN)
  j[0] = ox[1]; j[1] = ox[0];
#else
  j[0] = ox[0]; j[1] = ox[1];
#endif
}

short mus_char_to_lshort (const unsigned char *inp)
{
  short o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if (!MUS_LITTLE_ENDIAN)
  outp[0] = inp[1]; outp[1] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1];
#endif
  return(o);
}

void mus_ubshort_to_char(unsigned char *j, unsigned short x)
{
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if MUS_LITTLE_ENDIAN
  j[0] = ox[1]; j[1] = ox[0];
#else
  j[0] = ox[0]; j[1] = ox[1];
#endif
}

unsigned short mus_char_to_ubshort (const unsigned char *inp)
{
  unsigned short o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if MUS_LITTLE_ENDIAN
  outp[0] = inp[1]; outp[1] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1];
#endif
  return(o);
}

void mus_ulshort_to_char(unsigned char *j, unsigned short x)
{
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if (!MUS_LITTLE_ENDIAN)
  j[0] = ox[1]; j[1] = ox[0];
#else
  j[0] = ox[0]; j[1] = ox[1];
#endif
}

unsigned short mus_char_to_ulshort (const unsigned char *inp)
{
  unsigned short o;
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if (!MUS_LITTLE_ENDIAN)
  outp[0] = inp[1]; outp[1] = inp[0];
#else
  outp[0] = inp[0]; outp[1] = inp[1];
#endif
  return(o);
}

double mus_char_to_ldouble (const unsigned char *inp)
{
  double o;
#if (!MUS_LITTLE_ENDIAN)
  int i;
#endif
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if (!MUS_LITTLE_ENDIAN)
  for (i = 0; i < 8; i++) outp[i] = inp[i];
#else
  outp[0] = inp[7]; outp[1] = inp[6]; outp[2] = inp[5]; outp[3] = inp[4]; outp[4] = inp[3]; outp[5] = inp[2]; outp[6] = inp[1]; outp[7] = inp[0];
#endif
  return(o);
}

double mus_char_to_bdouble (const unsigned char *inp)
{
  double o;
#if MUS_LITTLE_ENDIAN
  int i;
#endif
  unsigned char *outp;
  outp=(unsigned char *)&o;
#if (!MUS_LITTLE_ENDIAN)
  outp[0] = inp[7]; outp[1] = inp[6]; outp[2] = inp[5]; outp[3] = inp[4]; outp[4] = inp[3]; outp[5] = inp[2]; outp[6] = inp[1]; outp[7] = inp[0];
#else
  for (i = 0; i < 8; i++) outp[i] = inp[i];
#endif
  return(o);
}

void mus_bdouble_to_char(unsigned char *j, double x)
{
#if MUS_LITTLE_ENDIAN
  int i;
#endif
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if (!MUS_LITTLE_ENDIAN)
  j[0] = ox[7]; j[1] = ox[6]; j[2] = ox[5]; j[3] = ox[4]; j[4] = ox[3]; j[5] = ox[2]; j[6] = ox[1]; j[7] = ox[0];
#else
  for (i = 0; i < 8; i++) j[i] = ox[i];
#endif
}

void mus_ldouble_to_char(unsigned char *j, double x)
{
#if (!MUS_LITTLE_ENDIAN)
  int i;
#endif
  unsigned char *ox;
  ox=(unsigned char *)&x;
#if (!MUS_LITTLE_ENDIAN)
  for (i = 0; i < 8; i++) j[i] = ox[i];
#else
  j[0] = ox[7]; j[1] = ox[6]; j[2] = ox[5]; j[3] = ox[4]; j[4] = ox[3]; j[5] = ox[2]; j[6] = ox[1]; j[7] = ox[0];
#endif
}

#if MUS_LITTLE_ENDIAN

  #define m_big_endian_short(n)                   (mus_char_to_bshort(n))
  #define m_big_endian_int(n)                     (mus_char_to_bint(n))
  #define m_big_endian_float(n)                   (mus_char_to_bfloat(n))
  #define m_big_endian_double(n)                  (mus_char_to_bdouble(n))
  #define m_big_endian_unsigned_short(n)          (mus_char_to_ubshort(n))

  #define m_little_endian_short(n)                (*((short *)n))
  #define m_little_endian_int(n)                  (*((int *)n))
  #define m_little_endian_float(n)                (*((float *)n))
  #define m_little_endian_double(n)               (*((double *)n))
  #define m_little_endian_unsigned_short(n)       (*((unsigned short *)n))

  #define m_set_big_endian_short(n, x)             mus_bshort_to_char(n, x)
  #define m_set_big_endian_int(n, x)               mus_bint_to_char(n, x)
  #define m_set_big_endian_float(n, x)             mus_bfloat_to_char(n, x)
  #define m_set_big_endian_double(n, x)            mus_bdouble_to_char(n, x)
  #define m_set_big_endian_unsigned_short(n, x)    mus_ubshort_to_char(n, x)

  #define m_set_little_endian_short(n, x)          (*((short *)n)) = x
  #define m_set_little_endian_int(n, x)            (*((int *)n)) = x
  #define m_set_little_endian_float(n, x)          (*((float *)n)) = x
  #define m_set_little_endian_double(n, x)         (*((double *)n)) = x
  #define m_set_little_endian_unsigned_short(n, x) (*((unsigned short *)n)) = x

#else

  #ifndef SUN
    #define m_big_endian_short(n)                   (*((short *)n))
    #define m_big_endian_int(n)                     (*((int *)n))
    #define m_big_endian_float(n)                   (*((float *)n))
    #define m_big_endian_double(n)                  (*((double *)n))
    #define m_big_endian_unsigned_short(n)          (*((unsigned short *)n))

    #define m_set_big_endian_short(n, x)             (*((short *)n)) = x
    #define m_set_big_endian_int(n, x)               (*((int *)n)) = x
    #define m_set_big_endian_float(n, x)             (*((float *)n)) = x
    #define m_set_big_endian_double(n, x)            (*((double *)n)) = x
    #define m_set_big_endian_unsigned_short(n, x)    (*((unsigned short *)n)) = x
  #else
    #define m_big_endian_short(n)                   (mus_char_to_bshort(n))
    #define m_big_endian_int(n)                     (mus_char_to_bint(n))
    #define m_big_endian_float(n)                   (mus_char_to_bfloat(n))
    #define m_big_endian_double(n)                  (mus_char_to_bdouble(n))
    #define m_big_endian_unsigned_short(n)          (mus_char_to_ubshort(n))

    #define m_set_big_endian_short(n, x)             mus_bshort_to_char(n, x)
    #define m_set_big_endian_int(n, x)               mus_bint_to_char(n, x)
    #define m_set_big_endian_float(n, x)             mus_bfloat_to_char(n, x)
    #define m_set_big_endian_double(n, x)            mus_bdouble_to_char(n, x)
    #define m_set_big_endian_unsigned_short(n, x)    mus_ubshort_to_char(n, x)
  #endif

  #define m_little_endian_short(n)                  (mus_char_to_lshort(n))
  #define m_little_endian_int(n)                    (mus_char_to_lint(n))
  #define m_little_endian_float(n)                  (mus_char_to_lfloat(n))
  #define m_little_endian_double(n)                 (mus_char_to_ldouble(n))
  #define m_little_endian_unsigned_short(n)         (mus_char_to_ulshort(n))

  #define m_set_little_endian_short(n, x)            mus_lshort_to_char(n, x)
  #define m_set_little_endian_int(n, x)              mus_lint_to_char(n, x)
  #define m_set_little_endian_float(n, x)            mus_lfloat_to_char(n, x)
  #define m_set_little_endian_double(n, x)           mus_ldouble_to_char(n, x)
  #define m_set_little_endian_unsigned_short(n, x)   mus_ulshort_to_char(n, x)

#endif


/* ---------------- file descriptors ---------------- */

static int rt_ap_out;   /* address of RT audio ports, if any */

#ifdef CLM
void set_rt_audio_p (int rt)
{
  rt_ap_out = rt;
}
#endif

typedef struct {
  char *name;
  int data_format, bytes_per_sample, data_location, true_fd, data_clipped, chans, header_type;
  float prescaler;
} io_fd;

static int io_fd_size = 0;
static io_fd **io_fds = NULL;

int mus_file_set_descriptors (int tfd, const char *name, int format, int size, int location, int chans, int type)
{
  io_fd *fd;
  int i, lim = -1;
  if (io_fd_size == 0)
    {
#ifdef MACOS
      io_fd_size = 64;
#else
      io_fd_size = tfd + 8;
#endif
      io_fds = (io_fd **)CALLOC(io_fd_size, sizeof(io_fd *));
    }
#ifndef MACOS
  if (io_fd_size <= tfd)
    {
      lim = io_fd_size;
      io_fd_size = tfd + 8;
      io_fds = (io_fd **)REALLOC(io_fds, io_fd_size * sizeof(io_fd *));
      for (i = lim; i < io_fd_size; i++) io_fds[i] = NULL;
    }
#endif
  if (io_fds[tfd] == NULL)
    io_fds[tfd] = (io_fd *)CALLOC(1, sizeof(io_fd));

  fd = io_fds[tfd];
  fd->data_format = format;
  fd->bytes_per_sample = size;
  fd->data_location = location;
  fd->data_clipped = 0;
  fd->prescaler = 1.0;
  fd->header_type = type;
  fd->chans = chans;
  if (name)
    {
      fd->name = (char *)CALLOC(strlen(name) + 1, sizeof(char));
      strcpy(fd->name, name);
    }
  return(MUS_NO_ERROR);
}

int mus_file_close_descriptors(int tfd)
{
  io_fd *fd;
  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL)) return(MUS_ERROR);
  fd = io_fds[tfd];
  if (fd->name) {FREE(fd->name); fd->name = NULL;}
  FREE(fd);
  io_fds[tfd] = NULL;
  return(MUS_NO_ERROR);
}

int mus_file_set_data_clipped (int tfd, int clipped)
{
  io_fd *fd;
  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL)) return(MUS_ERROR);
  fd = io_fds[tfd];
  fd->data_clipped = clipped;
  return(MUS_NO_ERROR);
}

int mus_file_set_header_type (int tfd, int type)
{
  io_fd *fd;
  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL)) return(MUS_ERROR);
  fd = io_fds[tfd];
  fd->header_type = type;
  return(MUS_NO_ERROR);
}

int mus_file_header_type(int tfd)
{
  io_fd *fd;
  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL)) return(MUS_ERROR);
  fd = io_fds[tfd];
  return(fd->header_type);
}

float mus_file_prescaler(int tfd) 
{
  io_fd *fd;
  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL)) return(0.0);
  fd = io_fds[tfd];
  return(fd->prescaler);
}

float mus_file_set_prescaler(int tfd, float val) 
{
  io_fd *fd;
  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL)) return(0.0);
  fd = io_fds[tfd];
  fd->prescaler = val; 
  return(val);
}

char *mus_file_fd_name(int tfd)
{
  io_fd *fd;
  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL)) return(NULL);
  fd = io_fds[tfd];
  return(fd->name);
}

int mus_file_set_chans (int tfd, int chans)
{
  io_fd *fd;
  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL)) return(MUS_ERROR);
  fd = io_fds[tfd];
  fd->chans = chans;
  return(MUS_NO_ERROR);
}


/* ---------------- open, creat, close ---------------- */

int mus_file_open_read(const char *arg) 
{
#ifdef MACOS
  return(open (arg, O_RDONLY));
#else
  int fd;
  #ifdef WINDOZE
    fd = open (arg, O_RDONLY | O_BINARY);
  #else
    fd = open (arg, O_RDONLY, 0);
  #endif
  return(fd);
#endif
}

int mus_file_probe(const char *arg) 
{
  int fd;
#ifdef MACOS
  fd = (open (arg, O_RDONLY));
#else
  #ifdef WINDOZE
    fd = open (arg, O_RDONLY | O_BINARY);
  #else
    #ifdef O_NONBLOCK
      fd = open(arg, O_RDONLY, O_NONBLOCK);
    #else
      fd = open(arg, O_RDONLY, 0);
    #endif
  #endif
#endif
  if (fd == -1) return(0);
  if (close(fd) != 0) return(0);
  return(1);
}

int mus_file_open_write(const char *arg)
{
  int fd;
#ifdef MACOS
  if ((fd = open(arg, O_RDWR)) == -1)
  #ifdef MPW_C
    fd = creat(arg);
  #else
    fd = creat(arg, 0);
  #endif
  else
    lseek(fd, 0L, SEEK_END);
#else
  #ifdef WINDOZE
    if ((fd = open(arg, O_RDWR | O_BINARY)) == -1)
  #else
    if ((fd = open(arg, O_RDWR, 0)) == -1)
  #endif
      {
        fd = creat(arg, 0666);  /* equivalent to the new open(arg, O_RDWR | O_CREAT | O_TRUNC, 0666) */
      }
    else
      lseek(fd, 0L, SEEK_END);
#endif
  return(fd);
}

int mus_file_create(const char *arg)
{
#ifdef MACOS
  #ifdef MPW_C
    return(creat(arg));
  #else
    return(creat(arg, 0));
  #endif
#else
  int fd;
  fd = creat(arg, 0666);
  return(fd);
#endif
}

int mus_file_reopen_write(const char *arg)
{
#ifdef MACOS
  return(open(arg, O_RDWR));
#else
  int fd;
  #ifdef WINDOZE
    fd = open(arg, O_RDWR | O_BINARY);
  #else
    fd = open(arg, O_RDWR, 0);
  #endif
  return(fd);
#endif
}

int mus_file_close(int fd)
{
  if (mus_file_close_descriptors(fd) == MUS_NO_ERROR)
    return(close(fd));
  return(MUS_ERROR);
}



/* ---------------- seek ---------------- */

long mus_file_seek(int tfd, long offset, int origin)
{
  io_fd *fd;
  int siz;
  long loc, true_loc, header_end;
  if ((tfd == MUS_DAC_CHANNEL) || 
      (tfd == MUS_DAC_REVERB)) 
    return(0);
  if ((io_fds == NULL) || 
      (tfd >= io_fd_size) || 
      (tfd < 0) || 
      (io_fds[tfd] == NULL))
    {
      mus_error(MUS_FILE_DESCRIPTORS_NOT_INITIALIZED,
		"no file descriptor %d %d %d\n  [%s[%d] %s]",
		tfd, (int)offset, origin,
		__FILE__, __LINE__, __FUNCTION__);
      return(MUS_ERROR);
    }
  fd = io_fds[tfd];
  if (fd->data_format == MUS_UNKNOWN) 
    {
      mus_error(MUS_NOT_A_SOUND_FILE,
		"invalid stream: %s %d, %d, %d\n  [%s[%d] %s]",
		fd->name, tfd, (int)offset, origin,
		__FILE__, __LINE__, __FUNCTION__);
      return(MUS_ERROR);
    }
  siz = fd->bytes_per_sample;
  if ((siz == 2) || 
      (origin != 0))
    return(lseek(tfd, offset, origin));
  else
    {
      header_end = fd->data_location;
      loc = offset - header_end;
      switch (siz)
	{
	case 1: 
	  true_loc = lseek(tfd, header_end + (loc >> 1), origin);

	  /* now pretend we're still in 16-bit land and return where we "actually" are in that region */
	  /* that is, loc (in bytes) = how many (2-byte) samples into the file we want to go, return what we got */

	  return(header_end + ((true_loc - header_end) << 1));
	  break;
	case 3:
	  true_loc = lseek(tfd, header_end + loc + (loc >> 1), origin);
	  return(true_loc + ((true_loc - header_end) >> 1));
	  break;
	case 4:
	  true_loc = lseek(tfd, header_end + (loc << 1), origin);
	  return(header_end + ((true_loc - header_end) >> 1));
	  break;
	case 8:
	  true_loc = lseek(tfd, header_end + (loc << 2), origin);
	  return(header_end + ((true_loc - header_end) >> 2));
	  break;
	}
    }
  return(MUS_ERROR);
}

int mus_file_seek_frame(int tfd, int frame)
{
  io_fd *fd;
  if ((tfd == MUS_DAC_CHANNEL) || (tfd == MUS_DAC_REVERB)) return(0);
  if (io_fds == NULL) 
    {
      mus_error(MUS_FILE_DESCRIPTORS_NOT_INITIALIZED,
		"no file descriptors!\n  [%s[%d] %s]",
		__FILE__, __LINE__, __FUNCTION__); 
      return(MUS_ERROR);
    }
  if (tfd < 0)
    {
      mus_error(MUS_FILE_DESCRIPTORS_NOT_INITIALIZED,
		"file descriptor = %d?\n  [%s[%d] %s]",
		tfd, __FILE__, __LINE__, __FUNCTION__); 
      return(MUS_ERROR);
    }
  if ((tfd >= io_fd_size) || 
      (io_fds[tfd] == NULL))
    {
      mus_error(MUS_FILE_DESCRIPTORS_NOT_INITIALIZED,
		"file descriptors not realloc'd? (tfd: %d, io_fd_size: %d)\n  [%s[%d] %s]",
		tfd, io_fd_size,
		__FILE__, __LINE__, __FUNCTION__);
      return(MUS_ERROR);
    }
  fd = io_fds[tfd];
  if (fd->data_format == MUS_UNKNOWN) 
    {
      mus_error(MUS_NOT_A_SOUND_FILE,
		"invalid stream: %s %d, %d\n  [%s[%d] %s]",
		fd->name, tfd, frame, __FILE__, __LINE__, __FUNCTION__);
      return(MUS_ERROR);
    }
  return(lseek(tfd, fd->data_location + (fd->chans * frame * fd->bytes_per_sample), SEEK_SET));
}



/* ---------------- mulaw/alaw conversions ----------------
 *
 *      x : input signal with max value 32767
 *     mu : compression parameter (mu = 255 used for telephony)
 *     y = (32767/log(1+mu))*log(1+mu*abs(x)/32767)*sign(x); -- this isn't right -- typo?
 */

/* from sox g711.c */

#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)		/* Number of A-law segments. */
#define	SEG_SHIFT	(4)		/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */

static short seg_end[8] = {0xFF, 0x1FF, 0x3FF, 0x7FF,  0xFFF, 0x1FFF, 0x3FFF, 0x7FFF};

static int search(int val, short *table, int size)
{
  int i;
  for (i = 0; i < size; i++) {if (val <= *table++) return (i);}
  return (size);
}

static unsigned char to_alaw(int pcm_val)
{
  int mask, seg;
  unsigned char	aval;
  if (pcm_val >= 0) {mask = 0xD5;} else {mask = 0x55; pcm_val = -pcm_val - 8;}
  seg = search(pcm_val, seg_end, 8);
  if (seg >= 8)	return (0x7F ^ mask);
  else 
    {
      aval = seg << SEG_SHIFT;
      if (seg < 2) aval |= (pcm_val >> 4) & QUANT_MASK; else aval |= (pcm_val >> (seg + 3)) & QUANT_MASK;
      return (aval ^ mask);
    }
}

static const int alaw[256] = {
 -5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736, -7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784, 
 -2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368, -3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392, 
 -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944, -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136, 
 -11008, -10496, -12032, -11520, -8960, -8448, -9984, -9472, -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568, 
 -344, -328, -376, -360, -280, -264, -312, -296, -472, -456, -504, -488, -408, -392, -440, -424, 
 -88, -72, -120, -104, -24, -8, -56, -40, -216, -200, -248, -232, -152, -136, -184, -168, 
 -1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184, -1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696, 
 -688, -656, -752, -720, -560, -528, -624, -592, -944, -912, -1008, -976, -816, -784, -880, -848, 
 5504, 5248, 6016, 5760, 4480, 4224, 4992, 4736, 7552, 7296, 8064, 7808, 6528, 6272, 7040, 6784, 
 2752, 2624, 3008, 2880, 2240, 2112, 2496, 2368, 3776, 3648, 4032, 3904, 3264, 3136, 3520, 3392, 
 22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944, 30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136, 
 11008, 10496, 12032, 11520, 8960, 8448, 9984, 9472, 15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568, 
 344, 328, 376, 360, 280, 264, 312, 296, 472, 456, 504, 488, 408, 392, 440, 424, 
 88, 72, 120, 104, 24, 8, 56, 40, 216, 200, 248, 232, 152, 136, 184, 168, 
 1376, 1312, 1504, 1440, 1120, 1056, 1248, 1184, 1888, 1824, 2016, 1952, 1632, 1568, 1760, 1696, 
 688, 656, 752, 720, 560, 528, 624, 592, 944, 912, 1008, 976, 816, 784, 880, 848
};

#if 0
static int from_alaw(unsigned char a_val)
{
  int t, seg;
  a_val ^= 0x55;
  t = (a_val & QUANT_MASK) << 4;
  seg = ((unsigned)a_val & SEG_MASK) >> SEG_SHIFT;
  switch (seg) 
    {
    case 0: t += 8; break;
    case 1: t += 0x108; break;
  default:  t += 0x108; t <<= seg - 1;
    }
  return((a_val & SIGN_BIT) ? t : -t);
}
#endif 

#define	BIAS		(0x84)		/* Bias for linear code. */

static unsigned char to_mulaw(int pcm_val)
{
  int mask;
  int seg;
  unsigned char	uval;
  if (pcm_val < 0) {pcm_val = BIAS - pcm_val; mask = 0x7F;} else {pcm_val += BIAS; mask = 0xFF;}
  seg = search(pcm_val, seg_end, 8);
  if (seg >= 8) return (0x7F ^ mask);
  else 
    {
      uval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0xF);
      return (uval ^ mask);
    }
}

/* generated by SNDiMulaw on a NeXT -- see /usr/include/sound/mulaw.h */
static const int mulaw[256] = {
  -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956, -23932, -22908, -21884, -20860, 
  -19836, -18812, -17788, -16764, -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412, 
  -11900, -11388, -10876, -10364, -9852, -9340, -8828, -8316, -7932, -7676, -7420, -7164, -6908, 
  -6652, -6396, -6140, -5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092, -3900, -3772, -3644, 
  -3516, -3388, -3260, -3132, -3004, -2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980, -1884, 
  -1820, -1756, -1692, -1628, -1564, -1500, -1436, -1372, -1308, -1244, -1180, -1116, -1052, -988, 
  -924, -876, -844, -812, -780, -748, -716, -684, -652, -620, -588, -556, -524, -492, -460, -428, 
  -396, -372, -356, -340, -324, -308, -292, -276, -260, -244, -228, -212, -196, -180, -164, -148, 
  -132, -120, -112, -104, -96, -88, -80, -72, -64, -56, -48, -40, -32, -24, -16, -8, 0, 32124, 31100, 
  30076, 29052, 28028, 27004, 25980, 24956, 23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764, 
  15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412, 11900, 11388, 10876, 10364, 9852, 9340, 
  8828, 8316, 7932, 7676, 7420, 7164, 6908, 6652, 6396, 6140, 5884, 5628, 5372, 5116, 4860, 4604, 
  4348, 4092, 3900, 3772, 3644, 3516, 3388, 3260, 3132, 3004, 2876, 2748, 2620, 2492, 2364, 2236, 
  2108, 1980, 1884, 1820, 1756, 1692, 1628, 1564, 1500, 1436, 1372, 1308, 1244, 1180, 1116, 1052, 
  988, 924, 876, 844, 812, 780, 748, 716, 684, 652, 620, 588, 556, 524, 492, 460, 428, 396, 372, 
  356, 340, 324, 308, 292, 276, 260, 244, 228, 212, 196, 180, 164, 148, 132, 120, 112, 104, 96, 
  88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8, 0};

#if 0
/* in case it's ever needed, here's the mulaw to linear converter from g711.c -- identical to table above */
static int from_mulaw(unsigned char u_val)
{
  int t;
  u_val = ~u_val;
  t = ((u_val & QUANT_MASK) << 3) + BIAS;
  t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;
  return ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
}
#endif

/* ---------------- read/write buffer allocation ---------------- */

#if LONG_INT_P
static MUS_SAMPLE_TYPE **long_int_p_table = NULL;
static int long_int_p_table_size = 0;

MUS_SAMPLE_TYPE *mus_table2ptr(int arr) {return(long_int_p_table[arr]);}

int mus_ptr2table(MUS_SAMPLE_TYPE *arr) 
{
  int i, loc;
  loc = -1;
  for (i = 0; i < long_int_p_table_size; i++) 
    if (long_int_p_table[i] == NULL)
      {
	loc = i;
	break;
      }
  if (loc == -1)
    {
      loc = long_int_p_table_size;
      long_int_p_table_size+=16;
      if (long_int_p_table)
	{
	  long_int_p_table = (MUS_SAMPLE_TYPE **)REALLOC(long_int_p_table, long_int_p_table_size * sizeof(MUS_SAMPLE_TYPE *));
	  for (i = loc; i < long_int_p_table_size; i++) long_int_p_table[i] = NULL;
	}
      else
	long_int_p_table = (MUS_SAMPLE_TYPE **)CALLOC(long_int_p_table_size, sizeof(MUS_SAMPLE_TYPE *));
    }
  long_int_p_table[loc] = arr;
  return(loc);
}

void mus_untableptr(int ip_1) 
{
  MUS_SAMPLE_TYPE *ip; 
  ip = mus_table2ptr(ip_1);
  if (ip) FREE(ip); 
  long_int_p_table[ip_1] = NULL;
}
#endif

#define BUFLIM (64 * 1024)

static int checked_write(int tfd, char *buf, int chars)
{
  int bytes;
  io_fd *fd;
  if (tfd == MUS_DAC_CHANNEL)
    {
      mus_audio_write(rt_ap_out, buf, chars);
    }
  else
    {
      bytes = write(tfd, buf, chars);
      if (bytes != chars) 
	{
	  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL))
	    {
	      mus_error(MUS_FILE_DESCRIPTORS_NOT_INITIALIZED,
			"no file descriptors!\n  [%s[%d] %s]",
			__FILE__, __LINE__, __FUNCTION__); 
	      return(MUS_ERROR);
	    }
	  fd = io_fds[tfd];
	  if (fd->data_format == MUS_UNKNOWN) 
	    mus_error(MUS_FILE_CLOSED,
		      "attempt to write closed file %s",
		      fd->name);
	  else
#ifndef MACOS
	    mus_error(MUS_WRITE_ERROR,
		      "IO write error (%s%s%s): %d of %d bytes written for %d (%d %d %d)\n\n  [%s[%d] %s]",
		      fd->name, (errno) ? " " : "", (errno) ? strerror(errno) : "",
		      bytes, chars, tfd, fd->bytes_per_sample, fd->data_format, fd->data_location,
		      __FILE__, __LINE__, __FUNCTION__);
#else
	    mus_error(MUS_WRITE_ERROR,
		      "IO write error (%s): %d of %d bytes written for %d (%d %d %d %d)\n\n  [%s[%d] %s]",
		      fd->name, bytes, chars, tfd, (int)buf, fd->bytes_per_sample, fd->data_format, fd->data_location,
		      __FILE__, __LINE__, __FUNCTION__);
#endif
	  return(MUS_ERROR);
	}
    }
  return(MUS_NO_ERROR);
}

/* ---------------- read ---------------- */

static int mus_read_any_1(int tfd, int beg, int chans, int nints, MUS_SAMPLE_TYPE **bufs, MUS_SAMPLE_TYPE *cm, char *inbuf)
{
  int loclim;
  io_fd *fd;
  int bytes, j, lim, siz, total, leftover, total_read, k, loc, oldloc, siz_chans, buflim, format;
  unsigned char *jchar;
  char *charbuf = NULL;
  MUS_SAMPLE_TYPE *buffer;
  float prescaling;
  int from_buffer = 0;
  if (nints <= 0) return(0);
  if (inbuf) from_buffer = 1;
  if (!from_buffer)
    {
      if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL))
	{
	  mus_error(MUS_FILE_DESCRIPTORS_NOT_INITIALIZED,
		    "no file descriptors!\n  [%s[%d] %s]",
		    __FILE__, __LINE__, __FUNCTION__); 
	  return(MUS_ERROR);
	}
      fd = io_fds[tfd];
      if (fd->data_format == MUS_UNKNOWN) 
	{
	  mus_error(MUS_FILE_CLOSED,
		    "attempt to read closed file %s\n  [%s[%d] %s]",
		    fd->name, __FILE__, __LINE__, __FUNCTION__); 
	  return(MUS_ERROR);
	}
      format = fd->data_format;
      siz = fd->bytes_per_sample;
      if ((format == MUS_OUT_FORMAT) && (chans == 1) && (beg == 0)
#if SNDLIB_USE_FLOATS 
	  && (fd->prescaler == 1.0)
#endif
	  )
	{
	  bytes = nints * siz;
	  total = read(tfd, (char *)(bufs[0]), bytes);
	  if (total != bytes)
	    {
	      if (total <= 0)
		memset((void *)(bufs[0]), 0, bytes);
	      else
		{
		  int i, last;
		  last = beg + nints;
		  for (i = total / siz; i < last; i++)
		    bufs[0][i] = MUS_SAMPLE_0;
		}
	    }
	  return(total / siz);
	}

      prescaling = (float)(fd->prescaler * MUS_FLOAT_TO_SAMPLE(1.0));
      /* not MUS_FLOAT_TO_SAMPLE(fd->prescaler) here because there's a possible cast to int which can overflow */

      charbuf = (char *)CALLOC(BUFLIM, sizeof(char)); 
      if (charbuf == NULL) 
	{
	  mus_error(MUS_MEMORY_ALLOCATION_FAILED,
		    "IO buffer allocation trouble\n  [%s[%d] %s]",
		    __FILE__, __LINE__, __FUNCTION__); 
	  return(MUS_ERROR);
	}
    }
  else
    {
      charbuf = inbuf;
      siz = mus_data_format_to_bytes_per_sample(tfd);
      prescaling = MUS_FLOAT_TO_SAMPLE(1.0);
      format = tfd;
    }
  siz_chans = siz * chans;
  leftover = (nints * siz_chans);
  k = (BUFLIM) % siz_chans;
  if (k != 0) /* for example, 3 channel output of 1-byte (mulaw) samples will need a mod 3 buffer */
    buflim = (BUFLIM) - k;
  else buflim = BUFLIM;
  total_read = 0;
  loc = beg;
  while (leftover > 0)
    {
      bytes = leftover;
      if (bytes > buflim) 
	{
	  leftover = (bytes - buflim); 
	  bytes = buflim;
	} 
      else leftover = 0;
      if (!from_buffer)
	{
	  total = read(tfd, charbuf, bytes); 
	  if (total <= 0) 
	    {
	      /* zero out trailing section (some callers don't check the returned value) -- this added 9-May-99 */

	      lim = beg + nints;
	      if (loc < lim)
		for (k = 0; k < chans; k++)
		  if ((cm == NULL) || (cm[k]))
		    {
		      if (loc == 0)
			memset((void *)(bufs[k]), 0, lim * sizeof(MUS_SAMPLE_TYPE));
		      else
			for (j = loc; j < lim; j++) 
			  bufs[k][j] = MUS_SAMPLE_0;
		    }
	      FREE(charbuf);
	      return(total_read);
	    }
	  lim = (int) (total / siz_chans);  /* this divide must be exact (hence the buflim calc above) */
	}
      else
	{
	  lim = nints; /* frames in this case */
	  leftover = 0;
	}
      total_read += lim;
      oldloc = loc;

      for (k = 0; k < chans; k++)
	{
	  if ((cm == NULL) || (cm[k]))
	    {
	      buffer = (MUS_SAMPLE_TYPE *)(bufs[k]);
	      if (buffer)
		{
		  loc = oldloc;
		  loclim = loc + lim;
		  jchar = (unsigned char *)charbuf;
		  jchar += (k * siz);
		  switch (format)
		    {
		    case MUS_BSHORT:               
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_SHORT_TO_SAMPLE(m_big_endian_short(jchar)); 
		      break;
		    case MUS_LSHORT: 
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_SHORT_TO_SAMPLE(m_little_endian_short(jchar)); 
		      break;
		    case MUS_BINT:              
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_INT_TO_SAMPLE(m_big_endian_int(jchar)); 
		      break;
		    case MUS_LINT: 
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_INT_TO_SAMPLE(m_little_endian_int(jchar)); 
		      break;
		    case MUS_BINTN:              
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_INT_TO_SAMPLE((m_big_endian_int(jchar) >> (32 - MUS_SAMPLE_BITS)));
		      break;
		    case MUS_LINTN: 
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_INT_TO_SAMPLE((m_little_endian_int(jchar) >> (32 - MUS_SAMPLE_BITS)));
		      break;
		    case MUS_MULAW:  	              
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_SHORT_TO_SAMPLE(mulaw[*jchar]); 
		      break;
		    case MUS_ALAW:                  
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_SHORT_TO_SAMPLE(alaw[*jchar]); 
		      break;
		    case MUS_BYTE:                
		      for (; loc < loclim; loc++, jchar += siz_chans)
			buffer[loc] = MUS_BYTE_TO_SAMPLE((signed char)(*jchar));
		      break;
		    case MUS_UBYTE:     	      
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_BYTE_TO_SAMPLE((int)(*jchar) - 128);
		      break;
		    case MUS_BFLOAT:
		      if (prescaling == 1.0)
			{
			  for (; loc < loclim; loc++, jchar += siz_chans) 
			    buffer[loc] = (MUS_SAMPLE_TYPE) (m_big_endian_float(jchar));
			}
		      else
			{
			  for (; loc < loclim; loc++, jchar += siz_chans) 
			    buffer[loc] = (MUS_SAMPLE_TYPE) (prescaling * (m_big_endian_float(jchar)));
			}
		      break;
		    case MUS_BDOUBLE:   
		      for (; loc < loclim; loc++, jchar += siz_chans)
			buffer[loc] = (MUS_SAMPLE_TYPE) (prescaling * (m_big_endian_double(jchar)));
		      break;
		    case MUS_LFLOAT:    
		      if (prescaling == 1.0)
			{
			  for (; loc < loclim; loc++, jchar += siz_chans) 
			    buffer[loc] = (MUS_SAMPLE_TYPE) (m_little_endian_float(jchar));
			}
		      else
			{
			  for (; loc < loclim; loc++, jchar += siz_chans) 
			    buffer[loc] = (MUS_SAMPLE_TYPE) (prescaling * (m_little_endian_float(jchar)));
			}
		      break;
		    case MUS_LDOUBLE:   
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = (MUS_SAMPLE_TYPE) (prescaling * (m_little_endian_double(jchar)));
		      break;
		    case MUS_UBSHORT:   
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_SHORT_TO_SAMPLE((int)(m_big_endian_unsigned_short(jchar)) - 32768);
		      break;
		    case MUS_ULSHORT:   
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_SHORT_TO_SAMPLE((int)(m_little_endian_unsigned_short(jchar)) - 32768);
		      break;
		    case MUS_B24INT:
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_INT24_TO_SAMPLE((int)(((jchar[0] << 24) + 
								 (jchar[1] << 16) + 
								 (jchar[2] << 8)) >> 8));
		      break;
		    case MUS_L24INT:   
		      for (; loc < loclim; loc++, jchar += siz_chans) 
			buffer[loc] = MUS_INT24_TO_SAMPLE((int)(((jchar[2] << 24) + 
								 (jchar[1] << 16) + 
								 (jchar[0] << 8)) >> 8));
		      break;
		    }
		}
	    }
	}
    }
  if (!from_buffer) FREE(charbuf);
  return(total_read);
}

int mus_file_read_any(int tfd, int beg, int chans, int nints, MUS_SAMPLE_TYPE **bufs, MUS_SAMPLE_TYPE *cm)
{
  return(mus_read_any_1(tfd, beg, chans, nints, bufs, cm, NULL));
}

int mus_file_read_file(int tfd, int beg, int chans, int nints, MUS_SAMPLE_TYPE **bufs)
{
  return(mus_read_any_1(tfd, beg, chans, nints, bufs, NULL, NULL));
}

int mus_file_read_buffer(int charbuf_data_format, int beg, int chans, int nints, MUS_SAMPLE_TYPE **bufs, char *charbuf)
{
  return(mus_read_any_1(charbuf_data_format, beg, chans, nints, bufs, NULL, charbuf)); 
}

int mus_file_read(int tfd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs)
{
  int num, rtn, i, k;
  MUS_SAMPLE_TYPE *buffer;
  num = (end - beg + 1);
  rtn = mus_read_any_1(tfd, beg, chans, num, bufs, NULL, NULL);
  if (rtn == MUS_ERROR) return(MUS_ERROR);
  if (rtn < num) 
    /* this zeroing can be fooled if the file is chunked and has trailing, non-data chunks */
    for (k = 0; k < chans; k++)
      {
	buffer=(MUS_SAMPLE_TYPE *)(bufs[k]);
	for (i = rtn + beg; i <= end; i++)
	  buffer[i] = MUS_SAMPLE_0;
      }
  return(num);
}

int mus_file_read_chans(int tfd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs, MUS_SAMPLE_TYPE *cm)
{
  /* an optimization of mus_file_read -- just reads the desired channels */
  int num, rtn, i, k;
  MUS_SAMPLE_TYPE *buffer;
  num = (end - beg + 1);
  rtn = mus_read_any_1(tfd, beg, chans, num, bufs, cm, NULL);
  if (rtn == MUS_ERROR) return(MUS_ERROR);
  if (rtn < num) 
    for (k = 0; k < chans; k++)
      if ((cm == NULL) || (cm[k]))
	{
	  buffer = (MUS_SAMPLE_TYPE *)(bufs[k]);
	  for (i = rtn + beg; i <= end; i++)
	    buffer[i] = MUS_SAMPLE_0;
	}
  return(num);
}


/* ---------------- write ---------------- */

#ifdef WINDOZE
  #undef min
#endif

#define min(x, y)  ((x) < (y) ? (x) : (y))

int mus_file_write_zeros(int tfd, int num)
{
  int lim, curnum, err;
  io_fd *fd;
  char *charbuf = NULL;
  if (tfd == MUS_DAC_REVERB) return(0);
  if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL))
    {
      mus_error(MUS_FILE_DESCRIPTORS_NOT_INITIALIZED,
		"no file descriptors!\n  [%s[%d] %s]",
		__FILE__, __LINE__, __FUNCTION__); 
      return(MUS_ERROR);
    }
  fd = io_fds[tfd];
  if (fd->data_format == MUS_UNKNOWN) 
    {
      mus_error(MUS_FILE_CLOSED,
		"attempt to write closed file %s\n  [%s[%d] %s]",
		fd->name, __FILE__, __LINE__, __FUNCTION__); 
      return(MUS_ERROR);
    }
  charbuf = (char *)CALLOC(BUFLIM, sizeof(char)); 
  if (charbuf == NULL) 
    {
      mus_error(MUS_MEMORY_ALLOCATION_FAILED,
		"IO buffer allocation trouble\n  [%s[%d] %s]",
		__FILE__, __LINE__, __FUNCTION__); 
      return(MUS_ERROR);
    }
  lim = num * (fd->bytes_per_sample);
  curnum = min(lim, BUFLIM);
  /* for (i = 0; i < curnum; i++) charbuf[i] = 0; */ /* using calloc, so surely this is unnecessary? */
  while (curnum > 0)
    {
      err = checked_write(tfd, charbuf, curnum);
      if (err == MUS_ERROR) return(MUS_ERROR);
      lim -= (BUFLIM);
      curnum = min(lim, BUFLIM);
    }
  FREE(charbuf);
  return(num);
}

static int mus_write_1(int tfd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs, char *inbuf, int clipped)
{
  int loclim, c3;
  int err;
  io_fd *fd;
  int bytes, j, k, lim, siz, leftover, loc, bk, val, oldloc, buflim, siz_chans, cliploc, data_format, clipping = 0;
  unsigned char *jchar;
  char *charbuf = NULL;
  int to_buffer = 0;
  MUS_SAMPLE_TYPE *buffer;
  if (chans <= 0) return(0);
  if (inbuf) to_buffer = 1;
  if (!to_buffer)
    {
      if (tfd == MUS_DAC_REVERB) return(0);
      if ((io_fds == NULL) || (tfd >= io_fd_size) || (tfd < 0) || (io_fds[tfd] == NULL))
	{
	  mus_error(MUS_FILE_DESCRIPTORS_NOT_INITIALIZED,
		    "no file descriptors!\n  [%s[%d] %s]",
		    __FILE__, __LINE__, __FUNCTION__);
	  return(MUS_ERROR);
	}
      fd = io_fds[tfd];
      if (fd->data_format == MUS_UNKNOWN) 
	{
	  mus_error(MUS_FILE_CLOSED,
		    "attempt to write closed file %s\n  [%s[%d] %s]",
		    fd->name, __FILE__, __LINE__, __FUNCTION__); 
	  return(MUS_ERROR);
	}

      siz = fd->bytes_per_sample;
      data_format = fd->data_format;
      clipping = fd->data_clipped;

      if ((data_format == MUS_OUT_FORMAT) && (chans == 1) && (clipping == 0) && (beg == 0))
	{
	  bytes = (end + 1) * siz;
	  return(checked_write(tfd, (char *)(bufs[0]), bytes));
	}
      charbuf = (char *)CALLOC(BUFLIM, sizeof(char)); 
      if (charbuf == NULL) 
	{
	  mus_error(MUS_MEMORY_ALLOCATION_FAILED,
		    "IO buffer allocation trouble\n  [%s[%d] %s]",
		    __FILE__, __LINE__, __FUNCTION__);
	  return(MUS_ERROR);
	}
    }
  else
    {
      charbuf = inbuf;
      siz = mus_data_format_to_bytes_per_sample(tfd);
      data_format = tfd;
      clipping = clipped;
    }
  lim = (end - beg + 1);
  siz_chans = siz * chans;
  leftover = lim * siz_chans;
  k = (BUFLIM) % siz_chans;
  if (k != 0) 
    buflim = (BUFLIM) - k;
  else buflim = BUFLIM;
  loc = beg;
  while (leftover > 0)
    {
      bytes = leftover;
      if (bytes > buflim) 
	{
	  leftover = (bytes - buflim); 
	  bytes = buflim;
	} 
      else leftover = 0;
      lim = (int)(bytes / siz_chans); /* see note above */
      oldloc = loc;

      for (k = 0; k < chans; k++)
	{
	  loc = oldloc;
	  buffer = (MUS_SAMPLE_TYPE *)(bufs[k]);
	  if (clipping)
	    {
	      cliploc = oldloc;
	      for (j = 0; j < lim; j++, cliploc++)
		if (buffer[cliploc] > MUS_SAMPLE_MAX)
		  buffer[cliploc] = MUS_SAMPLE_MAX;
		else
		  if (buffer[cliploc] < MUS_SAMPLE_MIN)
		    buffer[cliploc] = MUS_SAMPLE_MIN;
	    }
	  loclim = loc + lim;
	  jchar = (unsigned char *)charbuf; /* if to_buffer we should add the loop offset here, or never loop */
	  jchar += (k * siz); 
	  switch (data_format)
	    {
	    case MUS_BSHORT: 
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_big_endian_short(jchar, MUS_SAMPLE_TO_SHORT(buffer[loc]));
	      break;
	    case MUS_LSHORT:   
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_little_endian_short(jchar, MUS_SAMPLE_TO_SHORT(buffer[loc]));
	      break;
	    case MUS_BINT:   
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_big_endian_int(jchar, MUS_SAMPLE_TO_INT(buffer[loc]));
	      break;
	    case MUS_LINT:   
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_little_endian_int(jchar, MUS_SAMPLE_TO_INT(buffer[loc]));
	      break;
	    case MUS_BINTN:   
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_big_endian_int(jchar, MUS_SAMPLE_TO_INT(buffer[loc]) << (32 - MUS_SAMPLE_BITS));
	      break;
	    case MUS_LINTN:   
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_little_endian_int(jchar, MUS_SAMPLE_TO_INT(buffer[loc]) << (32 - MUS_SAMPLE_BITS));
	      break;
	    case MUS_MULAW:     
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		(*jchar) = to_mulaw(MUS_SAMPLE_TO_SHORT(buffer[loc]));
	      break;
	    case MUS_ALAW:      
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		(*jchar) = to_alaw(MUS_SAMPLE_TO_SHORT(buffer[loc]));
	      break;
	    case MUS_BYTE:    
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		(*((signed char *)jchar)) = MUS_SAMPLE_TO_BYTE(buffer[loc]);
	      break;
	    case MUS_UBYTE:  
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		(*jchar) = MUS_SAMPLE_TO_BYTE(buffer[loc]) + 128;
	      break;
	    case MUS_BFLOAT:    
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_big_endian_float(jchar, MUS_SAMPLE_TO_FLOAT(buffer[loc]));
	      break;
	    case MUS_LFLOAT:    
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_little_endian_float(jchar, MUS_SAMPLE_TO_FLOAT(buffer[loc]));
	      break;
	    case MUS_BDOUBLE:
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_big_endian_double(jchar, MUS_SAMPLE_TO_DOUBLE(buffer[loc]));
	      break;
	    case MUS_LDOUBLE:   
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_little_endian_double(jchar, MUS_SAMPLE_TO_DOUBLE(buffer[loc]));
	      break;
	    case MUS_UBSHORT: 
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_big_endian_unsigned_short(jchar, (unsigned short)(MUS_SAMPLE_TO_SHORT(buffer[loc]) + 32768));
	      break;
	    case MUS_ULSHORT: 
	      for (; loc < loclim; loc++, jchar += siz_chans) 
		m_set_little_endian_unsigned_short(jchar, (unsigned short)(MUS_SAMPLE_TO_SHORT(buffer[loc]) + 32768));
	      break;
	    case MUS_B24INT:   
	      bk = (k * 3);
	      c3 = chans * 3;
	      for (; loc < loclim; loc++, bk += c3) 
		{
		  val = MUS_SAMPLE_TO_INT24(buffer[loc]);
		  charbuf[bk] = (val >> 16); 
		  charbuf[bk + 1] = (val >> 8); 
		  charbuf[bk + 2] = (val & 0xFF); 
		}
	      break;
	    case MUS_L24INT:   
	      bk = (k * 3);
	      c3 = chans * 3;
	      for (; loc < loclim; loc++, bk += c3)
		{
		  val = MUS_SAMPLE_TO_INT24(buffer[loc]);
		  charbuf[bk + 2] = (val >> 16); 
		  charbuf[bk + 1] = (val >> 8); 
		  charbuf[bk] = (val & 0xFF); 
		}
	      break;
	    }
	}
      if (!to_buffer)
	{
	  err = checked_write(tfd, charbuf, bytes);
	  if (err == MUS_ERROR) 
	    {
	      FREE(charbuf); 
	      return(MUS_ERROR);
	    }
	}
    }
  if (!to_buffer) FREE(charbuf);
  return(MUS_NO_ERROR);
}

int mus_file_write(int tfd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs)
{
  return(mus_write_1(tfd, beg, end, chans, bufs, NULL, 0));
}

int mus_file_write_file(int tfd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs)
{
  return(mus_write_1(tfd, beg, end, chans, bufs, NULL, 0));
}

int mus_file_write_buffer(int charbuf_data_format, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs, char *charbuf, int clipped)
{
  return(mus_write_1(charbuf_data_format, beg, end, chans, bufs, charbuf, clipped));
}


#ifdef CLM
void reset_io_c(void) 
{
  io_fd_size = 0;
  io_fds = NULL;
#if LONG_INT_P
  long_int_p_table = NULL;
  long_int_p_table_size = 0;
#endif
}
#endif

char *mus_expand_filename(char *utok)
{
  /* fill out under-specified library pathnames etc */
  /* what about "../" and "./" ? these work, but perhaps we should handle them explicitly) */
  char *file_name_buf, *tok;
  int i, j, len;
  tok = utok;
  if ((tok) && (*tok)) 
    len = strlen(tok); 
  else return(NULL);
  file_name_buf = (char *)CALLOC(MUS_MAX_FILE_NAME, sizeof(char));
  j = 0;
  for (i = 0; i < len - 1; i++)
    {
      if ((tok[i] == '/') && 
	  (tok[i + 1] == '/')) 
	j = i + 1;
    }
  if (j > 0)
    {
      for (i = 0; j < len; i++, j++) 
	tok[i] = tok[j];
      tok[i] ='\0';
    }
#ifdef MACOS
  strcpy(file_name_buf, tok);
#else
  if (tok[0] != '/')
    {
      file_name_buf[0] = '\0';
      if (tok[0] == '~')
	{
	  strcpy(file_name_buf, getenv("HOME"));
	  strcat(file_name_buf, ++tok);
	}
      else
	{
#if (!defined(NEXT)) || defined(HAVE_GETCWD)
	  getcwd(file_name_buf, MUS_MAX_FILE_NAME);
#else
	  getwd(file_name_buf);
#endif
	  strcat(file_name_buf, "/");
	  strcat(file_name_buf, tok);
	}
    }
  else strcpy(file_name_buf, tok);
#endif
 return(file_name_buf);
}


void mus_snprintf(char *buffer, int buffer_len, const char *format, ...)
{
#if HAVE_VPRINTF
  va_list ap;
  va_start(ap, format);
#if HAVE_VSNPRINTF
  vsnprintf(buffer, buffer_len, format, ap);
#else
  vsprintf(buffer, format, ap);
#endif
  va_end(ap);
#else
  buffer[0] = '\0';
  strcat(buffer, format);
#endif
}

#define MUS_FORMAT_STRING_MAX 1024

char *mus_format(const char *format, ...)
{
  /* this is primarily intended for error messages */
  /* caller should free result */
  char *buf;
#if HAVE_VPRINTF
  va_list ap;
  buf = (char *)CALLOC(MUS_FORMAT_STRING_MAX, sizeof(char));
  va_start(ap, format);
#if HAVE_VSNPRINTF
  vsnprintf(buf, MUS_FORMAT_STRING_MAX, format, ap);
#else
  vsprintf(buf, format, ap);
#endif
  va_end(ap);
#else
  buf = (char *)CALLOC(MUS_FORMAT_STRING_MAX, sizeof(char));
#if HAVE_SNPRINTF
  snprintf(buf, MUS_FORMAT_STRING_MAX, "%s...[you need vprintf]", format);
#else
  sprintf(buf, "%s...[you need vprintf]", format);
#endif
#endif
  return(buf);
}

MusFloat mus_fclamp(MusFloat lo, MusFloat val, MusFloat hi) 
{
  if (val > hi) 
    return(hi); 
  else 
    if (val < lo) 
      return(lo); 
    else return(val);
}

int mus_iclamp(int lo, int val, int hi) 
{
  if (val > hi) 
    return(hi); 
  else 
    if (val < lo) 
      return(lo); 
    else return(val);
}

