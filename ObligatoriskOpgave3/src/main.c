/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define RAND 0
#define FIFO 1
#define CUSTOM 2

struct disk *disk;
int handler_type;
int available_frames;

void page_fault_handler( struct page_table *pt, int page )
{
	printf("page fault on page #%d\n",page);

	//If physical memory location has read, add write.
	if (page_bits[page] == PROT_READ) {
		page_bits[page] == PROT_READ|PROT_WRITE;
		return;
	}

	if (available_frames) {
		//What to  store in physical memory.
		char what_to_write;
		//Read from disk.
		disk_read(disk, page, &what_to_write);
		int store_location = pt->nframes - available_frames;
		//Store value in physical memory.
		pt->physmem[store_location] = what_to_write;
		available_frames--;
	}

	else {
		//No space in physical memory, replace frame with requested page.
		switch (handler_type) {
			case RAND:
				//generate random number up to nframes (exclusive) (size of phys. mem.)
				//set chosen_page to random number.
				break;
			case FIFO:

				break;
			case CUSTOM:

				break;
		}

		//Shared write-to-disk logic for allalgorithms.
		//write_to_disk(chosen_page);
		//Write "old" frame to disk, pull new page.
	}
}

int main( int argc, char *argv[] )
{
	if(argc!=5) {
		printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <sort|scan|focus>\n");
		return 1;
	}

	int npages = atoi(argv[1]);
	int nframes = atoi(argv[2]);
	const char *handler = argv[3];
	const char *program = argv[4];

	if(!strcmp(handler,"rand"))	{
		handler_type = RAND;
	} else if (!strcmp(handler,"fifo")){
		handler_type = FIFO;
	} else if (!strcmp(handler,"custom")) {
		handler_type = CUSTOM;
	} else {
		fprintf(stderr,"unknown handler: %s\n", handler);
		return 1;
	}

	disk = disk_open("myvirtualdisk",npages);
	if(!disk) {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}


	struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
	if(!pt) {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	char *virtmem = page_table_get_virtmem(pt);

	char *physmem = page_table_get_physmem(pt);

	if(!strcmp(program,"sort")) {
		sort_program(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"scan")) {
		scan_program(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"focus")) {
		focus_program(virtmem,npages*PAGE_SIZE);

	} else {
		fprintf(stderr,"unknown program: %s\n", program);

	}

	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
