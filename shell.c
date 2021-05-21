#include <stdio.h>
#include <string.h>                  // 20171331 이현창
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#define MAX_LEN_LINE    100
#define LEN_HOSTNAME    30
int main(void)
{
    char command[MAX_LEN_LINE];
    char *args[] = {command,NULL,NULL};
    char cwd[MAX_LEN_LINE];
    int ret, status;
    pid_t pid, cpid;
    
    char hostname[LEN_HOSTNAME + 1];
    memset(hostname, 0x00, sizeof(hostname));    
    gethostname(hostname, LEN_HOSTNAME);

    getcwd(cwd,sizeof(cwd));                   // get cwd (current working directory)

    
    while (true) {
        char *s;
	char *option;
        int len;

	printf("%c[93mMyShell@", 27);     // color change light yellow
        printf("%c[92m%s@%s@", 27, getpwuid(getuid())->pw_name,hostname);  /* color change light green
									     add username & hostname */
	printf("%c[33m%s $ ", 27,cwd);    //color change yellow, add cwd
	printf("%c[0m", 27);              // color reset
	
	s = fgets(command, MAX_LEN_LINE, stdin);

	if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }
        
        len = strlen(command);
        
	s = strtok(command," ");         // get command
	option = strtok(NULL,"\n");      // get supporting command-line arguments

        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }
	
	args[0] = command;             // initialize args[0] to input command

	if (!strcmp(command,"ls")){    // 'ls' is same the '/bin/ls'
		args[0] = "/bin/ls";
	}
	
	if(!strcmp(command,"hostname")){          // print hostname
		 args[0] = "/bin/hostname";
	}
	
	if(!strcmp(command,"username")){          // print username
		printf("username: %s\n", getpwuid(getuid())->pw_name);
		continue;
	}
	
	if(option != NULL){              // input option(supporting command-line arguments) in args[1]
		args[1] = option;
	}
	else{                            
		args[1] = '\0';          // initialize args[1] to NULL
	}
	
	if(!strcmp(command,"exit")){      // update: add exit command
		printf("exit!!!\n");
		return false;
	}
	printf("%d\n", len);
        printf("[%s]\n", command);
      
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } 
        if (pid != 0) {  /* parent */
            cpid = waitpid(pid, &status, 0);
            if (cpid != pid) {
                fprintf(stderr, "waitpid failed\n");        
            }
            printf("Child process terminated\n");
            if (WIFEXITED(status)) {
                printf("Exit status is %d\n", WEXITSTATUS(status)); 
            }
        }
        else {  /* child */
            ret = execve(args[0], args, NULL);
            if (ret < 0) {
                fprintf(stderr, "execve failed\n");   
                return 1;
            }
        }
    }
    return 0;
}
