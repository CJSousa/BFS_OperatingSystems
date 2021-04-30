#include <stdio.h>
#include <string.h>
#include <math.h>

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
#endif



/* bytemap operations */

// Reading a bytemap

static int bytemap_read(struct bytemap *bmap, unsigned int max_entries,\
			unsigned int absDskBlk) {
 	 
	int ercode;

  	//  Reading the bytemap block, using the absolute disk block value
  	ercode = disk_ops.read(absDskBlk,bmap->bmap);

  	if (ercode < 0) return ercode;

	// Using max_entries to set the bytemap size 
  	bmap->size = max_entries;

	//Setting index to 0
  	bmap->index =0;

  	return 0;
}

// Getting the next entry in the bytemap

static int bytemap_getNextEntry(struct bytemap *bmap) {
  int entry;

	// Checking if the size has been reached, returning -ENOSPC
  	if(bmap->size == bmap->index)
    		return -ENOSPC;
  
	
	// Returning the next value in the map (using index)
	entry = bmap->bmap[bmap->index];
	
	// update the index
  	bmap->index++;

  	return entry;
}

//Setting an index 
//We have not used this method

static int bytemap_setIndex(struct bytemap *bmap, unsigned int value) {

    bmap->index = value;

  	return value;
}


// Printing the contents of each entry in the bytemap

void bytemap_printTable(struct bytemap *bmap) {

 	 int left= bmap->size, entry= 0;

  	// Printing 16 entries per line

  	while (left) {

    		if ( (entry+1)%16 ) 
			printf("%u ", bmap->bmap[entry]);
    		else 
			printf("%u\n", bmap->bmap[entry]);
    		
		left--;
		entry++;
 	 }

	// Moving to the next line in case 16 entries have been reached

 	 if ( entry%16 ) 
		printf("\n"); 

}


struct bytemap_operations bmap_ops= {

	.read= bytemap_read,
	.getNextEntry= bytemap_getNextEntry,
	.setIndex= bytemap_setIndex,
	.printTable= bytemap_printTable

};
