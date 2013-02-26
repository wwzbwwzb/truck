#include <linux/module.h>
static int __init core_init(void)
{
	printk("start \n");
	return 0;
}

static void __exit core_cleanup(void)
{
	printk("exit\n");
}

module_init(core_init);
module_exit(core_cleanup);

MODULE_AUTHOR("Jeff <Jeff@h.com>");
MODULE_LICENSE("GPL and additional rights");
