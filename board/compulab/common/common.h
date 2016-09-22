/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2014 CompuLab, Ltd. <www.compulab.co.il>
 *
 * Authors: Igor Grinberg <grinberg@compulab.co.il>
 */

#ifndef _CL_COMMON_
#define _CL_COMMON_

#include <linux/errno.h>

void cl_print_pcb_info(void);

#ifdef CONFIG_CMD_USB
int cl_usb_hub_init(int gpio, const char *label);
void cl_usb_hub_deinit(int gpio);
#else /* !CONFIG_CMD_USB */
static inline int cl_usb_hub_init(int gpio, const char *label)
{
	return -ENOSYS;
}
static inline void cl_usb_hub_deinit(int gpio) {}
#endif /* CONFIG_CMD_USB */

#ifdef CONFIG_SMC911X
int cl_omap3_smc911x_init(int id, int cs, u32 base_addr,
			  int (*reset)(int), int rst_gpio);
#else /* !CONFIG_SMC911X */
static inline int cl_omap3_smc911x_init(int id, int cs, u32 base_addr,
					int (*reset)(int), int rst_gpio)
{
	return -ENOSYS;
}
#endif /* CONFIG_SMC911X */

#ifdef CONFIG_OF_BOARD_SETUP
enum fdt_node_action {
	FDT_NODE_ENABLE,
	FDT_NODE_DISABLE,
	FDT_NODE_DELETE,
	FDT_NODE_PROP_SET,
	FDT_NODE_PROP_DELETE,
};

struct fdt_node_prop {
	const char *name;
	int len;
	int create;
	char val[0];
};

struct fdt_node {
	struct list_head list;
	const char *name;
	struct fdt_node_prop *prop;
	enum fdt_node_action acton;
};

int fdt_board_adjust(void);
int fdt_node_enable(const char *name);
int fdt_node_disable(const char *name);
int fdt_node_delete(const char *name);
int fdt_prop_set(const char *node, const char *name,
		 void *val, int len, int create);
int fdt_prop_del(const char *node, const char *name);
#endif

#endif /* _CL_COMMON_ */
