#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <error.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

char *path = NULL;
int sleep_delay = 100;

void usage(int exit_status) {
  fputs("usage: dunst-checkmail [-h/--help] [-v] [-p PATH] [-s DELAY]\n", stderr);
  exit(exit_status);
}

void print_version(void) {
  printf("dunst-checkmail %s\n", VERSION);
  exit(EXIT_SUCCESS);
}

void parse_cmdline(int argc, char *argv[]) {
  int c;
  while (1) {
    static struct option long_options[] = {
      {"help", no_argument, NULL, 'h'},
      {"path", required_argument, NULL, 'p'},
      {"sleep", required_argument, NULL, 's'},
      {"version", no_argument, NULL, 'v'},
      {0, 0, 0, 0}
    };

    int option_index = 0;

    c = getopt_long_only(argc, argv, "hv", long_options, &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {
      case 0:
        break;
      case 'h':
        usage(EXIT_SUCCESS);
        break;
      case 'p':
        path = optarg;
        break;
      case 's':
        sleep_delay = atoi(optarg);
        break;
      case 'v':
        print_version();
        break;
      default:
        usage(EXIT_FAILURE);
        break;
    }
  }
}

void watch_mbox(char *file) {
  struct stat statentry;
  time_t last_ctime = 0;
  time_t latest_ctime = 0;

  for (;;) {
    latest_ctime = 0;

    if (stat(file, &statentry) == -1)
      error(EXIT_FAILURE, EXIT_FAILURE, "Unable to stat %s", file);

    if (statentry.st_mtime > latest_ctime)
      latest_ctime = statentry.st_mtime;

    if (last_ctime == 0) {
      last_ctime = latest_ctime;
    } else {
      if (latest_ctime > last_ctime) {
        last_ctime = latest_ctime;
        if (system("/usr/bin/notify-send 'New mail'") != 0)
          error(EXIT_FAILURE, EXIT_FAILURE, "Can't execute notify-send");
      }
    }
    sleep(sleep_delay);
  }
}

void watch_maildir(char *maildir) {
  DIR *dp;
  struct stat statentry;
  struct dirent *dentry;
  time_t last_ctime = 0;
  time_t latest_ctime = 0;

  if (chdir(maildir) == -1)
    error(1, 1, "Cannot cd to %s", maildir);

  for (;;) {

    latest_ctime = 0;
    if ((dp = opendir(maildir)) == NULL)
      error(2, 2, "Can't open directory %s", maildir);

    while ((dentry = readdir(dp)) != NULL) {
      if (!strcmp(dentry->d_name,".") || !strcmp(dentry->d_name,"..") || dentry->d_type != DT_REG)
        continue;

      if (stat(dentry->d_name, &statentry) == -1)
        error(EXIT_FAILURE, EXIT_FAILURE, "Unable to stat %s", dentry->d_name);

      if (statentry.st_mtime > latest_ctime)
        latest_ctime = statentry.st_mtime;
    }

    if (last_ctime == 0) {
      last_ctime = latest_ctime;
    } else {
      if (latest_ctime > last_ctime) {
        last_ctime = latest_ctime;
        if (system("/usr/bin/notify-send 'New mail'") != 0)
          error(EXIT_FAILURE, EXIT_FAILURE, "Can't execute notify-send");
      }
    }
    sleep(sleep_delay);

    if (closedir(dp) != 0)
      error(EXIT_FAILURE, EXIT_FAILURE, "Can't close directory %s", maildir);
  }
}

int main(int argc,char *argv[]) {
  struct stat statentry;

  parse_cmdline(argc, argv);

  if (path == NULL)
    error(1, 1, "Provide path to maildir/mbox with -p");

  if (stat(path, &statentry) == -1)
    error(EXIT_FAILURE, EXIT_FAILURE, "Unable to stat %s", path);

  if (S_ISDIR(statentry.st_mode)) {
    watch_maildir(path);
  } else if (S_ISREG(statentry.st_mode)) {
    watch_mbox(path);
  } else {
    error(EXIT_FAILURE, EXIT_FAILURE, "Please provide directory or regular file: %s", path);
  }

}
