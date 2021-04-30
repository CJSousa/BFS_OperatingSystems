#include <stdio.h>
#include <string.h>

#ifndef DISK_DRIVER_H
#include "disk_driver.h"
extern struct disk_operations disk_ops;
#endif

#ifndef FFS_SUPER_H
#include "ffs_super.h"
extern struct super_operations super_ops;
#endif


#ifndef FFS_BYTEMAP_H
#include "ffs_bytemap.h"
extern struct bytemap_operations bmap_ops;
#endif


#ifndef FFS_INODE_H
#include "ffs_inode.h"
extern struct inode_operations inode_ops;
#endif



int main( int argc, char *argv[]) {

  	int ercode, sumAreas;

  	struct super sb;
  	struct bytemap bmapINODES, bmapDATA;
	struct inode in;
	
	// Opening the file selected in the terminal
  	ercode = disk_ops.open(argv[1], 0);

  	if (ercode < 0) return ercode;

  	// Reading the superblock
	ercode = super_ops.read(&sb);

	if (ercode < 0) return ercode;

	// Checking if ejection was successful, that is, information was safely kept
	if(sb.mounted){
		printf("Unsucessful ejection\n");
		return 0;
	}

    // Checking consistency of the magic number 
    if(sb.fsmagic != FS_MAGIC) {
        printf("Wrong magic number\n");
        return 0;
    }

    // Checking the consistency regarding the size of the disk
    if(disk_ops.stat() != sb.nblocks) {
        printf("Wrong disk size\n");
        return 0;
    }

    // Superblock size + directory size + number of "user-available" data blocks + number of blocks to store inodes +
    // number of blocks to store the bytemap for the inodes +  number blocks to store the bytemap for data blocks
    sumAreas = 1 + 1 + sb.ndatablocks + sb.ninodeblocks + sb.nbmapblocksinodes + sb.nbmapblocksdata;

    // Checking the consistency regarding the sum of the sizes of the disk area
    if(disk_ops.stat() != sumAreas) {
        printf("Sum of areas differs from expected\n");
        return 0;
    }

    // Checking whether there are entries in the bytemap of inodes that contradict the isValid
	
	// Reading the bytemap
	ercode = bmap_ops.read(&bmapINODES,sb.ninodes,sb.startInArea-1);
	if (ercode < 0) return ercode;

	int i = 0;

	// Going through the inodes in the bytemap
	while(i < bmapINODES.size){

		// Reading the current inode
		ercode = inode_ops.read(sb.startInArea,i,&in);
		if (ercode < 0) return ercode;
		
		if(bmapINODES.bmap[i] != in.isvalid) {
			printf("Contradiction in inode validation\n");
        	return 0;
  		}
		
		i++;
	}

	// Checking whether there are entries in the bytemap with information contradicting that of the inodes

	// Reading the bytemap
	ercode = bmap_ops.read(&bmapDATA,sb.ndatablocks,sb.startDtBmap);
	if (ercode < 0) return ercode;

	int index = 0;

	// Going through the inodes in the bytemap
	while(index < bmapDATA.size){

		// Reading the current inode
		ercode = inode_ops.read(sb.startInArea,index/POINTERS_PER_INODE,&in);
		if (ercode < 0) return ercode;

		int size = in.size;

		if(in.isvalid){

		//Going through all the pointers in the node
		for(int j = 0; j<POINTERS_PER_INODE; j++){

			if(bmapDATA.bmap[j+index] == 1 && size<0) {
				printf("Contradiction between the bytemap data blocks and the inode informations\n");      	
				return 0;
			}
			
			else if(bmapDATA.bmap[j+index] == 0 && size>0){
				printf("Contradiction between the bytemap data blocks and the inode informations\n");         	
				return 0;
			}

			size = size - DISK_BLOCK_SIZE;

			}
		}
		
		index = index+POINTERS_PER_INODE;

	}

	// Checking if the positions in the pointers array for the inode are correctly initialized

	// Going through the inodes in the bytemap
	for(index = 0; index < bmapDATA.size; index+=POINTERS_PER_INODE){
		
		// Reading the current inode
		ercode = inode_ops.read(sb.startInArea,index/POINTERS_PER_INODE,&in);
		if (ercode < 0) return ercode;

		int size = in.size;

		if(in.isvalid){

			// Going through all the pointers in the inode
			for(int j=0; j<POINTERS_PER_INODE;j++){

				// Checking if the current pointer is properly initilized
				if(size<=0&& in.direct[j]!=0)
					printf("Invalid pointer to data block\n");

				size = size - DISK_BLOCK_SIZE;

			}

		}
	}
	
  	return 0;
}
