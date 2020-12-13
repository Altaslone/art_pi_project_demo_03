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
#include "drv_common.h"
#include "sensor.h"
#include <stdio.h>
#include "device_report.h"

#define LIGHT_THREAD_PRIORITY         10
#define LIGHT_THREAD_STACK_SIZE       2048
#define LIGHT_THREAD_TIMESLICE        10

#define LIGHT_PIN GET_PIN(G, 11)
#define LIGHT_LUX_LIMIT 100

char light_json[100];

static void bh1750_thread_entry(void *parameter)
{
    rt_device_t dev = RT_NULL;
    struct rt_sensor_data data;
    rt_size_t res;

    rt_int8_t LIGHT_STATUS = 0;
    rt_pin_mode(LIGHT_PIN, PIN_MODE_OUTPUT);

    /* ����bh1750������  */
    dev = rt_device_find("li_bh1750");
    if (dev == RT_NULL)
    {
        rt_kprintf("Can't Find Device: li_bh1750\n");
        return;
    }
    rt_kprintf("Find Device: li_bh1750\n");

    /* ��ֻ��ģʽ��bh1750 */
    if (rt_device_open(dev, RT_DEVICE_FLAG_RDONLY) != RT_EOK)
    {
        rt_kprintf("Open Device Failed!\n");
        return;
    }
    rt_kprintf("Open Device Succeed!\n");

    while (1)
    {
        /* �Ӵ�������ȡһ������ */
        res = rt_device_read(dev, 0, &data, 1);
        if (1 != res)
        {
            rt_kprintf("Read Data Failed!Size Is %d", res);
        }
        else
        {

            if ((data.data.light / 10) <= LIGHT_LUX_LIMIT && (LIGHT_STATUS == 0))
            {
                LIGHT_STATUS = 1;
                rt_kprintf("Light On\n");
//                rt_kprintf("light: %4d.%d lux\n", data.data.light / 10, data.data.light % 10);
                rt_pin_write(LIGHT_PIN, PIN_HIGH);

                sprintf(light_json, "{\"lux\":  %4d.%d, \"light\": %d}", data.data.light / 10, data.data.light % 10, 1);
                rt_kprintf("light_json: %s\n", light_json);

                user_webclient_post(light_json);
            }

            if ((data.data.light / 10) > LIGHT_LUX_LIMIT && (LIGHT_STATUS == 1))
            {
                LIGHT_STATUS = 0;
                rt_kprintf("Light Off\n");
//                rt_kprintf("light:%4d.%d lux\n", data.data.light / 10, data.data.light % 10);
                rt_pin_write(LIGHT_PIN, PIN_LOW);

                sprintf(light_json, "{\"lux\":  %4d.%d, \"light\": %d}", data.data.light / 10, data.data.light % 10, 0);
                rt_kprintf("light_json: %s\n", light_json);

                user_webclient_post(light_json);
            }
        }
        rt_thread_mdelay(1000);
    }
}

int bh1750_example(void)
{
    rt_thread_t tid; /* �߳̾�� */

    tid = rt_thread_create("bh1750_thread", bh1750_thread_entry, RT_NULL, LIGHT_THREAD_STACK_SIZE, LIGHT_THREAD_PRIORITY,
            LIGHT_THREAD_TIMESLICE);

    if (tid != RT_NULL)
    {
        /* �̴߳����ɹ��������߳� */
        rt_thread_startup(tid);
    }

    return 0;
}

/* ������ msh �����б��� */
MSH_CMD_EXPORT(bh1750_example, bh1750 example);
