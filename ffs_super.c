#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef DISK_DRIVER_H
#include "disk_driver.h"
extern struct disk_operations disk_ops;
#endif

#ifndef FFS_SUPER_H
#include "ffs_super.h"
#endif

/* #ifndef FFS_INODE_H
#include "ffs_inode.h"
#endif */


static int super_read(struct super *sb) {

 	int ercode;
  	union sb_block sb_u;

  	// Reading the disk block that contains the superblock
  	ercode = disk_ops.read(SB_OFFSET ,sb_u.data);

  	if (ercode < 0) 
		return ercode; 
	
	// Copying information from sb_u.sb to sb
	memcpy(sb, &sb_u.sb, sizeof(struct super));	

  	return 0;
}


/* Helper functions */

void super_print(const struct super *sb) {

 	printf("Superblock:\n");
 	 
	// Checking if the disk can be used
	if(sb->fsmagic != FS_MAGIC)  
		printf("invalid\n");
	else 
		printf("valid\n");

	printf("%d\n", sb->nblocks);
	printf("%d\n", sb->nbmapblocksinodes);
	printf("%d\n", sb->ninodeblocks);
	printf("%d\n", sb->ninodes);
	printf("%d\n", sb->nbmapblocksdata);
	printf("%d\n", sb->ndatablocks);
	printf("%d\n", sb->startInArea);
	printf("%d\n", sb->startDtBmap);
	printf("%d\n", sb->startDtArea);

	// Making the adequate changes for Mooshak delivery
	if(sb->mounted == 0)
		printf("no\n");
	else 
		printf("yes\n");
}

struct super_operations super_ops= {

	.read= &super_read,
	.print= &super_print

};
