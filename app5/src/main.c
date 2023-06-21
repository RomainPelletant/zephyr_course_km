/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <app_version.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define WORKQ_STACK_SIZE 8192
#define WORKQ_PRIORITY 15

static void string_timer_handler(struct k_timer *dummy);
K_TIMER_DEFINE(string_timer, string_timer_handler, NULL);

#if defined(CONFIG_OWN_WORKQ)
/**< @brief Define thread stack for workq >*/
K_THREAD_STACK_DEFINE(workq_stack, WORKQ_STACK_SIZE);
#endif

static struct device_info {
	struct k_work work;
	char timestamp[16];
	struct k_mutex mutex;
#if defined(CONFIG_OWN_WORKQ)
	struct k_work_q workq;
#endif
} m_device;

static void string_timer_handler(struct k_timer *dummy)
{
#if defined(CONFIG_OWN_WORKQ)
	k_work_submit_to_queue(&m_device.workq, &m_device.work);
#else
	k_work_submit(&m_device.work);
#endif
}

static void update_timestamp(struct k_work *item)
{
	struct device_info *the_device =
		CONTAINER_OF(item, struct device_info, work);

	if (k_mutex_lock(&the_device->mutex, K_FOREVER) == 0) {
		snprintk(the_device->timestamp, sizeof(the_device->timestamp),
			 	"%d", k_uptime_get_32());
		k_mutex_unlock(&the_device->mutex);
	}
}

int main(void)
{

	(void) k_mutex_init(&m_device.mutex);
	k_work_init(&m_device.work, update_timestamp);
#if defined(CONFIG_OWN_WORKQ)
	k_work_queue_start(&m_device.workq, workq_stack,
			   K_THREAD_STACK_SIZEOF(workq_stack),
			   WORKQ_PRIORITY,
			   NULL);
	k_thread_name_set(&m_device.workq.thread, "workqueue");
#endif
	k_timer_start(&string_timer, K_SECONDS(5), K_SECONDS(5));

	while(1) {
		if (k_mutex_lock(&m_device.mutex, K_MSEC(100)) == 0) {
			printk("timestamp %s\n", m_device.timestamp);
			k_mutex_unlock(&m_device.mutex);
		}
		k_sleep(K_SECONDS(1));
	}

	return 0;
}

