#include <stdio.h>
#include <dirent.h> // DIR
#include <stdlib.h>
#include <ctype.h>  //isdigit
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#define COMM_BUF_SIZE 2048
#define CMD_BUF_SIZE 2048

// open /proc/@pid/cmdline and read value
// open /proc/@pid/exefile and read value
// tbd 중복코드 통일
void iterate_process(int pid, char* cmdline, char* exefile)
{
	printf("wewake interate_process start\n");
	
	char paths[32]={0};
	int fd;
	int i=0;
	int ret;

	//sprintf(paths, "./sample/%d/cmdline", pid);
	sprintf(paths, "/proc/%d/cmdline", pid);
	//printf("wewake paths: %s\n", paths);
	fd = open(paths, O_RDONLY);
	ret = read(fd, cmdline, 32);
	if(ret == 0){
		memset(cmdline,0,sizeof(cmdline));
	}
	printf("read length is %d\n",ret);

	for (i=0; cmdline[i]!=0; i++) {
		if(cmdline[i]=='\n') {
			cmdline[i]=0;
			break;
		}
	}
	close(fd);

	//sprintf(paths, "./sample/%d/exefile", pid);
	sprintf(paths, "/proc/%d/exefile", pid);
	//printf("wewake paths: %s\n", paths);
	fd = open(paths, O_RDONLY);
	ret = read(fd, exefile, CMD_BUF_SIZE);
	if(ret == 0){
		memset(exefile,0,sizeof(exefile));
	}
	printf("read length is %d\n",ret);

	for (i=0; exefile[i]!=0; i++) {
		if(exefile[i]=='\n') {
			exefile[i]=0;
			break;
		}
	}
	close(fd);
}

int main(int argc, char** argv)
{
	struct dirent *dirent;
  	DIR *dir;
  	int r;
  	int pid;
	char cmdline[32];
	char exefile[32];
	char logpath[32]={0,};
	char proc_data[4096]={0,};
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int log_fd;
	int i, data_size;

 	// 1) find all processes
	if (!(dir = opendir ("/proc"))) {
	//if (!(dir = opendir ("./sample"))) {
		printf("wewake couldn't open /proc\n");
   		perror (NULL);
   		exit (EXIT_FAILURE);
 	}

	sprintf(logpath, "./testlog");
	//log_fd = open(logpath, O_CREAT | O_WRONLY, mode);
	log_fd = open(logpath, O_CREAT | O_TRUNC | O_WRONLY, mode);
	if ( 0 > log_fd){
		printf("file open failed.\n");
		perror(NULL);
   		exit (EXIT_FAILURE);
	}

	while (dirent = readdir (dir)) {
  	// 2) we are only interested in process IDs
  		if (isdigit (*dirent -> d_name)) {
    			pid = atoi (dirent -> d_name);
    			iterate_process(pid, cmdline, exefile);
			sprintf(proc_data, "%d,%s,%s\n", pid, cmdline, exefile);
			//printf("proc_data size=%d", sizeof(proc_data));
			for (i=0; proc_data[i]!=0; i++) {
				if(proc_data[i]=='\n') {
					data_size=i+1;
					break;
				}
			}
			write(log_fd, proc_data, data_size);
			printf("proc_data = %s", proc_data);
  		}
	}
	closedir (dir);
	close(log_fd);

	return 0;
}
