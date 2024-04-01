/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"

#define HOST "aviationweather.gov"
#define URL_REQUEST "/api/data/metar?ids=KSEA"

int main() {
    int result = 0;
    int retry_count = 0;
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    retry_count = 1;
    while ((
        result = cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID,
            WIFI_PASSWORD,
            CYW43_AUTH_WPA2_AES_PSK,
            10000)
    )) {
        retry_count++;
        if (retry_count < 10) {
            printf("Failed to connect to %s on try %d with ret %d\n",
                    WIFI_SSID, retry_count, result);
            sleep_ms(250);
            continue;
        }

        // lower power retry loop
        if ((retry_count % 10) == 0) {
            printf("Failed to connect to %s on try %d with ret %d\n",
                    WIFI_SSID, retry_count, result);
        }
        sleep_ms(5000);
    }

    EXAMPLE_HTTP_REQUEST_T req1 = {0};
    req1.hostname = HOST;
    req1.url = URL_REQUEST;
    req1.headers_fn = http_client_header_print_fn;
    req1.recv_fn = http_client_receive_print_fn;

    //TODO remove this http test..
    result += http_client_request_sync(cyw43_arch_async_context(), &req1);

    req1.tls_config = altcp_tls_create_config_client(NULL, 0); // https
    result += http_client_request_sync(cyw43_arch_async_context(), &req1);
    altcp_tls_free_config(req1.tls_config);

    if (result != 0) {
        printf("Got ret %d\n", result);
        panic("Test failed");
    }

    cyw43_arch_deinit();
    printf("Test passed\n");
    sleep_ms(100);
    return 0;
}