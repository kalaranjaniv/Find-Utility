#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void visitDir(char source[]);
int main(int argc, char **argv) {
	struct stat buf;

	unsigned long sourcetype;
	char source[100];
	if (argc > 1) {
		strcpy(source, argv[1]);
	}
	stat(source, &buf);
	sourcetype = buf.st_mode & 0x4000;
	if (sourcetype) {
		printf("Its a directory\n");
		visitDir(source);
	}
	return 0;
}
void visitDir(char *source) {
	DIR *dirp;
	char prefix[100];
	struct stat buflocal;
	struct dirent *direntries;
	time_t tm;
	long ltime = 1;
	time(&ltime);
	ctime(&ltime);

	dirp = opendir(source);
	if (!dirp) {
		perror("Error");
		return;
	}
	while (1) {
		direntries = readdir(dirp);
		if (!direntries) {
			printf("There are no more elements in the directory\n");
			break;
		} else {
			snprintf(prefix, sizeof(prefix), "%s/%s",source,
					direntries->d_name);
			if (!stat(prefix, &buflocal)) {

				tm = buflocal.st_mtime;
				printf("Filename : %s\tModified time : %d secs\n", prefix,
						ltime - tm);
			} else {
				printf("Stats cannot be invoked for %s \n",prefix);
				continue;
			}
		}
	}

	if (closedir(dirp)) {
		printf("Directory couldn't be closed\n");
	}
}
