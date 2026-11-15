#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int pid = getpid();

  printf(1, "My PID = %d\n", pid);

  printf(1, "Initial nice = %d\n", getnice(pid));

  printf(1, "setnice return = %d\n", setnice(pid, 10));

  printf(1, "New nice = %d\n", getnice(pid));

  printf(1, "\nAll processes:\n");
  ps(0);

  printf(1, "\nMy process:\n");
  ps(pid);

  printf(1, "\nInvalid PID:\n");
  printf(1, "getnice(999) = %d\n", getnice(999));

  printf(1, "\nInvalid nice values:\n");
  printf(1, "setnice(pid, -1) = %d\n", setnice(pid, -1));
  printf(1, "setnice(pid, 40) = %d\n", setnice(pid, 40));
  printf(1, "\nps invalid PID:\n");
  ps(999);
  printf(1, "ps(999) finished\n");
  exit();
}
