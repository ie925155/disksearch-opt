#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *name,
		       int dirinumber, struct direntv6 *dirEnt) {
  //fprintf(stderr, "directory_lookupname(name=%s dirinumber=%d)\n", name, dirinumber);
	struct inode in;
	int success = inode_iget(fs, dirinumber, &in);
	if(success < 0) return -1;
	if( (in.i_mode & IFMT) != IFDIR) return -1;
	int size = inode_getsize(&in);
	int num_of_dirent_per_sector = DISKIMG_SECTOR_SIZE/sizeof(struct direntv6);
	for(int offset = 0; offset < size; offset += DISKIMG_SECTOR_SIZE){
    struct direntv6 dirent[num_of_dirent_per_sector];
    int bno = offset/DISKIMG_SECTOR_SIZE;

    int bytesMoved = file_getblock(fs, dirinumber, bno, dirent);
    if (bytesMoved < 0) return -1;
		struct direntv6 *pdir_entry;
		for(int i = 0; i < num_of_dirent_per_sector; i++){
				pdir_entry = &dirent[i];
				if(strcmp(pdir_entry->d_name, name) == 0){
						memcpy(dirEnt, pdir_entry, sizeof(struct direntv6));
						return 0;
				}
		}
	}
  return -1;
}
