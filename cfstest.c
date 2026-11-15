#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int p1, p2;
  volatile int counter;

  p1 = fork();

  if(p1 < 0){
    printf(1, "fork failed\n");
    exit();
  }

  if(p1 == 0){
    counter = 0;
    while(1)
      counter++;
  }

  p2 = fork();

  if(p2 < 0){
    kill(p1);
    wait();
    exit();
  }

  if(p2 == 0){
    counter = 0;
    while(1)
      counter++;
  }

  // Assign different priorities.
  printf(1, "setnice p1: %d\n", setnice(p1, 10));
  printf(1, "setnice p2: %d\n", setnice(p2, 15));

  // Let both CPU-bound processes run.
  sleep(100);

  // Compare their scheduling statistics.
  printf(1, "\nCFS test results:\n");
  ps(0);

  // Clean up both children.
  kill(p1);
  kill(p2);
  wait();
  wait();

  exit();
}
