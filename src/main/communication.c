#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_http_server.h>
#include <nvs_flash.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include "camera_config.h"
#include "esp_mac.h"

#define EXAMPLE_ESP_WIFI_SSID      "Pathfinder"
#define EXAMPLE_ESP_WIFI_PASS      "pathfinder"
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_STA_CONN       5

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

#define WIFI_CONFIG_SSID "feever"
#define WIFI_CONFIG_PASSWORD "reveef"

esp_err_t stream_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len;
    uint8_t * _jpg_buf;
    char * part_buf[128];

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK) {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

    while(true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE("TEST", "Camera capture failed");
            res = ESP_FAIL;
            break;
        }
        if(fb->format != PIXFORMAT_JPEG){
            bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
            if(!jpeg_converted){
                ESP_LOGE("TEST", "JPEG compression failed");
                esp_camera_fb_return(fb);
                res = ESP_FAIL;
            }
        } else {
            _jpg_buf_len = fb->len;
            _jpg_buf = fb->buf;
        }

        if(res == ESP_OK) {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if(res == ESP_OK) {
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if(fb->format != PIXFORMAT_JPEG) {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        esp_camera_fb_return(fb);
        if(res != ESP_OK) {
            break;
        }
    }

    return res;
}

typedef struct {
    httpd_req_t *req;
    size_t len;
} jpg_chunking_t;

static size_t jpg_encode_stream(void *arg, size_t index, const void *data, size_t len) {
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if (!index) {
        j->len = 0;
    }
    if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK) {
        return 0;
    }
    j->len += len;
    return len;
}

esp_err_t capture_handler(httpd_req_t *req){
    camera_fb_t * frame = NULL;
    esp_err_t res = ESP_OK;

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    char ts[32];
    frame = esp_camera_fb_get();
    snprintf(ts, 32, "%ld.%06ld", frame->timestamp.tv_sec, frame->timestamp.tv_usec);
    httpd_resp_set_hdr(req, "X-Timestamp", (const char *)ts);

    if (frame->format == PIXFORMAT_JPEG) {
        res = httpd_resp_send(req, (const char *)frame->buf, frame->len);
    } else {
        jpg_chunking_t jchunk = {req, 0};
        res = frame2jpg_cb(frame, 80, jpg_encode_stream, &jchunk) ? ESP_OK : ESP_FAIL;
        httpd_resp_send_chunk(req, NULL, 0);
    }
    esp_camera_fb_return(frame);

    return res;
}

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html
esp_err_t get_matrix_handler(httpd_req_t *req) {
    char content[100];

    size_t recv_size = req->content_len;

    int ret = httpd_req_recv(req, content, recv_size);

    if (ret > 0) {  /* 0 return value indicates connection closed */
        printf("Matrix data: %s\n", content);
        return ESP_OK;
    }
    else {
        return ESP_FAIL;
    }

}


httpd_uri_t stream_uri = {
    .uri = "/stream",
    .method = HTTP_GET,
    .handler = stream_handler
    };

httpd_uri_t capture_uri = {
    .uri = "/capture",
    .method = HTTP_GET,
    .handler = capture_handler
    };

httpd_uri_t post_uri = {
    .uri = "/",
    .method = HTTP_POST,
    .handler = get_matrix_handler
};

httpd_handle_t start_webserver(void) {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  // Start the httpd server
  ESP_LOGI("TEST", "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK)
  {
    // Set URI handlers
    ESP_LOGI("TEST", "Registering URI handlers");
    httpd_register_uri_handler(server, &stream_uri);
    httpd_register_uri_handler(server, &capture_uri);
    httpd_register_uri_handler(server, &post_uri);
    return server;
  }

  ESP_LOGI("TEST", "Error starting server!");
  return NULL;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;

    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI("TAG", "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
        *server = start_webserver();
        
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI("TAG", "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
        /* Stop the web server */
        if (*server)
        {
            httpd_stop(server);
            *server = NULL;
        }
    }
}

void initialise_wifi(void *arg) {

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        arg,
                                                        NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
     wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(WIFI_CONFIG_PASSWORD) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

}