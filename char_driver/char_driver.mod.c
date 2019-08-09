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
	{ 0xa06673ee, "device_destroy" },
	{ 0x77ba5d1, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x67dc5402, "class_destroy" },
	{ 0xd05ae7ea, "cdev_add" },
	{ 0x681fd752, "cdev_init" },
	{ 0xb61e0ff7, "cdev_alloc" },
	{ 0xafb7f488, "device_create" },
	{ 0x25bae955, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x37a0cba, "kfree" },
	{ 0xf4fa543b, "arm_copy_to_user" },
	{ 0x5f754e5a, "memset" },
	{ 0x28cc25db, "arm_copy_from_user" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x9d669763, "memcpy" },
	{ 0x697ad648, "kmem_cache_alloc_trace" },
	{ 0x803600d2, "kmalloc_caches" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x7c32d0f0, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "DFE8DB801FA7E3E66056084");
