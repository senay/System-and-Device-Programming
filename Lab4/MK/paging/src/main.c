// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
//           Made for JamesM's tutorials <www.jamesmolloy.co.uk>

#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "paging.h"

int main(struct multiboot *mboot_ptr)
{
u32int *ptr, do_page_fault;
    // Initialise all the ISRs and segmentation
    init_descriptor_tables();
    // Initialise the screen (by clearing it)
    monitor_clear();

    initialise_paging();
    monitor_write("Hello, paging world!\n");
    
    ptr = 0x100020 ;
    monitor_write("Normal access at address 0x100020 contains  ");
    monitor_write_hex(*ptr);
    monitor_write("\n");
    
	ptr = 0;
	while (1) {
		do_page_fault = *ptr;
		monitor_write("Normal access at address 0x ");
		monitor_write_hex(ptr);
		monitor_write("\n");
		ptr += 1024;
	}

    return 0;
}
