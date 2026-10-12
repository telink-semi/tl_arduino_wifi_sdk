/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2023 Telink Semiconductor (Shanghai) Co., Ltd.
 */

#ifndef IPC_ICMSG_H_
#define IPC_ICMSG_H_

#include <hal/kernel.h>
#include <hal/device.h>

enum ipc_icmsg_state {
	ICMSG_STATE_OFF,
	ICMSG_STATE_BUSY,
	ICMSG_STATE_READY,
};

/* Event callback structure */
struct ipc_icmsg_cb {
	/* This callback is called when the endpoint binding is successful */
	void (*bound)(void *priv);

	/* This callback is called when new data is received */
	void (*received)(const void *data, size_t len, void *priv);
};

struct ipc_icmsg_ops {
	int (*connect)(struct device *dev, const struct ipc_icmsg_cb *cb, void *user_data);
	int (*send)(struct device *dev, const void *msg, size_t len);
};

#define ipc_icmsg_ops(x)  ((struct ipc_icmsg_ops *)(x)->driver->ops)

/* Open an ipc icmsg instance and connect to the remote ipc icmsg instance */
static __inline__ int ipc_icmsg_connect(struct device *dev, const struct ipc_icmsg_cb *cb, void *user_data)
{
	if (!dev)
		return -ENODEV;

	return ipc_icmsg_ops(dev)->connect(dev, cb, user_data);
}

/* Send a message to the remote ipc icmsg instance */
static __inline__ int ipc_icmsg_send(struct device *dev, const void *msg, size_t len)
{
	if (!dev)
		return -ENODEV;

	return ipc_icmsg_ops(dev)->send(dev, msg, len);
}

#endif /* IPC_ICMSG_H_ */
