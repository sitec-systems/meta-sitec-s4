/*
 * Copyright (C) 2017 sitec systems GmbH
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU  General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA
 */
#define DEBUG

#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/delay.h>

#define SITEC_LP_VERSION "1.0.0"

struct sitec_lp_priv {
	struct device *dev;
	struct spi_device *spi;
	int gpio;
	int irq;
};

struct sitec_lp_priv *priv;

static void sitec_lp_sts_crc(u8 *data, size_t len)
{
	int i;
	u8 crc = data[0];
	
	for (i = 1; i < len -1; i++) {
		crc ^= data[i];
	}

	data[len-1] = ~crc;
}

static int sitec_lp_irq_ctrl(struct device *dev, u8 state)
{
	int err;
	struct sitec_lp_priv *d = dev_get_drvdata(dev);

	if (state) {
		err = devm_gpio_request_one(dev, d->irq, GPIOF_IN, "rdy-gpio");
		if (err < 0) {
			dev_err(priv->dev, "Unable to rquest interrupt gpio\n");
			return err;
		}
	} else {
		gpio_free(d->irq);
	}

	return 0;
}

static int sitec_lp_spi_recv(struct device *dev, u8 *data, size_t len, long delay)
{
	struct sitec_lp_priv *d = dev_get_drvdata(dev);
	int i;
	int err;
	u8 buf;

	for (i = 0; i < len; i++) {
		err = spi_read(d->spi, &buf, 1);
		if (err) {
			return err;
		}
		dev_info(dev, "Read SPI Byte [%d] [0x%02x]\n", i, buf);
		data[i] = buf;
		mdelay(delay);
	}

	return 0;
}

static int sitec_lp_spi_send(struct device *dev, u8 *data, size_t len, long delay)
{
	struct sitec_lp_priv *d = dev_get_drvdata(dev);
	int i;
	int err;
	u8 buf;

	for (i = 0; i < len; i++) {
		buf = data[i];
#ifdef DEBUG
		dev_info(dev, "Write SPI Byte [0x%02x]\n", buf);
#endif // DEBUG
		err = spi_write(d->spi, &buf, 1);
		if (err) {
			return err;
		}
		mdelay(delay);
	}

	return 0;
}

static int sitec_lp_sts_send(struct device *dev, u8 *data, size_t len)
{
	return sitec_lp_spi_send(dev, data, len, 10);
}

static int sitec_lp_sts_recv(struct device *dev, u8 *data, size_t len)
{
	return sitec_lp_spi_recv(dev, data, len, 10);
}

static int sitec_lp_fm_send(struct device *dev, u8 *data, size_t len)
{
	return sitec_lp_spi_send(dev, data, len, 100);
}

static int sitec_lp_fm_recv(struct device *dev, u8 *buf, size_t len)
{
	int ret;
	int i;
	u8 byte;
	struct sitec_lp_priv *d = dev_get_drvdata(dev);

	for (i = 0; i < len; i++) {
		/* while (gpio_get_value(d->irq) == 1) { */
		/* 	// Wait for device to become active */
		/* 	mdelay(100); */
		/* } */
		dev_info(dev, "Read byte\n");
		ret = spi_read(d->spi, &byte, 1);
		if (ret) {
			goto exit_fm_recv;
		}
#ifdef DEBUG
		dev_info(dev, "Readed byted [0x%02x]\n", byte);
#endif // DEBUG
		buf[i] = byte;
		mdelay(100);
	}

exit_fm_recv:
	return ret;
}

static int sitec_lp_sts_i(struct device *dev)
{
	int err;
	u8 id_frame[] = {'#', 'S', 'T', 'S', 0x01, 0xFE, 0x03, 'I', 0x10, '1', 0x00};
	u8 rx_buf[128];
	
	sitec_lp_sts_crc(id_frame, ARRAY_SIZE(id_frame));
	dev_info(dev, "Send sts i frame\n");
	err = sitec_lp_sts_send(dev, id_frame, ARRAY_SIZE(id_frame));
	if (err) {
		return err;
	}

	mdelay(100);

	dev_info(dev, "Recv answer\n");
	err = sitec_lp_sts_recv(dev, rx_buf, ARRAY_SIZE(rx_buf));
	if (err) {
		dev_info(dev, "Can't recv something\n");
		return err;
	}

	return 0;
}

static int sitec_lp_sts_c(struct device *dev)
{
	int err;
	u8 sh_frame[] = {'#', 'S', 'T', 'S', 0x01, 0xFE, 0x04, 'C', 0x10, 'N', '1', 0x00};
	
	sitec_lp_sts_crc(sh_frame, ARRAY_SIZE(sh_frame));
#ifdef DEBUG
	dev_info(dev, "Send sts c n 1 frame\n");
#endif // DEBUG
	err = sitec_lp_sts_send(dev, sh_frame, ARRAY_SIZE(sh_frame));
	if (err) {
		return err;
	}
	
	return 0;
}

