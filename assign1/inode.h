#ifndef _INODE_H
#define _INODE_H

#include "unixfilesystem.h"

/**
 * Fetches the specified inode from the filesystem. 
 * Returns 0 on success, -1 on error.  
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp); 

/**
 * Gets the location of the specified file block of the specified inode.
 * Returns the disk block number on success, -1 on error.  
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum);

/**
 * Computes the size of an inode from its size0 and size1 fields.
 */
int inode_getsize(struct inode *inp);

#endif // _INODE_
