# include "processing_unit.h"
# include "esp_system.h"


void init_opencv_features2d();
void detect_features(uint8_t* data, int width, int heigth, bool processing);
void parse_camera_matrix(char * camera_matrix);
int compute_position();


esp_err_t process_frame(bool processing)
{
    //acquire a frame
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE("CAM", "Camera Capture Failed");
        return ESP_FAIL;
    }

    // Test Features extraction
    detect_features(fb->buf, fb->width, fb->height, processing);  
    if (processing) {
        ESP_LOGE("OPENCV", "MATCHES SIZE: %d\n", compute_position());
    }
    esp_camera_fb_return(fb);
    return ESP_OK;
}

void init_processing_unit(char *camera_matrix)
{
    init_opencv_features2d();
    parse_camera_matrix(camera_matrix);

    // bool processing = false;

    while(1)
    {

        int64_t fr_start = esp_timer_get_time();

    //     if(!process_frame(processing))
    //         processing = true;
        
        
    //     int64_t fr_end = esp_timer_get_time();
    //     float fps = 1*1000000/(fr_end - fr_start);
    //     ESP_LOGW("OpenCV", "Feature2d decode & compute - %2.2f FPS", fps);
    }
}
