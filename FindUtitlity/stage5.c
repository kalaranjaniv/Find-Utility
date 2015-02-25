#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pwd.h>

int selectionattr;
int emptyslot;
char *arglist;
char **cmd_list;
int commandlistcount;
const char *selTable[] = { "-name", "-mtime", "-user", NULL };
FILE *fd1;

char* ProcArg(int argc, char **argv);
void visitDir(char source[]);
int main(int argc, char **argv) {
	struct stat buf;
	fd1 = fopen("Shell_commands.sh", "w");
	unsigned long sourcetype;
	char *source;
	source = ProcArg(argc, argv);
	//printf("Source argument = %s\n\n", source);
	stat(source, &buf);
	sourcetype = buf.st_mode & 0x4000;
	if (sourcetype) {
		//	printf("\tDir\t%s\n", source);
		visitDir(source);
	} else {
		printf("The source argument is not a directory");
	}
	fclose(fd1);
	system("Shell_commands.sh");
	return 0;
}
void visitDir(char *source) {
	unsigned long int secondsdetails;
	struct passwd *pwd;
	int subi = 0;
	char *strbuffer;
	uid_t userid;
	DIR *dirp;
	char *prefix;
	int fd;
	struct stat bufsublocal;
	struct dirent *direntries;
	dirp = opendir(source);

	if (!dirp) {
		perror("Error");
		return;
	}

	switch (selectionattr) {
	case 0:
		while ((direntries = readdir(dirp)) != NULL) {
			if (!direntries) {
				//printf("There are no more elements in the directory\n");
				break;
			} else {
				if (strcmp(direntries->d_name, ".") == 0
						|| strcmp(direntries->d_name, "..") == 0) {
					continue;
				}

				prefix = malloc(
						strlen(source) + strlen(direntries->d_name) + 2);
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
					if (strcasecmp(direntries->d_name, arglist) == 0) {
						if (S_ISDIR(bufsublocal.st_mode)) {
							printf("\tDIR\t%s\n", prefix);
							perror(prefix);
						} else if (S_ISREG(bufsublocal.st_mode)) {
							//	printf("File : %s\t%ld",prefix,subtype);
							printf("\tReg\t%s\n", prefix);
						} else {
							printf("-\tOther\t%s\n", prefix);
						}
					}
					continue;
				}
				if (fstat(fd, &bufsublocal)) {
					perror("FSTAT failure:");
				}

				if (S_ISDIR(bufsublocal.st_mode)) {
					if (strcasecmp(direntries->d_name, arglist) == 0) {
						printf("\tDIR\t%s\n", prefix);
						cmd_list[emptyslot] = prefix;
						strbuffer = (char*) malloc((sizeof(char) * (50*commandlistcount)));
						for (subi = 0; subi < commandlistcount; subi++) {
							sprintf(strbuffer,"%s %s ",strbuffer,cmd_list[subi]);

						}
						fprintf(fd1,"%s\n",strbuffer);
						free(strbuffer);
						//printf("%s: strbuffer",strbuffer);

					}
					visitDir(prefix);
					free(prefix);

				} else if (S_ISREG(bufsublocal.st_mode)) {
					lstat(prefix, &bufsublocal);
					//	printf("File : %s\t%ld",prefix,subtype);
					if (S_ISREG(bufsublocal.st_mode)) {
						if (strcasecmp(direntries->d_name, arglist) == 0) {
							printf("\tReg\t%s\n", prefix);
							cmd_list[emptyslot] = prefix;
							strbuffer = (char*) malloc((sizeof(char) * (50*commandlistcount)));
							for (subi = 0; subi < commandlistcount; subi++) {
								sprintf(strbuffer,"%s %s ",strbuffer,cmd_list[subi]);
							}
							fprintf(fd1,"%s\n",strbuffer);
							free(strbuffer);

						}
					} else {
						if (strcasecmp(direntries->d_name, arglist) == 0) {
							printf("-\tOther\t%s\n", prefix);
							cmd_list[emptyslot] = prefix;
							strbuffer = (char*) malloc((sizeof(char) * (50*commandlistcount)));
							for (subi = 0; subi < commandlistcount; subi++) {
								sprintf(strbuffer,"%s %s ",strbuffer,cmd_list[subi]);
							}
							fprintf(fd1,"%s\n",strbuffer);
							free(strbuffer);

						}
					}

				}

				close(fd);
				free(prefix);

			}

		}
		break;
	case 1:
		secondsdetails = atoi(arglist) * 24 * 60 * 60;
		time_t tm;
		long ltime = 1;
		time(&ltime);
		ctime(&ltime);
		while ((direntries = readdir(dirp)) != NULL) {
			if (!direntries) {
				//printf("There are no more elements in the directory\n");
				break;
			} else {
				if (strcmp(direntries->d_name, ".") == 0
						|| strcmp(direntries->d_name, "..") == 0) {
					continue;
				}

				prefix = malloc(
						strlen(source) + strlen(direntries->d_name) + 2);
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
					tm = bufsublocal.st_mtime;
					if ((ltime - tm) <= secondsdetails) {
						if (S_ISDIR(bufsublocal.st_mode)) {
							printf("\tDIR\t%s\n", prefix);
							perror(prefix);
						} else if (S_ISREG(bufsublocal.st_mode)) {
							//	printf("File : %s\t%ld",prefix,subtype);
							printf("\tReg\t%s\n", prefix);
						} else {
							printf("-\tOther\t%s\n", prefix);
						}
					}
					continue;
				}
				if (fstat(fd, &bufsublocal)) {
					perror("FSTAT failure:");
				}

				if (S_ISDIR(bufsublocal.st_mode)) {
					tm = bufsublocal.st_mtime;
					if ((ltime - tm) <= secondsdetails) {
						printf("\tDIR\t%s\n", prefix);
						cmd_list[emptyslot] = prefix;
						strbuffer = (char*) malloc((sizeof(char) * (50*commandlistcount)));
						for (subi = 0; subi < commandlistcount; subi++) {
							sprintf(strbuffer,"%s %s ",strbuffer,cmd_list[subi]);
						}
						fprintf(fd1,"%s\n",strbuffer);
						free(strbuffer);

					}
					visitDir(prefix);
					free(prefix);

				} else if (S_ISREG(bufsublocal.st_mode)) {
					lstat(prefix, &bufsublocal);
					tm = bufsublocal.st_mtime;
					//	printf("File : %s\t%ld",prefix,subtype);
					if (S_ISREG(bufsublocal.st_mode)) {
						if ((ltime - tm) <= secondsdetails) {
							printf("\tReg\t%s\n", prefix);
							cmd_list[emptyslot] = prefix;
							strbuffer = (char*) malloc((sizeof(char) * (50*commandlistcount)));
							for (subi = 0; subi < commandlistcount; subi++) {
								sprintf(strbuffer,"%s %s ",strbuffer,cmd_list[subi]);
							}
							fprintf(fd1,"%s\n",strbuffer);
							free(strbuffer);

						}
					} else {
						if ((ltime - tm) <= secondsdetails) {
							printf("-\tOther\t%s\n", prefix);
							cmd_list[emptyslot] = prefix;
							strbuffer = (char*) malloc((sizeof(char) * (50*commandlistcount)));
							for (subi = 0; subi < commandlistcount; subi++) {
								sprintf(strbuffer,"%s %s ",strbuffer,cmd_list[subi]);
							}
							fprintf(fd1,"%s\n",strbuffer);
							free(strbuffer);

						}
					}

				}

				close(fd);
				free(prefix);

			}

		}
		break;
	case 2:
		pwd = getpwnam(arglist);
		if (pwd) {
			userid = pwd->pw_uid;
		} else {
			printf("Invalid userid\n");
			exit(0);
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

				prefix = malloc(
						strlen(source) + strlen(direntries->d_name) + 2);
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

					if (userid == bufsublocal.st_uid) {
						if (S_ISDIR(bufsublocal.st_mode)) {
							printf("\tDIR\t%s\n", prefix);
							perror(prefix);
						} else if (S_ISREG(bufsublocal.st_mode)) {
							//	printf("File : %s\t%ld",prefix,subtype);
							printf("\tReg\t%s\n", prefix);
						} else {
							printf("-\tOther\t%s\n", prefix);
						}
					}
					continue;
				}
				if (fstat(fd, &bufsublocal)) {
					perror("FSTAT failure:");
				}

				if (S_ISDIR(bufsublocal.st_mode)) {

					if (userid == bufsublocal.st_uid) {
						printf("\tDIR\t%s\n", prefix);
						cmd_list[emptyslot] = prefix;
						strbuffer = (char*) malloc((sizeof(char) * (50*commandlistcount)));
						for (subi = 0; subi < commandlistcount; subi++) {
							sprintf(strbuffer,"%s %s ",strbuffer,cmd_list[subi]);
						}
						fprintf(fd1,"%s\n",strbuffer);
						free(strbuffer);

					}
					visitDir(prefix);
					free(prefix);

				} else if (S_ISREG(bufsublocal.st_mode)) {
					lstat(prefix, &bufsublocal);

					//	printf("File : %s\t%ld",prefix,subtype);
					if (S_ISREG(bufsublocal.st_mode)) {
						if (userid == bufsublocal.st_uid) {
							printf("\tReg\t%s\n", prefix);
							cmd_list[emptyslot] = prefix;
							strbuffer = (char*) malloc((sizeof(char) * (50*commandlistcount)));
							for (subi = 0; subi < commandlistcount; subi++) {
								sprintf(strbuffer,"%s %s ",strbuffer,cmd_list[subi]);
							}
							fprintf(fd1,"%s\n",strbuffer);
							free(strbuffer);

						}
					} else {
						if (userid == bufsublocal.st_uid) {
							printf("-\tOther\t%s\n", prefix);
							cmd_list[emptyslot] = prefix;
							strbuffer = (char*) malloc((sizeof(char) * (50*commandlistcount)));
							for (subi = 0; subi < commandlistcount; subi++) {
								sprintf(strbuffer,"%s %s ",strbuffer,cmd_list[subi]);
							}
							fprintf(fd1,"%s\n",strbuffer);
							free(strbuffer);

						}
					}

				}

				close(fd);
				free(prefix);

			}

		}
		break;

	}
	if (closedir(dirp)) {
		printf("Directory couldn't be closed\n");
	}
}

