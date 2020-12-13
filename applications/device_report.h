/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-14     ulogic       the first version
 */
#ifndef APPLICATIONS_DEVICE_REPORT_H_
#define APPLICATIONS_DEVICE_REPORT_H_

#define POST_RESP_BUFSZ     1024
#define POST_HEADER_BUFSZ   1024

#define POST_LOCAL_URI  "http://xxx.xxx.xxx/iot_test.php"

int user_webclient_post(char *post_json);


#endif /* APPLICATIONS_DEVICE_REPORT_H_ */
