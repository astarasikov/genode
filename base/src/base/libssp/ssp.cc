#include <base/printf.h>
#include <base/env.h>
#include <base/sleep.h>

void *__stack_chk_guard = 0;

static void __attribute__ ((constructor))
__guard_setup (void)
{
  unsigned char *p;
  int fd;

  if (__stack_chk_guard != 0)
    return;
#if 0
  fd = open ("/dev/urandom", 0);
  if (fd != -1)
    {
      int size = read (fd, &__stack_chk_guard,
                           sizeof (__stack_chk_guard));
      close (fd);
      if (size == sizeof(__stack_chk_guard) && __stack_chk_guard != 0)
        return;
    }
#endif
  /* If a random generator can't be used, the protector switches the guard
     to the "terminator canary".  */
  p = (unsigned char *) &__stack_chk_guard;
  p[sizeof(__stack_chk_guard)-1] = 255;
  p[sizeof(__stack_chk_guard)-2] = '\n';
  p[0] = 0;
}

void
__stack_chk_fail (void)
{
  const char *msg = "*** stack smashing detected ***: ";
    PERR("%s\t%s\n",msg,"stack smashing detected: terminated");
    Genode::sleep_forever();
}

void
__chk_fail (void)
{
  const char *msg = "*** buffer overflow detected ***: ";
    PERR("%s\t%s\n",msg,"buffer overflow detected: terminated");
    Genode::sleep_forever();
}

extern "C" __attribute__((weak)) void __stack_chk_fail_local(void)
{
  __stack_chk_fail ();
}
