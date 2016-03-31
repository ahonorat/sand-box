#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void exec_gdb()
{
    // Create child process for running GDB debugger
    int pid = fork();
    
    if (pid < 0) /* error */
    {
        abort();
    }
    else if (pid) /* parent */
    {
       sleep(10); /* Give GDB time to attach */
    }
    else /* child */
    {

      char *ppidString = NULL;
      int  ppidStringLen = 0;
      for(;;) {
	// this loop is executed only two times
	ppidStringLen = 1 + snprintf(ppidString, ppidStringLen, "%i", getppid());
	if (ppidString != NULL) break;                  // success
	ppidString = realloc(ppidString, ppidStringLen);
	if (ppidString == NULL) break;                  // fail
      }      
      
      execl("/usr/bin/sudo", "/usr/bin/gdb", "/usr/bin/gdb", "-p", ppidString, (char *) NULL);

      printf("GDB launch failure (err: %i). Parent PID was: %s.", errno, ppidString);
      free(ppidString);
      abort();
    }
}
