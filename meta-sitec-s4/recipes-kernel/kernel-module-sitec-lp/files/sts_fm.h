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

#ifndef SITEC_STS_H
#define SITEC_STS_H

/**
 * @file: sts_fm.h
 * @author: Robert Lehmann <robert.lehmann@sitec-systems.de>
 * @brief File contains functions for communication with low power mcu.
 */

#include <linux/device.h>
#include <linux/slab.h>

#include "sitec_lp.h"

#define DATA_MAXSIZE 245
#define MSG_MAXSIZE 256
#define STS_MIN_SIZE 10
#define IND_LEN 6
#define IND_FG_ID 7
#define IND_VERSION 8

struct sts_msg {
    u8 data[DATA_MAXSIZE]; // msg
    u8 fg_id;              // function group id
    u8 version;            // version of sts command
    u8 len;                // length of the sts data + 2
    u8 crc;                // checksum of crc frame
};

struct sts_header {
    u8 fg_id;
    u8 version;
    u8 len;
};

/**
 * @brief Sends a sts command via the low power mcu.
 *
 * This function is a low level function for sending data to the low power mcu.
 * The functions
 * doesn't perform any checks if the sts frame is valid. This needs to be done
 * before calling that
 * function.
 * @param dev Pointer to the global device structure
 * @param data Data array which will be written to the mcu
 * @param len Length of the data array
 * @return Return 0 if everythings was fine, else return error code.
 */
int sitec_lp_sts_send(struct device *dev, u8 *data, size_t len);

/**
 * @brief Receives a sts response from the low power mcu.
 *
 * This function receives data which is send by the low power mcu to the ARM
 * core. The function
 * doesn't perform any checks if the received sts frame is valid. This needs to
 * be done by the caller
 * of the function.
 * @param dev Pointer to the global device structure
 * @param data Pointer to buffer where the sts response will be stored
 * @param len Number of elements which will be read and stored in the buffer
 * @return Return 0 if everything was fine, else return an error code.
 */
int sitec_lp_sts_recv(struct device *dev, u8 *data, size_t len);

/**
 * @brief Sends a flash monitor message to low power mcu.
 *
 * Function sends a data frame to the flash monitor.
 * @param dev Pointer to global device structure
 * @param data Pointer to send buffer
 * @param len Length of the send buffer
 * @return Returns 0 if everything was fine, else return an error code.
 */
int sitec_lp_fm_send(struct device *dev, u8 *data, size_t len);

/**
 * @brief Receives a flash monitor message from the low power mcu.
 *
 * Function receives a flash monitor message from the low power mcu.
 * @param dev Pointer to global device structure
 * @param buf Pointer to buffer where the flash monitor message will be stored
 * @param len Number of elements which will be read and stored into the buffer
 * @return Return 0 if everything works fine, else return an error code.
 */
int sitec_lp_fm_recv(struct device *dev, u8 *buf, size_t len);

/**
 * @brief Sends a sts I message.
 *
 * @param dev Pointer to global device structure
 * @rx_msg structure to fill for the received sts message
 * @return Return 0 if everything works fine, else return an error code.
 */
int sitec_lp_sts_i(struct device *dev, struct sts_msg *rx_msg);

/**
 * @brief Sends a sts C message.
 *
 * @param dev Pointer to global device structure
 * @return Return 0 if everything works fine, else return an error code.
 */
int sitec_lp_sts_c(struct device *dev);

/**
 * @brief Sends a sts C message with additional wakeup configuration
 *
 * @param dev pointer to global device structure
 * @param config pointer to wakeup structure
 * @return return 0 if everything works fine, else return an error code
 */
int sitec_lp_sts_c_wakeup(struct device *dev, struct wakeup_config *config);

/**
 * @brief Sends a sts G message.
 *
 * @param dev Pointer to global device structure
 * @return Return 0 if everything works fine, else return an error code.
 */
int sitec_lp_sts_u(struct device *dev);

/**
 * @brief Sends a sts P message
 *
 * @param dev Pointer to global device structure
 * @cmd character with the corresponding command for the P message
 * @rx_msg structure to fill for the received sts message
 * @return Return 0 if everything works fine, else return an error code.
 */
int sitec_lp_sts_p(struct device *dev, const char cmd, struct sts_msg *rx_msg);
#endif // SITEC_STS_H
