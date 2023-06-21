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

#define THREAD2_STACK_SIZE	512
#define THREAD2_PRIORITY	10

K_MUTEX_DEFINE(mul_mutex);
K_SEM_DEFINE(mul_done, 0, 1);

static uint32_t variable = 30U;

static void thread_function(void *, void *, void *);

static void thread_function2(void *, void *, void *);

static void thread_function(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	if (k_mutex_lock(&mul_mutex, K_FOREVER) == 0) {
		variable *= 10U;
		printk("mul 10: %d\n", variable);
		k_sem_give(&mul_done);
		k_mutex_unlock(&mul_mutex);
	}
}

static void thread_function2(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	int ret = k_sem_take(&mul_done, K_FOREVER);
	if (ret == 0 && k_mutex_lock(&mul_mutex, K_FOREVER) == 0) {
		variable += 2U;
		printk("add 2: %d\n", variable);
		k_mutex_unlock(&mul_mutex);
	}
}

int main(void)
{
	if (k_mutex_lock(&mul_mutex, K_FOREVER) == 0) {
		variable = 30U;
		printk("set 30: %d\n", variable);
		k_mutex_unlock(&mul_mutex);
	}

	return 0;
}

K_THREAD_DEFINE(my_tid, THREAD_STACK_SIZE,
                thread_function, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(my_tid2, THREAD2_STACK_SIZE,
                thread_function2, NULL, NULL, NULL,
                THREAD2_PRIORITY, 0, 0);

