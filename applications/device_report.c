/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-14     ulogic       the first version
 */

#include <string.h>

#include <rtthread.h>
#include <webclient.h>
#include "device_report.h"

char *post_json = "";

static int webclient_post_comm(char *uri, char *post_data)
{
    struct webclient_session* session = RT_NULL;
    unsigned char *buffer = RT_NULL;
    int index, ret = 0;
    int bytes_read, resp_status;

    buffer = (unsigned char *) web_malloc(POST_RESP_BUFSZ);
    if (buffer == RT_NULL)
    {
        rt_kprintf("no memory for receive response buffer.\n");
        ret = -RT_ENOMEM;
        goto __exit;
    }

    /* create webclient session and set header response size */
    session = webclient_session_create(POST_HEADER_BUFSZ);
    if (session == RT_NULL)
    {
        ret = -RT_ENOMEM;
        goto __exit;
    }

    /* build header for upload */
    webclient_header_fields_add(session, "Content-Length: %d\r\n", strlen(post_data));
    webclient_header_fields_add(session, "Content-Type: application/octet-stream\r\n");

    /* send POST request by default header */
    if ((resp_status = webclient_post(session, uri, post_data)) != 200)
    {
        rt_kprintf("POST Request Failed, Response(%d) Error.\n", resp_status);
        ret = -RT_ERROR;
        goto __exit;
    }

    rt_kprintf("Post Response Data: \n");
    do
    {
        bytes_read = webclient_read(session, buffer, POST_RESP_BUFSZ);
        if (bytes_read <= 0)
        {
            break;
        }

        for (index = 0; index < bytes_read; index++)
        {
            rt_kprintf("%c", buffer[index]);
        }
    } while (1);

    rt_kprintf("\n");

__exit:
    if (session)
    {
        webclient_close(session);
    }

    if (buffer)
    {
        web_free(buffer);
    }

    return ret;
}


int user_webclient_post(char *post_json)
{
    char *uri = RT_NULL;
    rt_kprintf("start request... \n");
    uri = web_strdup(POST_LOCAL_URI);
    if (uri == RT_NULL)
    {
        rt_kprintf("URL: \n", POST_LOCAL_URI);
        rt_kprintf("no memory for create post request uri buffer.\n");
        return -RT_ENOMEM;
    }

    webclient_post_comm(uri, post_json);
    rt_kprintf("end request... \n");
    if (uri)
    {
        web_free(uri);
    }

    return RT_EOK;
}
MSH_CMD_EXPORT(user_webclient_post, webclient post request test.);
