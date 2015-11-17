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
#include <time.h>

#define RAND 0
#define FIFO 1
#define CUSTOM 2

struct disk *disk;
int handler_type;
int available_frames;
int *frame_to_page;
int fifo_position = 0;
int page_faults = 0;
int write_faults = 0;
int disk_reads = 0;
int disk_writes = 0;
int *pages_faults;

void page_fault_handler( struct page_table *pt, int page )
{
	page_faults++;
	// a page fault happened for this page.
	pages_faults[page]++;

	int nframes = page_table_get_nframes(pt);

	int frame, bits;
	page_table_get_entry(pt, page, &frame, &bits);

	//If physical memory location has read, add write rights.
	if (bits == PROT_READ) {
		write_faults++;
		// write faults are worse therefore counts double.
		pages_faults[page]++;
		page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE);
	}
	else if (available_frames) {
		//Get the store location.
		int store_location = nframes - available_frames;
		//Store value in physical memory.
		disk_read(disk, page, page_table_get_physmem(pt) + PAGE_SIZE * store_location);
		disk_reads++;

		//Set mapping back to page from frame (for performance later).
		frame_to_page[store_location] = page;
		available_frames--;

		page_table_set_entry(pt, page, store_location, PROT_READ);
	}
	else {
		//No space in physical memory, replace frame with requested page.
		int frame_to_overwrite, i, value, pageValue;
		switch (handler_type) {
			case RAND:
				//Generate random number up to nframes (exclusive).
				frame_to_overwrite = lrand48() % nframes;
				break;
			case FIFO:
				//Oldest elements at top of physmem.
				frame_to_overwrite = fifo_position;
				//Take the topmost element, set fifo position ++.
				//Fifo position must never be more than physmem size.
				fifo_position = (fifo_position+1) % nframes;
				break;
			case CUSTOM:
				frame_to_overwrite = -1;
				for (i = 0; i < nframes; i++) {
					// find the amount of page faults for the current frame.
					pageValue = pages_faults[frame_to_page[i]];
					// if first iteration use the first frame.
					if (frame_to_overwrite == -1)
					{
						frame_to_overwrite = i;
						value = pageValue;
					} // if the current element has fewer faults then use that instead
					else if (value > pageValue)
					{
						frame_to_overwrite = i;
						value = pageValue;
					}
				}
				break;
		}

		//Find page index to overwrite rights for.
		int index_of_overriden_page = frame_to_page[frame_to_overwrite];
		//Get the page entry.
		page_table_get_entry(pt, index_of_overriden_page, &frame, &bits);

		if (bits == (PROT_READ|PROT_WRITE)) {
			//Write the physical memory to the disk, since it is being swapped.
			disk_write(disk, index_of_overriden_page, page_table_get_physmem(pt) + PAGE_SIZE * frame_to_overwrite);
			disk_writes++;
		}
		//Extract data from disk of the frame of the page.
		disk_read(disk, page, page_table_get_physmem(pt) + PAGE_SIZE * frame_to_overwrite);
		disk_reads++;

		// Clear old value in virtual memory.
		page_table_set_entry(pt, index_of_overriden_page, 0, PROT_NONE);

		// map from frame back to the new page.
		frame_to_page[frame_to_overwrite] = page;

		//Update page table to reflect physical memory changes.
		page_table_set_entry(pt, page, frame_to_overwrite, PROT_READ);
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

	pages_faults = calloc(npages, sizeof(int));

	if(!strcmp(handler,"rand"))	{
		handler_type = RAND;
		srand48(time(NULL));
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

	frame_to_page = calloc(nframes, sizeof(int));
	available_frames = page_table_get_nframes(pt);

	char *virtmem = page_table_get_virtmem(pt);

	//char *physmem = page_table_get_physmem(pt);

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
	free(frame_to_page);
	free(pages_faults);

	printf("Page faults: %d, Write faults: %d\n", page_faults, write_faults);
	printf("Disk reads: %d, Disk writes: %d\n", disk_reads, disk_writes);

	return 0;
}
