#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <error.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

char *maildir = "/home/lzap/Mail/INBOX/cur";

int main(int argc,char *argv[]) {
  DIR *dp;
  struct stat statentry;
  struct dirent *dentry;
  time_t last_ctime = 0;
  time_t latest_ctime = 0;

  if (chdir(maildir) == -1)
    error(1, 1, "Can't cd to %s.", maildir);

  for (;;) {

    latest_ctime = 0;
    if ((dp = opendir(maildir)) == NULL)
      error(2, 2, "Can't open directory %s", maildir);

    while ((dentry = readdir(dp)) != NULL) {
      if (!strcmp(dentry->d_name,".") || !strcmp(dentry->d_name,"..") || dentry->d_type != DT_REG)
        continue;

      if (stat(dentry->d_name, &statentry) == -1)
        error(0, 0, "Unable to stat %s", dentry->d_name);

      // determine latest ctime of the run
      if (statentry.st_ctime > latest_ctime)
        latest_ctime = statentry.st_ctime;
    }

    if (last_ctime == 0) {
      last_ctime = latest_ctime;
    } else {
      if (latest_ctime > last_ctime) {
        last_ctime = latest_ctime;
        printf("New mail\n");
        if (system("/usr/bin/notify-send 'New mail'") != 0)
          error(0, 0, "Can't execute notify-send");
      }
    }
    sleep(100);

    if (closedir(dp) != 0)
      error(0, 0, "Can't close directory %s", maildir);

  }
}
