/***************************************************************************
 *   Copyright (C) 2019 by LambdaConcept                                   *
 *   Pierre-Olivier Vauboin <po@lambdaconcept.com>                         *
 *   Ramtin Amin <ramtin@lambdaconcept.com>                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include "bonobo.h"

#define ERROR_OK	(0)
#define ERROR_FAIL	(-4)

#define LOG_ERROR printf

static int bonobo_usb_open(libusb_context *usb_ctx, libusb_device_handle **new_handle)
{
	libusb_device_handle* usb_handle;

	usb_handle = libusb_open_device_with_vid_pid(usb_ctx, VID, PID);

	if (!usb_handle) {
		LOG_ERROR("Failed to open or find the device\n");
		return ERROR_FAIL;
	}

	/* Claim the Bonobo (bulk transfer) interface */

	if (libusb_claim_interface(usb_handle, 0) != ERROR_OK) {
		LOG_ERROR("Failed to claim Bonobo (bulk transfer) interface\n");
		return ERROR_FAIL;
	}

	*new_handle = usb_handle;

	return ERROR_OK;
}

static void bonobo_usb_close(libusb_device_handle* usb_handle)
{
	libusb_close(usb_handle);
}

int jtag_libusb_bulk_write(libusb_device_handle *dev, int ep, char *bytes,
		int size, int timeout)
{
	int transferred = 0;

	libusb_bulk_transfer(dev, ep, (unsigned char *)bytes, size,
			     &transferred, timeout);
	return transferred;
}

int jtag_libusb_bulk_read(libusb_device_handle *dev, int ep, char *bytes,
		int size, int timeout)
{
	int transferred = 0;

	libusb_bulk_transfer(dev, ep, (unsigned char *)bytes, size,
			     &transferred, timeout);
	return transferred;
}

static int bonobo_reset_target(libusb_device_handle *usb_handle)
{
	int ret;
	uint8_t buf[4];

	/* Reset the ACC line */

	buf[0] = CMD_MUX_SEL;
	buf[1] = MUX_MODE_ACCRESET;

	ret = jtag_libusb_bulk_write(usb_handle, BULK_EP_OUT,
			(char *)buf, 2, BULK_EP_TIMEOUT);
	if (ret <= 0) {
		LOG_ERROR("Bulk write failed\n");
		return ERROR_FAIL;
	}

	/* Select SDQ RESET sequence */

	buf[0] = CMD_SDQ_SEL;
	buf[1] = SDQ_MODE_RESET;

	ret = jtag_libusb_bulk_write(usb_handle, BULK_EP_OUT,
			(char *)buf, 2, BULK_EP_TIMEOUT);
	if (ret <= 0) {
		LOG_ERROR("Bulk write failed\n");
		goto err;
	}

	/* MUX select SDQ */

	buf[0] = CMD_MUX_SEL;
	buf[1] = MUX_MODE_SDQ;

	ret = jtag_libusb_bulk_write(usb_handle, BULK_EP_OUT,
			(char *)buf, 2, BULK_EP_TIMEOUT);
	if (ret <= 0) {
		LOG_ERROR("Bulk write failed\n");
		goto err;
	}

	/* Check SDQ sequence status */

	buf[0] = CMD_SDQ_RESULT;

	ret = jtag_libusb_bulk_write(usb_handle, BULK_EP_OUT,
			(char *)buf, 1, BULK_EP_TIMEOUT);
	if (ret <= 0) {
		LOG_ERROR("Bulk write failed\n");
		goto err;
	}

	ret = jtag_libusb_bulk_read(usb_handle, BULK_EP_IN,
			(char *)buf, 1, BULK_EP_TIMEOUT);
	if (ret <= 0) {
		LOG_ERROR("Bulk read failed\n");
		goto err;
	}

	if (!(buf[0] & SDQ_STATUS_DONE)) {
		LOG_ERROR("No SDQ, phone not plugged/powered ?\n");
		goto err;
	}

	return ERROR_OK;

err:
	return ERROR_FAIL;
}

int main(int argc, char *argv[])
{
	libusb_context *usb_context;
	libusb_device_handle* usb_handle;

	if (libusb_init(&usb_context) < 0)
		return ERROR_FAIL;

	/* Open USB device */

	if (bonobo_usb_open(usb_context, &usb_handle) != ERROR_OK) {
		LOG_ERROR("Can't find a Bonobo device! Please check device connections and permissions.\n");
		goto err_open;
	}

	do {
	} while (bonobo_reset_target(usb_handle) != ERROR_OK);
	bonobo_reset_target(usb_handle);

	bonobo_usb_close(usb_handle);

err_open:
	libusb_exit(usb_context);
	return 0;
}
