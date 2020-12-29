#define TEST_COMMON_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#if defined(BITS_TEST_C) || defined(DECODE_TEST_C)
#  include "../../src/bits.h"
#endif
#ifndef DECODE_TEST_C
  static unsigned int loglevel;
#  define DWG_LOGLEVEL loglevel
#  include "../../src/logging.h"
#endif
#include "../../src/config.h"
#include "../../src/common.h"

#if defined(_WIN32) && defined(HAVE_FUNC_ATTRIBUTE_MS_FORMAT) && !defined(__USE_MINGW_ANSI_STDIO)
#  define ATTRIBUTE_FORMAT(x, y) __attribute__ ((format (ms_printf, x, y)))
#elif defined HAVE_FUNC_ATTRIBUTE_GNU_FORMAT
#  define ATTRIBUTE_FORMAT(x, y) __attribute__ ((format (gnu_printf, x, y)))
#elif defined HAVE_FUNC_ATTRIBUTE_FORMAT
#  define ATTRIBUTE_FORMAT(x, y) __attribute__ ((format (printf, x, y)))
#else
#  define ATTRIBUTE_FORMAT(x, y)
#endif

static int num = 0;
static int failed = 0;
static int passed = 0;
static char buffer[512];

int numpassed (void);
int numfailed (void);
int is_make_silent (void);

static inline void pass (void);
static void fail (const char *fmt, ...) ATTRIBUTE_FORMAT (1, 2);
static void ok (const char *fmt, ...) ATTRIBUTE_FORMAT (1, 2);

int
numpassed (void)
{
  return passed;
}
int
numfailed (void)
{
  return failed;
}

static void
ATTRIBUTE_FORMAT (1, 2)
ok (const char *fmt, ...)
{
  va_list ap;

  passed++;
  va_start (ap, fmt);
  vsnprintf (buffer, sizeof (buffer), fmt, ap);
  va_end (ap);
  if (loglevel >= 2)
    printf ("ok %d\t# %s\n", ++num, buffer);
}

static inline void
pass (void)
{
  passed++;
  num++;
}

static void
ATTRIBUTE_FORMAT (1, 2)
fail (const char *fmt, ...)
{
  va_list ap;

  failed++;
  va_start (ap, fmt);
  vsnprintf (buffer, sizeof (buffer), fmt, ap);
  va_end (ap);
  if (loglevel >= 2)
    printf ("not ok %d\t# %s\n", ++num, buffer);
}

#if 0
static void
ATTRIBUTE_FORMAT (1, 2)
todo (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  vsnprintf (buffer, sizeof (buffer), fmt, ap);
  va_end (ap);
  if (loglevel >= 2)
    printf ("not ok %d\t# TODO %s\n", ++num, buffer);
  passed++;
}
#endif

#if defined(BITS_TEST_C) || defined(DECODE_TEST_C)

void bitprepare (Bit_Chain *dat, size_t size);
void bitfree (Bit_Chain *dat);
Bit_Chain strtobt (const char *binarystring);

/*
 * This functions initializes dat and allocates the given
 * size.
 * @param Bit_Chain* dat
 * @param size_t size
 */
void
bitprepare (Bit_Chain *dat, size_t size)
{
  dat->bit = 0;
  dat->byte = 0;
  dat->from_version = dat->version = R_2000;
  dat->opts = 1;
  dat->size = size + 1;
  dat->fh = NULL;
  dat->chain = (unsigned char *)calloc (size + 1, 1);
}

/*
 * This functions resets and free's the dat chain.
 * @param Bit_Chain* dat
 */
void
bitfree (Bit_Chain *dat)
{
  dat->bit = 0;
  dat->byte = 0;
  dat->size = 0;
  if (dat->chain)
    free (dat->chain);
}

/*
 * This function converts the given binary string to binary
 * value
 * for eg "010101" => 010101
 * and puts it in dat to be examined
 */
Bit_Chain
strtobt (const char *binarystring)
{
  Bit_Chain dat;
  int i;
  const int length = strlen (binarystring);
  int size_needed = length / 8;
  if (length % 8)
    size_needed++;

  bitprepare (&dat, size_needed);

  for (i = 0; i < length; ++i)
    {
      if (binarystring[i] == '0')
        bit_write_B (&dat, 0);
      else
        bit_write_B (&dat, 1);
    }

  // LOG_TRACE(bit_print (&dat, size_need));

  // Reset the bit position
  dat.bit = 0;
  dat.byte = 0;

  return dat;
}

#endif

// make -s makes it silent, but can be overidden by VERBOSE=1
int is_make_silent()
{
  const char *make = getenv("MAKEFLAGS");
  if (!make)
    return 0; 			// not from make: verbose
  if (strstr (make, "-s") || memBEGINc (make, "s "))  // make check with -s
    return getenv("VERBOSE") ? 0 : 1;
  else
    return 0; 			// make check without -s
}
