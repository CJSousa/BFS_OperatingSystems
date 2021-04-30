CFLAGS = -Wall

fsckBFS: fsckBFS.o disk_driver.o ffs_super.o ffs_bytemap.o ffs_inode.o
	gcc -o fsckBFS fsckBFS.o disk_driver.o ffs_super.o ffs_bytemap.o ffs_inode.o -lm

fsckBFS.o: fsckBFS.c
	gcc $(CFLAGS) -c fsckBFS.c

disk_driver.o: disk_driver.c disk_driver.h ffs_errno.h
	gcc $(CFLAGS) -c disk_driver.c

ffs_super.o: ffs_super.c ffs_super.h ffs_inode.h ffs_errno.h
	gcc $(CFLAGS) -c ffs_super.c

ffs_bytemap.o: ffs_bytemap.c ffs_bytemap.h ffs_errno.h
	gcc $(CFLAGS) -c ffs_bytemap.c

ffs_inode.o: ffs_inode.c ffs_inode.h ffs_errno.h
	gcc $(CFLAGS) -c ffs_inode.c

clean:
	rm fsckBFS fsckBFS.o disk_driver.o ffs_bytemap.o ffs_inode.o ffs_super.o
