#include <stdio.h>     /* standard I/O functions                         */
#include <stdlib.h>    /* exit                                           */
#include <unistd.h>    /* standard unix functions, like getpid()         */
#include <signal.h>    /* signal name macros, and the signal() prototype */

#include <string.h>    /// required by memset

/* first, define the Ctrl-C counter, initialize it with zero. */
int ctrl_c_count = 0;
int got_response = 0;
#define CTRL_C_THRESHOLD  5

/* the Ctrl-C signal handler */
void catch_int(int sig_num)
{
  /* increase count, and check if threshold was reached */
  ctrl_c_count++;
  if (ctrl_c_count >= CTRL_C_THRESHOLD) {
    char answer[30];

    /* prompt the user to tell us if to really
     * exit or not */
    printf("\nReally exit? [Y/n]: ");
    fflush(stdout);

    alarm(5);

    fgets(answer, sizeof(answer), stdin);
    if (answer[0] == 'n' || answer[0] == 'N') {

      got_response = 1;

      printf("\nContinuing\n");
      fflush(stdout);
      /*
       * Reset Ctrl-C counter
       */
      ctrl_c_count = 0;
    }
    else {
      printf("\nExiting...\n");
      fflush(stdout);
      exit(0);
    }
  }
}

/* the Ctrl-Z signal handler */
void catch_tstp(int sig_num)
{
  /* print the current Ctrl-C counter */
  printf("\n\nSo far, '%d' Ctrl-C presses were counted\n\n", ctrl_c_count);
  fflush(stdout);
}

/*
Called if the alarm signal is activated (no input from user)
Prints message and then program will terminate
*/
void catch_alarm(int sig_num)
{
  printf("\nALARM ALARM ALARM");
}

int main(int argc, char* argv[])
{
  struct sigaction sa;
  sigset_t mask_set;  /* used to set a signal masking set. */

  /* setup mask_set */
  memset(&sa, '\0', sizeof(sa));

  // initialize the set to include all signals
  sigfillset(&mask_set);

  // remove SIGALARM from set of included signals
  sigdelset(&mask_set, SIGALRM);

  sa.sa_mask = mask_set;

  /* set signal handlers */

  // SIGINT
  sa.sa_handler = catch_int;
  if(sigaction(SIGINT, &sa, NULL) != 0)
  {
    printf("\n\nError with sigint handler -> ERROR");
    return 1;
  }

  // SIGTSTP
  sa.sa_handler = catch_tstp;
  if(sigaction(SIGTSTP, &sa, NULL) != 0)
  {
    printf("\n\nError with sigftsp handler -> ERROR");
    return 1;
  }

  // SIGALRM
  sa.sa_handler = catch_alarm;
  if(sigaction(SIGALRM, &sa, NULL) != 0)
  {
    printf("\n\nError with sigalarm handler -> ERROR");
    return 1;
  }

  /* Causes calling process to wait until
  any signals are received */
  while(1)
    pause();

  return 0;
}
