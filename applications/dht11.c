/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-14     ulogic       the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "dhtxx.h"
#include <stdio.h>
#include "device_report.h"

#define DATA_PIN GET_PIN(G, 13)

#define DHT11_THREAD_PRIORITY         15
#define DHT11_THREAD_STACK_SIZE       2048
#define DHT11_THREAD_TIMESLICE        15

int t,h = 0;
char dht11_json[100];
extern int user_webclient_post(char *post_json);

static rt_thread_t tid2 = RT_NULL;

static void dht11_thread_entry(void *parameter)
{
    dht_device_t sensor = dht_create(DATA_PIN);

    if (dht_read(sensor))
    {
        while (1)
        {
            rt_int32_t integer;
            rt_int32_t decimal;
            rt_int32_t temp = dht_get_temperature(sensor);
            rt_int32_t flag = split_int(temp, &integer, &decimal, 10);

            rt_int32_t humi = dht_get_humidity(sensor);
            if ((temp != t) || (humi != h))
            {
                t = temp;h=humi;
//                rt_kprintf("temp: %s%d.%d C, humi: %3d.%d%%\n", flag > 0 ? "-" : "", integer, decimal, humi / 10, humi % 10);
                sprintf(dht11_json, "{\"temp\":  %s%d.%d, \"humi\": %3d.%d}", flag > 0 ? "-" : "", integer, decimal, humi / 10, humi % 10);
                rt_kprintf("dht11_json: %s\n", dht11_json);

                user_webclient_post(dht11_json);
            }
            rt_thread_mdelay(5000);
        }
    }
    else
    {
        rt_kprintf("Read dht11 sensor failed.\n");
    }

    dht_delete(sensor);
}

int dht11_sample(void)
{
    tid2 = rt_thread_create("dht11_sample", dht11_thread_entry, RT_NULL, DHT11_THREAD_STACK_SIZE, DHT11_THREAD_PRIORITY,
            DHT11_THREAD_TIMESLICE);

    if (tid2 != RT_NULL)
    {
        rt_thread_startup(tid2);
    }
    return 0;
}
MSH_CMD_EXPORT(dht11_sample, dht11_sample);
