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

K_SEM_DEFINE(done_sem, 0, 1);

static void thread_function(void *, void *, void *);

static void thread_function2(void *, void *, void *);

static void thread_function(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	uint16_t counter = 0;

	while(1) {
		++counter;
		if (counter == 20000U) {
			k_sem_give(&done_sem);

			/* Force giving CPU time to other threads */
			// k_yield();
		}
		/* Or give CPU time */
		// k_sleep(K_USEC(100));
	}
}

static void thread_function2(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	k_sem_take(&done_sem, K_FOREVER);
	printk("DONE2");
}

int main(void)
{
	k_sem_take(&done_sem, K_FOREVER);
	printk("DONE");

	return 0;
}

K_THREAD_DEFINE(my_tid, THREAD_STACK_SIZE,
                thread_function, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(my_tid2, THREAD2_STACK_SIZE,
                thread_function2, NULL, NULL, NULL,
                THREAD2_PRIORITY, 0, 0);

