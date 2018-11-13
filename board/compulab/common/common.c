// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2014 CompuLab, Ltd. <www.compulab.co.il>
 *
 * Authors: Igor Grinberg <grinberg@compulab.co.il>
 */

#include <common.h>
#include <asm/bootm.h>
#include <asm/gpio.h>
#include <asm/setup.h>

#include "common.h"
#include "eeprom.h"

void cl_print_pcb_info(void)
{
	u32 board_rev = get_board_rev();
	u32 rev_major = board_rev / 100;
	u32 rev_minor = board_rev - (rev_major * 100);

	if ((rev_minor / 10) * 10 == rev_minor)
		rev_minor = rev_minor / 10;

	printf("PCB:   %u.%u\n", rev_major, rev_minor);
}

#ifdef CONFIG_SERIAL_TAG
void __weak get_board_serial(struct tag_serialnr *serialnr)
{
	/*
	 * This corresponds to what happens when we can communicate with the
	 * eeprom but don't get a valid board serial value.
	 */
	serialnr->low = 0;
	serialnr->high = 0;
};
#endif

#ifdef CONFIG_CMD_USB
int cl_usb_hub_init(int gpio, const char *label)
{
	if (gpio_request(gpio, label)) {
		printf("Error: can't obtain GPIO%d for %s", gpio, label);
		return -1;
	}

	gpio_direction_output(gpio, 0);
	udelay(10);
	gpio_set_value(gpio, 1);
	udelay(1000);
	return 0;
}

void cl_usb_hub_deinit(int gpio)
{
	gpio_free(gpio);
}
#endif

#ifdef CONFIG_OF_BOARD_SETUP
#include <fdt_support.h>
#include <malloc.h>

/* FDT nodes list */
static struct list_head fdt_nodes;

char *fdt_node_action_name[] = {
	"NODE_ENABLE",
	"NODE_DISABLE",
	"NODE_DELETE",
	"NODE_PROP_SET",
	"NODE_PROP_DELETE",
};

static inline void fdt_node_list_create(void)
{
	INIT_LIST_HEAD(&fdt_nodes);
}

static struct list_head *fdt_node_list_get(void)
{
	static int initialized;

	if (!initialized) {
		fdt_node_list_create();
		initialized = 1;
	}

	return &fdt_nodes;
}

static inline void fdt_node_prepare(struct fdt_node *node, const char *name,
				    struct fdt_node_prop *prop,
				    enum fdt_node_action action)
{
	memset(node, 0, sizeof(struct fdt_node));
	node->name = name;
	node->acton = action;
	node->prop = prop;
	INIT_LIST_HEAD(&node->list);
}

static void fdt_node_insert(struct fdt_node *node)
{
	struct list_head *fdt_node_list = fdt_node_list_get();

	list_add_tail(&node->list, fdt_node_list);
}

static int fdt_node_add(const char *name, enum fdt_node_action action)
{
	struct fdt_node *node = NULL;

	node = (struct fdt_node *)malloc(sizeof(struct fdt_node));
	if (!node)
		return -ENOMEM;

	fdt_node_prepare(node, name, NULL, action);

	fdt_node_insert(node);

	return 0;
}

int fdt_node_enable(const char *name)
{
	return fdt_node_add(name, FDT_NODE_ENABLE);
}

int fdt_node_disable(const char *name)
{
	return fdt_node_add(name, FDT_NODE_DISABLE);
}

int fdt_node_delete(const char *name)
{
	return fdt_node_add(name, FDT_NODE_DELETE);
}

static int fdt_prop_add(const char *node, enum fdt_node_action action,
			const char *name, void *val, int len,
			int create)
{
	struct fdt_node *new = NULL;
	struct fdt_node_prop *prop = NULL;

	prop = (struct fdt_node_prop *)malloc(sizeof(struct fdt_node_prop)
					      + len);
	if (!prop)
		return -ENOMEM;

	memset(prop, 0, sizeof(struct fdt_node_prop));
	prop->name = name;
	if (action == FDT_NODE_PROP_SET) {
		if (val)
			memcpy(prop->val, val, len);
		prop->len = len;
		prop->create = create;
	}

	/* Prepare and populate node struct, insert to the list */
	new = (struct fdt_node *)malloc(sizeof(struct fdt_node));
	if (!new)
		return -ENOMEM;

	fdt_node_prepare(new, node, prop, action);

	fdt_node_insert(new);

	return 0;
}

int fdt_prop_set(const char *node, const char *name,
		 void *val, int len, int create)
{
	return fdt_prop_add(node, FDT_NODE_PROP_SET, name,
			    val, len, create);
}

int fdt_prop_del(const char *node, const char *name)
{
	return fdt_prop_add(node, FDT_NODE_PROP_DELETE, name,
			    NULL, 0, 0);
}

int __weak fdt_board_adjust(void)
{
	return 0;
}

int __weak ft_board_setup(void *blob, bd_t *bd)
{
	struct list_head *fdt_nodes_list;
	struct list_head *entry, *tmp;
	struct fdt_node *node;
	struct fdt_node_prop *prop;
	int nodeoffset;

	/* Resize FDT to be on the safe side */
	fdt_shrink_to_minimum(blob, 0);

	/* Call board specific routine to populate the node list */
	fdt_board_adjust();

	/* Get node list */
	fdt_nodes_list = fdt_node_list_get();

	/* For each node check action to be done and apply required changes */
	list_for_each_safe(entry, tmp, fdt_nodes_list) {
		/* Get list entry */
		node = list_entry(entry, struct fdt_node, list);
		prop = node->prop;
		debug("%s: get node: name %s, action %s\n",
		      __func__, node->name, fdt_node_action_name[node->acton]);
		if (node->acton >= FDT_NODE_PROP_SET) {
			int i;
			debug("%s: node prop name %s\n",
			      __func__, node->prop->name);
			debug("%s: prop val", __func__);
			for (i = 0; i < node->prop->len; i++)
				debug(" %02x", ((char *)(node->prop->val))[i]);
			debug("\n");
		}

		switch (node->acton) {
		case FDT_NODE_ENABLE:
			fdt_status_okay_by_alias(blob, node->name);
			break;
		case FDT_NODE_DISABLE:
			fdt_status_disabled_by_alias(blob, node->name);
			break;
		case FDT_NODE_DELETE:
			fdt_del_node_and_alias(blob, node->name);
			break;
		case FDT_NODE_PROP_SET:
			fdt_find_and_setprop(blob, node->name, prop->name,
					     prop->val, prop->len,
					     prop->create);
			break;
		case FDT_NODE_PROP_DELETE:
			nodeoffset = fdt_path_offset(blob, node->name);
			fdt_delprop(blob, nodeoffset, prop->name);
			break;
		default:
			debug("%s: Error: unsupported node action\n",
			      __func__);
		}

		/* Delete list entry */
		list_del(entry);
		/* Free memory allocated */
		if (prop)
			free(prop);

		free(node);
	}

	return 0;
}
#endif