static int sitec_lp_sts_u(struct device *dev)
{
	int err;
	u8 fm_frame[] = {'#', 'S', 'T', 'S', 0x01, 0xFE, 0x02, 'U', 0x10, 0x00};

	sitec_lp_sts_crc(fm_frame, ARRAY_SIZE(fm_frame));
#ifdef DEBUG
	dev_info(dev, "Send sts u frame\n");
#endif // DEBUG

	/* Switch into FM Mode */
	err = sitec_lp_sts_send(dev, fm_frame, ARRAY_SIZE(fm_frame));
	if (err) {
		return err;
	}

	return 0;
}

static ssize_t sitec_lp_sts_i_test_store(struct device *dev,
										 struct device_attribute *attr,
										 const char *buf,
										 size_t count)
{
	int err;

	err = sitec_lp_sts_i(dev);
	if (err) {
		dev_err(dev, "Can't send STS I Command (err = %d)\n", err);
		return err;
	}
	return count;
}

static DEVICE_ATTR(sts_i_test, S_IWUSR, NULL, sitec_lp_sts_i_test_store);

static ssize_t sitec_lp_sts_u_test_store(struct device *dev,
										 struct device_attribute *attr,
										 const char *buf,
										 size_t count)
{
	int err;

	err = sitec_lp_sts_u(dev);
	if (err) {
		dev_err(dev, "Can't send STS U Command (err = %d)\n", err);
		return err;
	}
	return count;
}

static DEVICE_ATTR(sts_u_test, S_IWUSR, NULL, sitec_lp_sts_u_test_store);

static ssize_t sitec_lp_fm_c_test_store(struct device *dev,
										struct device_attribute *attr,
										const char *buf,
										size_t count)
{
	int err;
	u8 fm_buffer[256];
	u8 cmd;

	err = sitec_lp_irq_ctrl(dev, 1);
	if (err) {
		dev_err(dev, "Can't access rdy gpio\n");
		return err;
	}

#if 0
	/* Try to recv '?' from FM */
	err = sitec_lp_fm_recv(dev, fm_buffer, 3);
	if (err) {
		dev_err(dev, "Can't receive data from FM (err = %d)\n", err);
		sitec_lp_irq_ctrl(dev, 0);
		return err;
	}

	if (fm_buffer[2] != '?') {
		dev_err(dev, "LP Controller is not in FM mode\n");
		sitec_lp_irq_ctrl(dev, 0);
		return -EIO;
	}
#endif

#ifdef DEBUG
	dev_info(dev, "Send C cmd to FM\n");
#endif // DEBUG
	/* Send command to build Checksum */
	cmd = 'C';
	err = sitec_lp_fm_send(dev, &cmd, 1);
	if (err) {
		dev_err(dev, "Can't send C commant to FM (err = %d)\n", err);
		// TODO: Error Handling improvements
		sitec_lp_irq_ctrl(dev, 0);
		return err;
	}

#ifdef DEBUG
	dev_info(dev, "Recv Answer from FM\n");
#endif // DEBUG
	/* Recv answer from FM */
	err = sitec_lp_fm_recv(dev, fm_buffer, 18);
	if (err) {
		dev_err(dev, "Can't get checksum for Flash (err = %d)\n", err);
		sitec_lp_irq_ctrl(dev, 0);
		return err;
	}

	fm_buffer[18] = '\0';
#ifdef DEBUG
	dev_info(dev, "CRC %s\n", fm_buffer);
#endif // DEBUG

	sitec_lp_irq_ctrl(dev, 0);

	return count;
}
static DEVICE_ATTR(fm_c_test, S_IWUSR, NULL, sitec_lp_fm_c_test_store);

static ssize_t sitec_lp_sts_c_test_store(struct device *dev,
										 struct device_attribute *attr,
										 const char *buf,
										 size_t count)
{
	int err;

	err = sitec_lp_sts_c(dev);
	if (err) {
		dev_err(dev, "Can't send STS CN1 Command (err = %d)\n", err);
		return err;
	}

	return count;
}

static DEVICE_ATTR(sts_c_test, S_IWUSR, NULL, sitec_lp_sts_c_test_store);

static ssize_t sitec_lp_version_show(struct device *dev,
									 struct device_attribute *attr,
									 char *buf)
{
	return sprintf(buf, "Version: %s\n", SITEC_LP_VERSION);
}
static DEVICE_ATTR(version, S_IRUGO, sitec_lp_version_show, NULL);

