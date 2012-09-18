/*
 * \brief  Linux platform_device emulation
 * \author Sebastian Sumpf
 * \date   2012-06-18
 */

/*
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <lx_emul.h>

#define to_platform_driver(drv) (container_of((drv), struct platform_driver, \
                                 driver))
#define to_platform_device(x) container_of((x), struct platform_device, dev)


static int platform_match(struct device *dev, struct device_driver *drv)
{
	if (!dev->name)
		return 0;

	return (strcmp(dev->name, drv->name) == 0);
}


static int platform_drv_probe(struct device *_dev)
{
	struct platform_driver *drv = to_platform_driver(_dev->driver);
	struct platform_device *dev = to_platform_device(_dev);

	return drv->probe(dev);
}


struct bus_type platform_bus_type = {
	.name  = "platform",
	.match = platform_match,
	.probe = platform_drv_probe
};

int platform_driver_register(struct platform_driver *drv)
{
	drv->driver.bus = &platform_bus_type;
	if (drv->probe)
		drv->driver.probe = platform_drv_probe;

	return driver_register(&drv->driver);
}

int platform_driver_probe(struct platform_driver *drv,
	int (*probe)(struct platform_device *))
{
	drv->probe = probe;
	return platform_driver_register(drv);
}

struct resource *platform_get_resource_byname(struct platform_device *dev,
                                              unsigned int type,
                                              const char *name)
{
	int i;

	for (i = 0; i < dev->num_resources; i++) {
		struct resource *r = &dev->resource[i];
		printk("%s: %p %s\n", __func__, r, r->name);

	if (type == r->flags && !strcmp(r->name, name))
		return r;
	}

	return NULL;
}

struct resource *platform_get_resource(struct platform_device *dev,
	unsigned int type, unsigned int id)
{
	int i;

	for (i = 0; i < dev->num_resources; i++) {
		struct resource *r = &dev->resource[i];
		printk("%s: %p %s\n", __func__, r, r->name);

	if (type == r->flags)
		return r;
	}

	return NULL;
}

int platform_get_irq_byname(struct platform_device *dev, const char *name)
{
	struct resource *r = platform_get_resource_byname(dev, IORESOURCE_IRQ, name);
	return r ? r->start : -1;
}


int platform_device_register(struct platform_device *pdev)
{
	pdev->dev.bus  = &platform_bus_type;
	pdev->dev.name = pdev->name;
	/* XXX: Fill with magic value to see page fault */
	pdev->dev.parent = (struct device *)0xaaaaaaaa;
	printk("%s: adding %s\n", __func__, pdev->name);
	device_add(&pdev->dev);
	printk("%s: added %s\n", __func__, pdev->name);
	return 0;
}

struct platform_device *platform_device_alloc(const char *name, int id)
{

	struct platform_device *pdev = NULL;
	pdev = kzalloc(sizeof(struct platform_device), 0);
	if (!pdev) {
		printk("%s: oom\n", __func__);
		return NULL;
	}

	pdev->name = name;
	pdev->dev.name = name;
	pdev->id = id;
	printk("%s: %p\n", __func__, pdev);
	return pdev;
}

void platform_device_del(struct platform_device *pdev) {
	kfree(pdev);
}

void platform_device_put(struct platform_device *pdev) {
}

void platform_set_drvdata(struct platform_device *pdev, void *data) {
}

int platform_device_add_resources(struct platform_device *pdev,
	const struct resource *res,
	unsigned int num)
{
	printk("%s: %s [%p %d]\n", __func__, pdev->dev.name, res, num);
	
	struct resource *r = NULL;
	if (res) {
		r = kmemdup(res, sizeof(struct resource) * num, GFP_KERNEL);
		if (!r) {
			return -ENOMEM;
		}
	}

	kfree(pdev->resource);
	pdev->resource = r;
	pdev->num_resources = num;
	return 0;
}

int platform_device_add_data(struct platform_device *pdev,
	const void *data, size_t size)
{
	printk("%s: %s [%p 0x%x]\n", __func__, pdev->dev.name, data, size);

	void *d = NULL;

	if (data) {
		d = kmemdup(data, size, GFP_KERNEL);
		if (!d)
			return -ENOMEM;
	}

	kfree(pdev->dev.platform_data);
	pdev->dev.platform_data = d;
	return 0;
}

int platform_device_add(struct platform_device *pdev)
{
	printk("%s: %s\n", __func__, pdev->dev.name);
	return platform_device_register(pdev);
}
