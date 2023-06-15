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
#include <linux/mutex.h>

#include "sitec_lp.h"
#include "sts_fm.h"

#define SITEC_LP_VERSION "1.0.1"

struct sitec_lp_priv *priv;

static ssize_t sitec_lp_sts_u_test_store(struct device *dev, struct device_attribute *attr,
	 const char *buf, size_t count)
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

static ssize_t sitec_lp_fm_c_test_store(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t count)
{
	int err;
	u8 fm_buffer[256];
	u8 cmd;

#ifdef DEBUG
	dev_info(dev, "Send C cmd to FM\n");
#endif // DEBUG
	/* Send command to build Checksum */
	cmd = 'C';
	err = sitec_lp_fm_send(dev, &cmd, 1);
	if (err) {
		dev_err(dev, "Can't send C commant to FM (err = %d)\n", err);
		// TODO: Error Handling improvements
		return err;
	}

#ifdef DEBUG
	dev_info(dev, "Recv Answer from FM\n");
#endif // DEBUG
	/* Recv answer from FM */
	err = sitec_lp_fm_recv(dev, fm_buffer, 18);
	if (err) {
		dev_err(dev, "Can't get checksum for Flash (err = %d)\n", err);
		return err;
	}

	fm_buffer[18] = '\0';
#ifdef DEBUG
	dev_info(dev, "CRC %s\n", fm_buffer);
#endif // DEBUG

	return count;
}
static DEVICE_ATTR(fm_c_test, S_IWUSR, NULL, sitec_lp_fm_c_test_store);

static ssize_t sitec_lp_fm_g_test_store(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t count)
{
	int err;
	u8 cmd;

#ifdef DEBUG
	dev_info(dev, "Send G cmd to FM\n");
#endif // DEBUG
	cmd = 'G';
	err = sitec_lp_fm_send(dev, &cmd, 1);
	if (err) {
		dev_err(dev, "Can't send G command to FM (err = %d)\n", err);
		// TODO: Error Handling improvements
		return err;
	}

	return count;
}
static DEVICE_ATTR(fm_g_test, S_IWUSR, NULL, sitec_lp_fm_g_test_store);


static ssize_t sitec_lp_sts_c_test_store(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t count)
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

static ssize_t sitec_lp_wakeup_config_store(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t count)
{
	u64 result = 0;
	int err;
	u8 temp[3];
	u8 len = 0;
	int i;

	err = kstrtoul(buf, 0, &result);
	if (err) {
		return err;
	}

	if (result <= 0xff && result > 0 ) {
		len = 1;
	} else if (result <= 0xffff && result > 0) {
		len = 2;
	} else if (result <= 0xffffff && result > 0) {
		len = 3;
	} else {
		len = 0;
	}

	temp[0] = result & 0xff;
	temp[1] = (result >> 8) & 0xff;
	temp[2] = (result >> 16) & 0xff;

	mutex_lock(&priv->lock);
	for (i = 0; i < len; i++) {
		priv->wakeup.data[i] = temp[i];
	}
	priv->wakeup.len = len;

	mutex_unlock(&priv->lock);

	return  count;
}

static DEVICE_ATTR(wakeup_config, S_IWUSR, NULL, sitec_lp_wakeup_config_store);

static ssize_t sitec_lp_version_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "Version: %s\n", SITEC_LP_VERSION);
}
static DEVICE_ATTR(version, S_IRUGO, sitec_lp_version_show, NULL);

static ssize_t sitec_lp_freq_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct sts_msg rx_msg;
    int err;
    u16 freq_di1;
    u16 freq_di2;
    u16 freq_di3;

    err = sitec_lp_sts_p(dev, '?', &rx_msg);
    if (err) {
        return err;
    }

    if (rx_msg.len < 9) {
        dev_err(dev, "Incorrect response");
        return -EIO;
    }

    freq_di1 = rx_msg.data[3] << 8;
    freq_di1 = freq_di1 | rx_msg.data[4];
    freq_di2 = rx_msg.data[5] << 8;
    freq_di2 = freq_di2 | rx_msg.data[6];
    freq_di3 = rx_msg.data[7] << 8;
    freq_di3 = freq_di3 | rx_msg.data[8];

    return sprintf(buf, "%d-%d-%d", freq_di1, freq_di2, freq_di3);
}
static DEVICE_ATTR(freq, S_IRUGO, sitec_lp_freq_show, NULL);

static ssize_t sitec_lp_fw_version_show(struct device *dev, struct device_attribute *attr,
        char *buf) {
    struct sts_msg rx_msg;
    char version[31];
    int ret;
    int i;
    int wbytes = 0;
    int count = 0;

    ret = sitec_lp_sts_i(dev, &rx_msg);
    if (ret < 0) {
        return ret;
    }
#ifdef DEBUG
    for (i = 0; i < rx_msg.len - 2; i++) {
        wbytes = sprintf(buf, "{0x%02x}", rx_msg.data[i]);
        buf += wbytes;
        count += wbytes;
    }

    return count;
#else //DEBUG
    rx_msg.data[30] = 0;
    for (i = 0; i < rx_msg.len - 2 ; i++) {
        if (rx_msg.data[i] == 0x0d) {
            sprintf(buf, "%c", '\n');
        } else {
            sprintf(buf, "%c", rx_msg.data[i]);
        }
        buf++;
    }

    return rx_msg.len - 2;
#endif
}
static DEVICE_ATTR(fw_version, S_IRUGO, sitec_lp_fw_version_show, NULL);

static struct attribute *sitec_lp_attributes[] = {
	&dev_attr_version.attr,
	&dev_attr_wakeup_config.attr,
	&dev_attr_sts_c_test.attr,
	&dev_attr_sts_u_test.attr,
	&dev_attr_fm_c_test.attr,
	&dev_attr_fm_g_test.attr,
	&dev_attr_freq.attr,
	&dev_attr_fw_version.attr,
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
	if (priv->wakeup.len == 0) {
		dev_info(priv->dev, "Normal shutdown");
		sitec_lp_sts_c(priv->dev);	
	} else {
		dev_info(priv->dev, "Shutdown with mask");
		sitec_lp_sts_c_wakeup(priv->dev, &priv->wakeup);
	}
}

static int sitec_lp_notify_sys(struct notifier_block *this, unsigned long code,
	void *unused)
{
	dev_info(priv->dev, "Recv reboot code 0x%x\n", (u32) code);
	switch(code) {
	case SYS_RESTART:
		dev_dbg(priv->dev, "Catch down event\n");
		sitec_lp_restart();
		dev_info(priv->dev, "Restart the system\n");
		break;
	case SYS_HALT:
		dev_dbg(priv->dev, "Catch halt event\n");
		sitec_lp_halt();
		dev_info(priv->dev, "Shutdown the system\n");
		break;
	case SYS_POWER_OFF:
		dev_info(priv->dev, "Catch Power Off event\n");
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

	client->mode = SPI_MODE_1;
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
	priv->wakeup.len = 0;

	mutex_init(&priv->lock);

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

	dev_info(priv->dev, "Successfully initialized\n");

	return 0;

exit_irq:
	gpio_free(priv->irq);
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