static struct attribute *sitec_lp_attributes[] = {
	&dev_attr_version.attr,
	&dev_attr_sts_c_test.attr,
	&dev_attr_sts_u_test.attr,
	&dev_attr_sts_i_test.attr,
	&dev_attr_fm_c_test.attr,
	NULL,
};

static const struct attribute_group sitec_lp_attr_group = {
	.attrs = sitec_lp_attributes,
};

static void sitec_lp_restart(void)
{
	gpio_set_value(priv->gpio, 1);
}

static void sitec_lp_halt(void)
{
}

static int sitec_lp_notify_sys(struct notifier_block *this, unsigned long code,
	void *unused)
{
	switch(code) {
	case SYS_RESTART:
		dev_info(priv->dev, "Catch down event\n");
		sitec_lp_restart();
		dev_info(priv->dev, "Restart the system\n");
		break;
	case SYS_HALT:
		dev_info(priv->dev, "Catch halt event\n");
		sitec_lp_halt();
		break;
	default:
		dev_info(priv->dev, "Unknown event\n");
	}
	return NOTIFY_DONE;
}


static struct notifier_block sitec_lp_notifier = {
	.notifier_call = sitec_lp_notify_sys,
};

static int sitec_lp_probe(struct spi_device *client)
{
	int err = 0;
	struct device_node *np;
		
	dev_info(&client->dev, "Initialize low power routine ...\n");
	
	err = spi_setup(client);
	if (err < 0) {
		dev_err(&client->dev, "Can't setup SPI interface\n");
		goto exit;
	}

	priv = kzalloc(sizeof(struct sitec_lp_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&client->dev, "Not enough memory\n");
		err = -ENOMEM;
		goto exit;
	}

	dev_set_drvdata(&client->dev, priv);
	priv->dev = &client->dev;
	priv->spi = client;
	
	np = priv->dev->of_node;
	if (!np) {
		dev_err(priv->dev, "Unable to get data from of node\n");
		err = -EINVAL;
		goto exit_free;
	}

	priv->gpio = of_get_named_gpio(np, "reset-gpio", 0);
	if (priv->gpio < 0) {
		dev_err(priv->dev, "Can't get reset gpio\n");
		err = -EINVAL;
		goto exit_free;
	}

	if(!gpio_is_valid(priv->gpio)) {
		dev_err(priv->dev, "Reset gpio is not valid\n");
		err = -EINVAL;
		goto exit_free;
	}

	err = devm_gpio_request_one(priv->dev, priv->gpio, GPIOF_OUT_INIT_LOW, "reset-gpio");
	if (err < 0) {
		dev_warn(priv->dev, "Unable to request reset gpio\n");
		goto exit_free;
	}

	err = sysfs_create_group(&priv->dev->kobj, &sitec_lp_attr_group);
	if (err) {
		dev_err(priv->dev, "Can't create sysfs entries\n");
		goto exit_irq;
	}

	err = register_reboot_notifier(&sitec_lp_notifier);
	if (err) {
		dev_err(priv->dev, "Can't register reboot notifier (err=%d)\n", err);
		goto exit_irq;
	}

	/* sitec_lp_sts_crc(shutdown_frame, ARRAY_SIZE(shutdown_frame)); */
	/* sitec_lp_sts_crc(shutdown_frame, ARRAY_SIZE(fm_frame)); */

	dev_info(priv->dev, "Successfully initialized\n");

	return 0;

exit_irq:
	gpio_free(priv->irq);

exit_gpio:
	gpio_free(priv->gpio);

exit_free:
	kfree(priv);

exit:
	return err;
}

static int sitec_lp_remove(struct spi_device *client)
{
	struct sitec_lp_priv *d = dev_get_drvdata(&client->dev);
	
	unregister_reboot_notifier(&sitec_lp_notifier);
	sysfs_remove_group(&d->dev->kobj, &sitec_lp_attr_group);
	gpio_free(priv->irq);
	gpio_free(priv->gpio);
	kfree(d);

	return 0;
}

static const struct of_device_id sitec_lp_of_match[] = {
	{
		.compatible = "sitec,sitec-lp",
	},
	{},
};
MODULE_DEVICE_TABLE(of, sitec_lp_of_match);

static const struct spi_device_id sitec_lp_id[] = {
    {"sitec-lp", 0}, {},
};
MODULE_DEVICE_TABLE(spi, sitec_lp_id);

static struct spi_driver sitec_lp_spi_driver = {
	.driver =
	{
		.owner = THIS_MODULE,
		.name = "sitec-lp",
		.of_match_table = sitec_lp_of_match,
	},
	.id_table = sitec_lp_id,
	.probe = sitec_lp_probe,
	.remove = sitec_lp_remove,
};

module_spi_driver(sitec_lp_spi_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert Lehmann <robert.lehmann@sitec-systems.de>");
MODULE_DESCRIPTION("Low Power Module for sitec systems devices");
