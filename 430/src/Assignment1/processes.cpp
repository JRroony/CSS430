#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//   ps -A | grep argv[1] | wc -l
int main(int argc, char *argv[]) {
  int pfd1[2], pfd2[2], status;
  pid_t pid;

  if (argc != 2) {
    perror("Not enough arguments");
    exit(EXIT_FAILURE);
  }

  if (pipe(pfd1) == -1)                   // Creates first pipe
    perror("Failed to create first pipe");

  if ((pid = fork()) < 0)                 // Creates fork
    perror("Failed to fork first time");

  if (pid == 0) {
    dup2(pfd1[1], 1);
    close(pfd1[0]);                       // Close read end
    close(pfd1[1]);                       // Close write end
    execlp("ps", "ps", "-A", (char *)0);
    perror("Failed to exec 'ps'");
  }

  if (pipe(pfd2) == -1)                   // Creates second pipe
    perror("failed to create second pipe");

  if ((pid = fork()) < 0)                 // Creates fork
    perror("Failed to fork second time");

  if (pid == 0) {                          // Child reads from pipe
    dup2(pfd1[0], 0);                      // int dup2( int oldfd, int newfd ); creates in newfd a copy of the file descriptor oldfd.
    close(pfd1[0]);                        // Close read end
    close(pfd1[1]);                        // Close write end
    dup2(pfd2[1], 1);                      // int dup2( int oldfd, int newfd ); creates in newfd a copy of the file descriptor oldfd.
    close(pfd2[0]);                        // Close read end
    close(pfd2[1]);                        // Close write end
    execlp("grep", "grep", argv[1], (char *)0);
    perror("Failed to exec 'grep'");
  } else {
    close(pfd1[0]);                        // Close read end
    close(pfd1[1]);                        // Close write end
    dup2(pfd2[0], 0);                      // int dup2( int oldfd, int newfd ); creates in newfd a copy of the file descriptor oldfd.
    close(pfd2[0]);                        // Close read end
    close(pfd2[1]);                        // Close write end
    execlp("wc", "wc", "-l", (char *)0);
    perror("Failed to exec 'wc'");
  }
  wait(&status);
  return 0;
}
