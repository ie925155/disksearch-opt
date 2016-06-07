
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int plain_name(const char *pathname);
static char* rest(const char *pathname);
static char* first(const char *pathname);
static int path_to_inode_number(struct unixfilesystem *fs, const char* pathname, int dir);

static int plain_name(const char *pathname)
{
  return (strchr(pathname, '/') == NULL);
}

static char* rest(const char *pathname)
{
  char *first = strchr(pathname, '/');
  char *rest = strchr(first+1, '/');
  return (rest == NULL) ? NULL : rest;
}

static char* first(const char *pathname)
{
  char *path = strchr(pathname, '/') + 1;
  char *next_slash = strchr(path, '/');
  size_t dir_len = (next_slash == NULL) ? strlen(path) : (size_t)(next_slash - path);
  char *dir = (char*) malloc(dir_len+1);
  memcpy(dir, path, dir_len);
  dir[dir_len] = '\0';
  return dir;
}

static int path_to_inode_number(struct unixfilesystem *fs, const char* pathname, int dir)
{
  struct direntv6 dirEnt;
  if(plain_name(pathname))
  {
    int ret = directory_findname(fs, pathname, dir, &dirEnt);
    if(ret < 0) return -1;
    return dirEnt.d_inumber;
  }
  else
  {
    char *dir_name = first(pathname);
    int ret = directory_findname(fs, dir_name, dir, &dirEnt);
    if(ret < 0) return -1;
    free(dir_name);
    const char *rest_path = rest(pathname);
    if(rest_path == NULL || strlen(rest_path) == 0) return dirEnt.d_inumber;
    return path_to_inode_number(fs, rest_path, dirEnt.d_inumber);
  }
}

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
  if(strcmp(pathname, "/") == 0) return ROOT_INUMBER;
  return path_to_inode_number(fs, pathname, ROOT_INUMBER);
}
