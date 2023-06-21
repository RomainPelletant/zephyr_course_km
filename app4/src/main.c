/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <app_version.h>
#include <zephyr/random/rand32.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define THREAD_STACK_SIZE	512
#define THREAD_PRIORITY		10

#define THREAD2_STACK_SIZE	512
#define THREAD2_PRIORITY	10

K_FIFO_DEFINE(fifo);

struct fifo_data_item_t {
    void *fifo_dummy;   /* 1st word reserved for use by FIFO */
    int32_t integer;
	float floating_point;
};

static void thread_function(void *, void *, void *);

static void thread_function2(void *, void *, void *);

static void thread_function(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	struct fifo_data_item_t tx_data;

	while(1) {
		tx_data.integer = sys_rand32_get();
		tx_data.floating_point = ((float)sys_rand32_get()/INT16_MAX + 3.56f);
		k_fifo_put(&fifo, &tx_data);
		k_sleep(K_SECONDS(1));
	}
}

static void thread_function2(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	struct fifo_data_item_t *rx_data;
	while(1) {
		rx_data = k_fifo_get(&fifo, K_FOREVER);
		printk("int %d / fp %f\n", rx_data->integer, rx_data->floating_point);
	}
}

int main(void)
{

	return 0;
}

K_THREAD_DEFINE(my_tid, THREAD_STACK_SIZE,
                thread_function, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(my_tid2, THREAD2_STACK_SIZE,
                thread_function2, NULL, NULL, NULL,
                THREAD2_PRIORITY, 0, 0);

