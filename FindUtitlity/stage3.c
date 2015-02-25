#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

void visitDir(char source[]);
int main(int argc, char **argv) {
	struct stat buf;

	unsigned long sourcetype;
	char source[100];
	if (argc > 1) {
		strcpy(source, argv[1]);
	}
	printf("Source argument = %s\n\n", source);
	stat(source, &buf);
	sourcetype = buf.st_mode & 0x4000;
	if (sourcetype) {
		printf("\tDir\t%s\n", source);
		visitDir(source);
	}
	return 0;
}
void visitDir(char *source) {
	DIR *dirp;
	char *prefix;
	int fd;
	struct stat bufsublocal;
	struct dirent *direntries;

	dirp = opendir(source);
	if (!dirp) {
		perror("Error\n");
		return;
	}
	while ((direntries = readdir(dirp)) != NULL) {
		if (!direntries) {
			//printf("There are no more elements in the directory\n");
			break;
		} else {
			if (strcmp(direntries->d_name, ".") == 0
					|| strcmp(direntries->d_name, "..") == 0) {
				continue;
			}

			prefix = malloc(strlen(source) + strlen(direntries->d_name) + 2);
			strcpy(prefix, "");
			strcat(prefix, source);
			strcat(prefix, "/");
			strcat(prefix, direntries->d_name);
			//	printf("%s\n",prefix);
			//snprintf(prefix, sizeof(prefix), "%s/%s", source,
			//		direntries->d_name);

			fd = open(prefix, O_RDONLY);
			if (fd == -1) {
				lstat(prefix, &bufsublocal);
				if (S_ISDIR(bufsublocal.st_mode)) {
					printf("\tDIR\t%s\n", direntries->d_name);
					perror(prefix);
				} else if (S_ISREG(bufsublocal.st_mode)) {
					//	printf("File : %s\t%ld",prefix,subtype);
					printf("\tReg\t%s\n", direntries->d_name);
				} else {
					printf("-\tOther\t%s\n", direntries->d_name);
				}

				continue;
			}
			if(fstat(fd, &bufsublocal))
			{
				perror("FSTAT failure:");
			}

			if (S_ISDIR(bufsublocal.st_mode)) {
				printf("\tDIR\t%s\n", direntries->d_name);
				visitDir(prefix);
				free(prefix);

			} else if (S_ISREG(bufsublocal.st_mode)) {
				lstat(prefix, &bufsublocal);
				//	printf("File : %s\t%ld",prefix,subtype);
				if (S_ISREG(bufsublocal.st_mode)) {
					printf("\tReg\t%s\n", direntries->d_name);
				} else {
					printf("-\tOther\t%s\n", direntries->d_name);
				}

			}

			close(fd);
			free(prefix);

		}

	}
	if (closedir(dirp)) {
		printf("Directory couldn't be closed\n");
	}
}
