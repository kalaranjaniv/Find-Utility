/* This is a test program to show
how to capture info from the inode of a file directly from the
structure.  It also shows use of two other system calls, link()
and unlink().  */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/vnode.h>
#include <sys/fs/ufs_fs.h>
#include <sys/fs/ufs_inode.h>
#include <time.h>


main(argc, argv)
int argc; 
char *argv[];
{
  char cmd[80];
  long ltime=1;
  char timbuf[80];
  struct stat buf;
  int fd;
  mode_t s_m;
  time_t tm;
  time_t cm;
  struct tm lt;
  unsigned long ftype;

	if (argc != 2) {printf ("usage: UID filename \n"); exit(0);}

/* "ls -i file" prints out the inode of a file */
	sprintf(cmd, "ls -i %s", argv[1]);
	printf ("cmd=  %s\n", cmd);
	system (cmd);  

/* Print current time */
	time(&ltime);
	printf("%22.20s",       ctime(&ltime));
	printf("current time %ld\n",ltime);

	
/* Open the file to test if the file indeed exists before
reading its "stat" structure. */
/* To see what are the fields in the inode, see the file 
/usr/include/sys/fs/ufs_inode.h */

/* Demo of lstat() funtcion */
	lstat(argv[1], &buf);
	ftype = buf.st_mode & 0xF000;
	printf("ftype=%x\n", ftype);
	if (ftype == S_IFLNK) printf ("link file\n");

/* Demo of fstat() */
	if ((fd = open (argv[1], O_RDWR)) == -1)
	{
	printf ("Given file does not exists \n");
	exit (-1);
	}
	fstat(fd, &buf);
	tm = buf.st_atime; //time of last access
	printf("diff = %ld\n", ltime - tm);
	ftype = buf.st_mode & 0xF000;
	if (ftype == S_IFREG) printf ("reg file\n");
	else printf ("not reg file\n");
	printf("ftype=%x\n", ftype);

	printf (" inode num=%d \n", buf.st_ino);
	printf("File size = %d \n",buf.st_size);

	/* Testing permission bits */
	s_m = buf.st_mode & S_IEXEC;
	if (s_m) printf ("exec permission is granted to the owner \n");
	else printf("Not granted \n");

	/* testing Linking */
	printf("Number of links to the given file = %d \n", buf.st_nlink);

	/* Create a new link. To see manpage of link(),
	man -s 2 link, because link() belongs to section 2. */
	
	link(argv[1], "new_link");
	printf("Number of links after link() call = %d \n", buf.st_nlink);
	sleep(30);
	/* examine from another window if "new_link" file exists now */
	/* Notice the count in column 2 of the output of ls -i
	of directory where you run this program from.*/

	unlink("new_link");
	printf("Number of links after unlink() = %d \n", buf.st_nlink);
	close (fd);
	//printf("Buffer after closing %d\n",buf.st_nlink);
	/* the close() updates the link information */
	if ((fd = open (argv[1], O_RDWR)) == -1)
	{
	printf ("Given file does not exists \n");
	exit (-1);
	}
	fstat(fd, &buf);
	printf("Number of links after closing the given file = %d \n", buf.st_nlink);
	close(fd);

	if ((fd = open ("test_stat.c", O_RDWR)) == -1)
		{
		printf ("Given file does not exists \n");
		exit (-1);
		}
	fstat(fd, &buf);
	tm =buf.st_mtime;
	printf("Modified time of test_stat.c: %d\n",ltime-tm);

	close(fd);

}
