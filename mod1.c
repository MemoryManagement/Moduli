#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/mmzone.h>
#include <linux/numa.h>	

int init_module(void)
{
	int i;
	printk(KERN_INFO "max zones: %d.\n", MAX_NR_ZONES);
	for (i=0; i<MAX_NR_ZONES; i++) {
		printk(KERN_INFO "zone %d: %s.\n", i,node_data[0]->node_zones[i].name);
		printk(KERN_INFO "zone %d: %ld.\n", i,node_data[0]->node_zones[i].managed_pages);
		printk(KERN_INFO "zone %d: %ld.\n", i,node_data[0]->node_zones[i].spanned_pages);
		printk(KERN_INFO "zone %d: %ld.\n", i,node_data[0]->node_zones[i].present_pages);
	}
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}
