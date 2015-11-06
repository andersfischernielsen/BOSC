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
int *frame_to_page;

void page_fault_handler( struct page_table *pt, int page )
{
	printf("page fault on page #%d\n",page);

	int frame;
	int bits;
	page_table_get_entry(pt, page, &frame, &bits);

	//If physical memory location has read, add write.
	if (bits == PROT_READ) {
		printf("First if:\n");
		page_table_set_entry(pt, page, frame, (PROT_READ|PROT_WRITE));
	}

	else if (available_frames) {
		printf("Second if.\n");
		//What to  store in physical memory.
		char what_to_write;
		//Read from disk.
		disk_read(disk, page, &what_to_write);
		int store_location = page_table_get_nframes(pt) - available_frames;
		//Store value in physical memory.
		page_table_get_physmem(pt)[store_location] = what_to_write;
		//Set mapping back to page from frame (for performance later).
		frame_to_page[store_location] = page;
		available_frames--;

		page_table_set_entry(pt, page, store_location, PROT_READ);
	}

	else {
		int frame_to_overwrite;
		printf("%s\n", "tjek 9");
		//No space in physical memory, replace frame with requested page.
		switch (handler_type) {
			case RAND:
				//Generate random number up to nframes (exclusive).
				frame_to_overwrite = rand() % page_table_get_nframes(pt);
				break;
			case FIFO:
				break;
			case CUSTOM:
				break;
		}
		printf("%s\n", "tjek 10");
		//Override old page.
		int index_of_overriden_page = frame_to_page[frame_to_overwrite];
		page_table_get_entry(pt, index_of_overriden_page, &frame, &bits);
		printf("%s\n", "tjek 11");

		if (bits == (PROT_READ|PROT_WRITE)) {
			//Write old data to disk.
			char old_data;
			old_data = page_table_get_physmem(pt)[frame_to_overwrite];
			disk_write(disk, index_of_overriden_page, &old_data);
		}
		printf("%s\n", "tjek 12");
		//Set bit to 0 to indicate the value has been overriden.
		bits = 0;

		//Extract data from disk.
		char data;
		disk_read(disk, page, &data);
		//Write disk data to physical memory.
		page_table_get_physmem(pt)[frame_to_overwrite] = data;
		//Update page table to reflect physical memory changes.
		page_table_set_entry(pt, page, frame_to_overwrite, PROT_READ);
		frame_to_page[frame_to_overwrite] = page;
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

	return 0;
}