char* ProcArg(int argc, char **argv) {
	char selection[10];
	char *args;
	char *sourcearg;
	int flag = 0;
	int i = 3, j = 0, k = 0;
	if (argc > 4) {
		sourcearg = malloc(100 * sizeof(char));
		strcpy(sourcearg, argv[1]);
		printf("Source=%s\n", sourcearg);
		strcpy(selection, argv[2]);
		printf("Selection=%s\n", selection);
		args = argv[3];
		printf("Argument=%s\n", args);
		printf("\n");
		for (i = 0; i < 3; i++) {
			if (strcmp(selection, selTable[i]) == 0) {
				selectionattr = i;
				arglist = malloc(sizeof(args));
				strcpy(arglist, args);
				flag = 1;
				break;
			}

		}
		if (flag == 0) {
			printf("Selection criteria is not available\n");
			exit(0);
		}

		cmd_list = (char **) malloc(sizeof(char *) * (argc));
		if (argv[4][0] == '-') {
			cmd_list[0] = argv[4] + 1;
			printf("Command Arguument : %s\n", cmd_list[0]);
		}
		i = 5;
		j = 1;
		if (argc > 5) {
			while (i<argc && argv[i][0] == '-') {
				cmd_list[j] = argv[i];
				i++;
				j++;
			}
			emptyslot = j;
			cmd_list[j] = "";
			j++;
			while (i < argc) {
				cmd_list[j] = argv[i];
				j++;
				i++;
			}
		}
		else if(argc ==5)
		{
			emptyslot = j;
			cmd_list[j] = "";
			j++;
		}
		commandlistcount = j;
		for (k = 0; k < j; k++) {
			printf("cmd_list[%d]=%s\t", k, cmd_list[k]);
		}
		printf("\n");
		return sourcearg;
	} else {
		printf(
				"Usage. find <sourcedir> <selection> <arguments> <command-name>\n");
		exit(0);
	}

}
