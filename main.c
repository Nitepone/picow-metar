#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"

#include "example_http_client_util.h"
#include "EPD_printf.h"
#include "EPD_1in54_V2.h"

#define HOST "aviationweather.gov"
#define URL_REQUEST "/api/data/metar?ids=KSEA"

#define RXBUF_SZ 4096

#define REQ_INTERVAL_MS 300000

char rxbuf[RXBUF_SZ] = {0};

err_t http_client_rx_to_rxbuf(__unused void *arg, __unused struct altcp_pcb *conn, struct pbuf *p, err_t err) {
    unsigned offset;
    char cur_char;
    char* buf_ptr = rxbuf;
    //TODO(luna) Start at 0.. when u figure out how to parse p_buf proper
    for (offset = 2; offset < p->tot_len && offset < (sizeof(rxbuf) - 2); offset++) {
        cur_char = (char)pbuf_get_at(p, offset);
        printf("%x", cur_char);
        if ((cur_char >= 'A' && cur_char <= 'z')
            || (cur_char >= '0' && cur_char <= '9')
            || cur_char == ' '
            || cur_char == '$'
            )
        {
            *(buf_ptr++) = cur_char;
        }
    }
    printf("\n");
    *buf_ptr = '\0';
    pbuf_free(p);
    return ERR_OK;
}

void establish_wifi_connection() {
    int result = 0;
    int retry_count = 0;
    while ((
        result = cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID,
            WIFI_PASSWORD,
            CYW43_AUTH_WPA2_AES_PSK,
            10000)
    )) {
        retry_count++;

        // high power retry (every 250 ms)
        if (retry_count < 10) {
            EPD_printf("Retrying to connect to %s.."
                       "Attempt: %d, Code %d",
                       WIFI_SSID, retry_count, result);
            sleep_ms(250);
            continue;
        }

        // lower power retry loop (sleep 5000 ms)
        if ((retry_count % 10) == 0) {
            EPD_printf("Retrying to connect to %s.."
                       "Attempt: %d, Code %d",
                       WIFI_SSID, retry_count, result);
        }
        sleep_ms(5000);
    }
}



int make_web_request() {
    int result = 0;
    EXAMPLE_HTTP_REQUEST_T req1 = {0};

    // setup request
    req1.hostname = HOST;
    req1.url = URL_REQUEST;
    req1.headers_fn = NULL;
    req1.recv_fn = http_client_rx_to_rxbuf;
    req1.tls_config = altcp_tls_create_config_client(NULL, 0); // https

    // make request
    result += http_client_request_sync(cyw43_arch_async_context(), &req1);
    if (result != 0) {
        printf("Web request ret %d\n", result);
    }
    altcp_tls_free_config(req1.tls_config);
    return result;
}

void debug_print_rxbuf(void) {
    char* cur_buf;
    printf("rxbuf: ");
    for (
        cur_buf = rxbuf;
        (*cur_buf != '\0' && cur_buf < rxbuf + sizeof(rxbuf));
        cur_buf++)
    {
        printf("%x", *cur_buf);
    }
    printf("\n");
}

int main() {
    int ret = 0;
    int webreq_ret = 0;
SYS_INIT:
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    EPD_init();
    cyw43_arch_enable_sta_mode();
START_CONNECTION:
    if (webreq_ret == 0) {
        EPD_printf("Connecting to WLAN:\n  %s", WIFI_SSID);
    } else {
        EPD_printf("Reconnecting to WLAN\nPrev WebReq Ret: %d", webreq_ret);
    }
    establish_wifi_connection();
MAKE_REQUEST:
    if ((webreq_ret = make_web_request())) {
        goto START_CONNECTION;
    }
    debug_print_rxbuf();
    EPD_print(rxbuf);
    printf("Data displayed, sleeping for %dms\n", REQ_INTERVAL_MS);
    sleep_ms(REQ_INTERVAL_MS);
    goto MAKE_REQUEST;
EXIT:
    cyw43_arch_deinit();
    return 0;
}
