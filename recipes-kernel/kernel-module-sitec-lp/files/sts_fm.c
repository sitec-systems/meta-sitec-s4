/*
 * Copyright (C) 2017 sitec systems GmbH
 * Copyright (C) 2018 sitec systems GmbH
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

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>

#include "sitec_lp.h"

static void sitec_lp_sts_crc(u8 *data, size_t len) {
    int i;
    u8 crc = data[0];

    for (i = 1; i < len - 1; i++) {
        crc ^= data[i];
    }

    data[len - 1] = ~crc;
}

static int sitec_lp_spi_recv(struct device *dev, u8 *data, size_t len,
                             long delay) {
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
        udelay(delay);
    }

    return 0;
}

static int sitec_lp_spi_send(struct device *dev, u8 *data, size_t len,
                             long delay) {
    struct sitec_lp_priv *d = dev_get_drvdata(dev);
    int i;
    int err;
    u8 buf;

    for (i = 0; i < len; i++) {
        buf = data[i];
#ifdef DEBUG
        dev_info(dev, "Write SPI Byte [0x%02x]\n", buf);
#endif  // DEBUG
        err = spi_write(d->spi, &buf, 1);
        if (err) {
            return err;
        }
        udelay(delay);
    }

    return 0;
}

static void build_crc(u8 *data, u8 len) {
    int i = 0;
    u8 crc = data[0];

    for (i = 1; i < len; i++) {
        crc ^= data[i];
    }

    data[len] = ~crc;
}

static size_t build_complete_msg(struct sts_msg *msg, u8 *data) {
    int i = 0;

    data[0] = '#';
    data[1] = 'S';
    data[2] = 'T';
    data[3] = 'S';
    data[4] = 0x01;
    data[5] = 0xFE;
    data[6] = msg->len + 2;
    data[7] = msg->fg_id;
    data[8] = msg->version;

    for (i = 0; i < msg->len; i++) {
        data[i + 9] = msg->data[i];
    }

    build_crc(data, msg->len + 9);

    return msg->len + 10;
}

static int sts_send_msg(struct device *dev, struct sts_msg *msg) {
    u8 buf[MSG_MAXSIZE];
    u8 outbuf[MSG_MAXSIZE];
    u8 *outbuf_ptr = &outbuf[0];
    int wbytes;
    int i = 0;

    size_t len = build_complete_msg(msg, buf);

    for (i = 0; i < len; i++) {
        wbytes = sprintf(outbuf_ptr, "{%02x}", buf[i]);
        outbuf_ptr += wbytes;
    }
#ifdef DEBUG
    dev_info(dev, "SEND: %s", outbuf);
#endif

    return sitec_lp_spi_send(dev, buf, len, 50);
}

static int sts_parse_header(struct device *dev, u8 *data, struct sts_header *header) {
    if (data[0] != '#' || data[1] != 'S' || data[2] != 'T' || data[3] != 'S') {
        return -EINVAL;
    }

    header->len = data[IND_LEN];
    header->fg_id = data[IND_FG_ID];
    header->version = data[IND_VERSION];

    return 0;
}

static void sts_fill_msg_header(struct sts_msg *msg, struct sts_header *header) {
    msg->fg_id = header->fg_id;
    msg->version = header->version;
    msg->len = header->len;
}

static void sts_fill_msg_data(struct sts_msg *msg, u8 *data) {
    int i = 0;

    for (i = 0; i < msg->len - 2; i++) {
        msg->data[i] = data[i];
    }
}

static int sts_recv_msg(struct device *dev, struct sts_msg *msg) {
    int ret = 0;
    u8 buf_header[10];
    u8 data[DATA_MAXSIZE + 1];
    struct sts_header header;

    // recv header
    ret = sitec_lp_spi_recv(dev, buf_header, 9, 50); 
    if (ret < 0) {
        return ret;
    }

    ret = sts_parse_header(dev, buf_header, &header);
    if (ret < 0) {
        return ret;
    }

    sts_fill_msg_header(msg, &header);

    if (header.len == 2) {
        return 0;
    }

    // recv data plus cr
    // data length is header->len = datacount + 2 so the length to read is header->len -2
    // but you altough want to read the crc so you need to add 1. the result is you need to
    // read header->len - 1 bytes
    ret = sitec_lp_spi_recv(dev, data, header.len - 1, 50);
    if (ret < 0) {
        return ret;
    }

    sts_fill_msg_data(msg, data);

    msg->crc = data[header.len - 1];
    return 0;
}

int sitec_lp_sts_send(struct device *dev, u8 *data, size_t len) {
    return sitec_lp_spi_send(dev, data, len, 50);
}
EXPORT_SYMBOL_GPL(sitec_lp_sts_send);

int sitec_lp_sts_recv(struct device *dev, u8 *data, size_t len) {
    return sitec_lp_spi_recv(dev, data, len, 50);
}
EXPORT_SYMBOL_GPL(sitec_lp_sts_recv);

int sitec_lp_fm_send(struct device *dev, u8 *data, size_t len) {
    return sitec_lp_spi_send(dev, data, len, 100);
}
EXPORT_SYMBOL_GPL(sitec_lp_fm_send);

int sitec_lp_fm_recv(struct device *dev, u8 *buf, size_t len) {
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
#endif  // DEBUG
        buf[i] = byte;
        mdelay(100);
    }

exit_fm_recv:
    return ret;
}
EXPORT_SYMBOL_GPL(sitec_lp_fm_recv);

int sitec_lp_sts_i(struct device *dev, struct sts_msg *rx_msg) {
    int err;
    struct sts_msg i_msg;
    i_msg.fg_id = 'I';
    i_msg.version = 0x10;
    i_msg.data[0] = '1';
    i_msg.len = 1;

    err = sts_send_msg(dev, &i_msg);
    if (err) {
        dev_err(dev, "Can't send STS I Message");
        return err;
    }

    mdelay(10);

    err = sts_recv_msg(dev, rx_msg);
    if (err < 0) {
        return err;
    };

    return 0;
}
EXPORT_SYMBOL_GPL(sitec_lp_sts_i);

int sitec_lp_sts_c(struct device *dev) {
    int err;
    u8 sh_frame[] = {'#',  'S', 'T',  'S', 0x01, 0xFE,
                     0x04, 'C', 0x10, 'N', '1',  0x00};

    sitec_lp_sts_crc(sh_frame, ARRAY_SIZE(sh_frame));
#ifdef DEBUG
    dev_info(dev, "Send sts c n 1 frame\n");
#endif  // DEBUG
    err = sitec_lp_sts_send(dev, sh_frame, ARRAY_SIZE(sh_frame));
    if (err) {
        return err;
    }

    return 0;
}
EXPORT_SYMBOL_GPL(sitec_lp_sts_c);

int sitec_lp_sts_c_wakeup(struct device *dev, struct wakeup_config *config) {
    int err;
    int i;
    struct sts_msg cmsg;

    cmsg.fg_id = 'C';
    cmsg.version = 0x10;
    cmsg.data[0] = 'N';
    cmsg.data[1] = '1';
    cmsg.len = 2 + config->len;

    for (i = 0; i < config->len; i++) {
        cmsg.data[2 + i] = config->data[i];
    }

    err = sts_send_msg(dev, &cmsg);
    if (err) {
        return err;
    }

    return 0;
}
EXPORT_SYMBOL_GPL(sitec_lp_sts_c_wakeup);

int sitec_lp_sts_u(struct device *dev) {
    int err;
    u8 fm_frame[] = {'#', 'S', 'T', 'S', 0x01, 0xFE, 0x02, 'U', 0x10, 0x00};

    sitec_lp_sts_crc(fm_frame, ARRAY_SIZE(fm_frame));
#ifdef DEBUG
    dev_info(dev, "Send sts u frame\n");
#endif  // DEBUG

    /* Switch into FM Mode */
    err = sitec_lp_sts_send(dev, fm_frame, ARRAY_SIZE(fm_frame));
    if (err) {
        return err;
    }

    return 0;
}
EXPORT_SYMBOL_GPL(sitec_lp_sts_u);

int sitec_lp_sts_p(struct device *dev, const char cmd, struct sts_msg *rx_msg) {
    int err;
    struct sts_msg tx_msg;
    tx_msg.fg_id = 'P';
    tx_msg.version = 0x10;
    tx_msg.len = 1;
    tx_msg.data[0] = '?';

    err = sts_send_msg(dev, &tx_msg);
    if (err) {
        return err;
    }

    mdelay(10);

    err = sts_recv_msg(dev, rx_msg);
    if (err) {
        return err;
    }

    return 0;
}

EXPORT_SYMBOL_GPL(sitec_lp_sts_p);
