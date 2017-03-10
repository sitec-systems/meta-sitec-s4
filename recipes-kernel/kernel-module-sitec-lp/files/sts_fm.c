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

#include "sts_fm.h"

#include <linux/module.h>
#include <linux/device.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include "sitec_lp.h"

static void sitec_lp_sts_crc(u8 *data, size_t len)
{
	int i;
	u8 crc = data[0];

	for (i = 1; i < len -1; i++) {
		crc ^= data[i];
	}

	data[len-1] = ~crc;
}

static int sitec_lp_is_char_ascii(char c)
{
	if (c >= 0x21 && c <= 0x7e) {
		return 0;
	}

	return 1;
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
#if 0
		dev_info(dev, "Read SPI Byte [%d] [0x%02x]\n", i, buf);
#endif
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

int sitec_lp_sts_send(struct device *dev, u8 *data, size_t len)
{
	return sitec_lp_spi_send(dev, data, len, 10);
}
EXPORT_SYMBOL_GPL(sitec_lp_sts_send);


int sitec_lp_sts_recv(struct device *dev, u8 *data, size_t len)
{
	return sitec_lp_spi_recv(dev, data, len, 10);
}
EXPORT_SYMBOL_GPL(sitec_lp_sts_recv);

int sitec_lp_fm_send(struct device *dev, u8 *data, size_t len)
{
	return sitec_lp_spi_send(dev, data, len, 100);
}
EXPORT_SYMBOL_GPL(sitec_lp_fm_send);

int sitec_lp_fm_recv(struct device *dev, u8 *buf, size_t len)
{
	int ret;
	int i;
	u8 byte;
	struct sitec_lp_priv *d = dev_get_drvdata(dev);

	for (i = 0; i < len; i++) {
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
EXPORT_SYMBOL_GPL(sitec_lp_fm_recv);

int sitec_lp_sts_i(struct device *dev)
{
	int err;
	int i;
	u8 id_frame[] = {'#', 'S', 'T', 'S', 0x01, 0xFE, 0x03, 'I', 0x10, '1', 0x00};
	u8 rx_buf[128];

	sitec_lp_sts_crc(id_frame, ARRAY_SIZE(id_frame));
#ifdef DEBUG
	dev_info(dev, "Send sts i frame\n");
#endif
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

	for (i = 0; i < ARRAY_SIZE(rx_buf); i++) {
		if (!sitec_lp_is_char_ascii(rx_buf[i])) {
			dev_info(dev, "Recv: %c\n", rx_buf[i]);
		} else {
			dev_info(dev, "Recv: 0x%02x\n", rx_buf[i]);
		}
	}

	return 0;
}
EXPORT_SYMBOL_GPL(sitec_lp_sts_i);

int sitec_lp_sts_c(struct device *dev)
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
EXPORT_SYMBOL_GPL(sitec_lp_sts_c);

int sitec_lp_sts_u(struct device *dev)
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
EXPORT_SYMBOL_GPL(sitec_lp_sts_u);
