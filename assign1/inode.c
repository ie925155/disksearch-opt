#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "inode.h"
#include "diskimg.h"
#include "ino.h"

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
  int inumber_index = inumber - 1;
  int fd = fs->dfd;
  int inode_per_sector = DISKIMG_SECTOR_SIZE/sizeof(struct inode);
  int sector_offset = inumber_index / inode_per_sector;
  int sector_index = INODE_START_SECTOR + sector_offset;
  /*fprintf(stdout, "nodePerSector = %u sector_index = %d\n", inode_per_sector,
    sector_index);*/
  struct inode inode_table[inode_per_sector];
  int size = diskimg_readsector(fd, sector_index, inode_table);
  if(size == -1) return -1;
  int inode_position = inumber_index % inode_per_sector;
  memcpy(inp, &inode_table[inode_position], sizeof(struct inode));
  return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
  int isLargeFile = ( (inp->i_mode & ILARG) != 0 );
  //fprintf(stderr, "isLargeFile=%d\n", isLargeFile);
  if(isLargeFile) //indirect mode
  {
    int offset = blockNum / (DISKIMG_SECTOR_SIZE / sizeof(uint16_t));
    //fprintf(stderr, "offset=%d sector_index=%d\n", offset, blockNum % 256);
    if(offset < 7) //singe indirect sector
    {
      int num_of_sector_index = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);
      int single_indirect_index = inp->i_addr[offset];
      uint16_t sector_index[num_of_sector_index];
      int success = diskimg_readsector(fs->dfd, single_indirect_index, sector_index);
      if(success == -1) return -1;
      return sector_index[blockNum % num_of_sector_index];
    }
    else // doubly indirect sector
    {
      int doubly_indirect_index = inp->i_addr[7];
      int num_of_single_sector_index = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);
      int num_of_sector_index = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);
      uint16_t single_indirect_index[num_of_single_sector_index];
      int success = diskimg_readsector(fs->dfd, doubly_indirect_index, single_indirect_index);
      if(success == -1) return -1;
      uint16_t sector_index[num_of_sector_index];
      int single_indirect_offset = offset - 7;
      success = diskimg_readsector(fs->dfd, single_indirect_index[single_indirect_offset], sector_index);
      if(success == -1) return -1;
      return sector_index[blockNum % num_of_sector_index];
    }
  }
  else
    return inp->i_addr[blockNum];
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1);
}
