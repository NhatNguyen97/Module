#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xefc72b87, "module_layout" },
	{ 0xfe990052, "gpio_free" },
	{ 0xb486bbec, "gpiod_unexport" },
	{ 0x77ba5d1, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x67dc5402, "class_destroy" },
	{ 0xa06673ee, "device_destroy" },
	{ 0xd05ae7ea, "cdev_add" },
	{ 0x681fd752, "cdev_init" },
	{ 0xb61e0ff7, "cdev_alloc" },
	{ 0xafb7f488, "device_create" },
	{ 0x25bae955, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xf972999c, "gpiod_get_raw_value" },
	{ 0x8e8383ad, "gpiod_direction_input" },
	{ 0xba32bd1b, "gpiod_set_raw_value" },
	{ 0x42086fb8, "gpiod_direction_output_raw" },
	{ 0x8b489103, "gpio_to_desc" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x8e865d3c, "arm_delay_ops" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x7c32d0f0, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "F094F87F9F0F442BAD5C932");
