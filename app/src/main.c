/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <app_version.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define THREAD_STACK_SIZE	512
#define THREAD_PRIORITY		10

static void thread_function(void *, void *, void *);

static void thread_function(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	while(1) {
		printk("hello\n");
		k_sleep(K_SECONDS(1));
	}
}

int main(void)
{
	return 0;
}

K_THREAD_DEFINE(my_tid, THREAD_STACK_SIZE,
                thread_function, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);

