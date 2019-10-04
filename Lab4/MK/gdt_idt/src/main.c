// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
//           Made for JamesM's tutorials <www.jamesmolloy.co.uk>

#include "monitor.h"
#include "descriptor_tables.h"

int main(struct multiboot *mboot_ptr)
{
 int i=2, j=0;

    // Initialise all the ISRs and segmentation
    init_descriptor_tables();
    // Initialise the screen (by clearing it)
    monitor_clear();
    // Write out a sample string
    monitor_write("Hello, world!\n");

    asm volatile("int $0x3");
    asm volatile("int $0x4");

		monitor_write("i/j\n");
    i = i / j;
		monitor_write_dec(i);

		for(;;); 

    return 0;
}
