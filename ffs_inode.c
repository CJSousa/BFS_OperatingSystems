#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef FFS_INODE_H
#include "ffs_inode.h"
#endif


// Printing inode information

static void inode_print(unsigned int number, struct inode *in) {

	int size =in->size;

	// Printing the absulute number of the inode
  	printf("%d:\n", number);
	
	// Printing if the inode is valid or not
  	printf("\t%s\n", (in->isvalid)?"valid":"invalid");
  	
	if (in->isvalid) {
    		
		// Printing the size of the inode
		printf("\t%d\n", size);

		// Printing all the information for each inode
		for(int i = 0;i<POINTERS_PER_INODE;i++){
			
			//Checking if there is a pointer to an inode
			if(size>0)
      				printf("\t\t%d\n",in->direct[i]);
			else{
				printf("\t\t%s\n","NULL");
				}
				size = size - DISK_BLOCK_SIZE;
    		}
			

  	}
}

// Printing the inode table

static int inode_printTable(unsigned int ninodeblocks, unsigned int ninodes,\
			unsigned int inodesStartBlock) {
  	
	// Having ninodeblocks as the number of blocks of the inode table
	
	int ercode;
  	union in_block in_b;
  	

  	printf("i-nodes:\n");
	
	// Going through all the blocks in the table
	for(int i = 0; i< ninodeblocks; i++){

		// Getting a full inode block, knowing that the table starts at inodesStartBlock
		ercode = disk_ops.read(i+inodesStartBlock,in_b.data);

		if (ercode < 0) return ercode;

   		 // Printing each inode in the current block
    		for(int j = 0; j<INODES_PER_BLOCK && (i*16+j != ninodes);j++){

			 // Sending the number of the inode and its structure to print method
    			  inode_print(i*16+j,&in_b.ino[j]);
			 
		}
  	}
  
 	 return 0;
}


	/* inode (global) number is decomposed into inode block number
   	and offset within that block. The inode block number starts at 0 */

static void inode_location(unsigned int numinode,\
		 unsigned int *numblock, unsigned int *offset) {
  	
	*numblock= numinode / INODES_PER_BLOCK;
  	*offset= numinode % INODES_PER_BLOCK;
}


// Reading an i-node block from disk

static int inode_read(unsigned int startInArea, unsigned int absinode, struct inode *in) {
 	
	int ercode;
  	unsigned int block, offset;
  	union in_block in_b;

  	inode_location(absinode, &block, &offset);
  	
	// Reading the inode block
  	ercode = disk_ops.read(block+startInArea,in_b.data);
  	
	if (ercode < 0) return ercode;
  	
	// Extracting the inode information from the block into inode *in
  	*in = in_b.ino[offset];
  
 	return 0;
}


//Option A: printing a buffer of data up to toPrint chars

void f_data_print(unsigned char *buf, int toPrint) {
	
	int left= toPrint, entry= 0;

	// Printing 16 entries per line
  	while (left) {
	
		// Only printing 16 entries per line
    		if ( (entry+1)%16 ) 
			 printf("%c ", buf[entry]);
    		else 
			printf("%c\n", buf[entry]);
    		
		// Entries left to print
		left--; 
		// Next entry
		entry++;

  	}
  	
	if ( entry%16 ) printf("\n"); // last NL for general case

}

//Printing lines with characters 

static int inode_printFileData(unsigned int startInArea, unsigned int absinode,\
			   unsigned int startDtArea) {
 	
	 int ercode, size, toPrint;
     //unsigned int block, offset;
  	 unsigned char buf[DISK_BLOCK_SIZE];
   	 struct inode in;
	
	 
	// Reading the data block containing the inode number absinode
  	//inode_location(absinode, &block, &offset);
	  
	// Reading the inode block
  	ercode = inode_read(startInArea,absinode,&in);
  	
	if (ercode < 0) 
		return ercode;

	// Checking if the inode is valid
 	if (!in.isvalid) 
		return 0;  

  	printf("\nPrinting contents of file(inode) %d\n", absinode);

  	size = in.size;
  	
	// Making sure the size is adequate
	if (!size) {
		printf("** NO DATA **\n"); 
		return 0;
	}
	
	// Going through all the inodes
  	for (int i=0;i<POINTERS_PER_INODE && size>0;i++){

		//Reading according to the buffer
		disk_ops.read(in.direct[i]+startDtArea,buf);

		if (size<=DISK_BLOCK_SIZE)
			toPrint = size;
		 else
		  	toPrint = DISK_BLOCK_SIZE;
	  	 f_data_print(buf,toPrint);

	 	 size = size - DISK_BLOCK_SIZE;

	  }

  	return 0;
}

struct inode_operations inode_ops= {

	.read= inode_read,
	.printFileData= inode_printFileData,
	.printTable= inode_printTable

};
