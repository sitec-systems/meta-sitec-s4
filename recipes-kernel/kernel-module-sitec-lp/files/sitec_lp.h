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

#ifndef SITEC_LP_H
#define SITEC_LP_H

#include <linux/mutex.h>

struct wakeup_config {
    u8 data[3];
    u8 len;
};

struct sitec_lp_priv {
    struct device *dev;
    struct spi_device *spi;
    struct wakeup_config wakeup;
    struct mutex lock;
    int gpio;
    int irq;
};


#endif // SITEC_LP_H
