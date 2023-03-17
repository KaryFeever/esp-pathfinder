#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <opencv2/core/ocl.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

//#include <opencv2/core/version.hpp>
#include <opencv2/core/utility.hpp>

extern "C" void test_opencv(void);
extern "C" void test2_opencv(void);
extern "C" void test3_opencv(void);

#include "opencv2/opencv_modules.hpp"

#include "opencv2/core.hpp"
#include <opencv2/core/cuda.hpp>
#include <opencv2/core/cvstd.hpp>
#include <opencv2/core/softfloat.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

using namespace std;
using namespace cv;

// extern "C" long int sysconf(int);

// long int sysconf(int wtf)
// {
//     return 1;
// }


extern "C" void init_opencv_features2d();
extern "C" void detect_features(const uint8_t* data, int width, int height, bool processing);
extern "C" void parse_camera_matrix(const char* camera_matrix);
extern "C" int compute_position();

// cv::Ptr<cv::ORB> orb_detector;
// cv::Ptr<cv::DescriptorMatcher> bfmatcher;
// std::vector<cv::KeyPoint> prev_keypoints, cur_keypoints;
// cv::Mat prev_descriptors, cur_descriptors;
// cv::Mat CameraMatrix;
// cv::Mat DistCoeffs;


std::vector<cv::KeyPoint>* cur_keypoints = (std::vector<cv::KeyPoint>*)heap_caps_malloc(sizeof(std::vector<cv::KeyPoint>), MALLOC_CAP_SPIRAM  );
std::vector<cv::KeyPoint>* prev_keypoints = (std::vector<cv::KeyPoint>*)heap_caps_malloc(sizeof(std::vector<cv::KeyPoint>), MALLOC_CAP_SPIRAM  );
cv::Mat* prev_descriptors = (cv::Mat*)heap_caps_malloc(sizeof(cv::Mat), MALLOC_CAP_SPIRAM  );
cv::Mat* cur_descriptors = (cv::Mat*)heap_caps_malloc(sizeof(cv::Mat), MALLOC_CAP_SPIRAM  );
cv::Ptr<cv::ORB>* orb_detector = (cv::Ptr<cv::ORB>*)heap_caps_malloc(sizeof(cv::Ptr<cv::ORB>), MALLOC_CAP_SPIRAM  );
cv::Ptr<cv::DescriptorMatcher>* bfmatcher = (cv::Ptr<cv::DescriptorMatcher>*)heap_caps_malloc(sizeof(cv::Ptr<cv::DescriptorMatcher>), MALLOC_CAP_SPIRAM  );
cv::Mat* CameraMatrix = (cv::Mat*)heap_caps_malloc(sizeof(cv::Mat), MALLOC_CAP_SPIRAM  );
cv::Mat* DistCoeffs = (cv::Mat*)heap_caps_malloc(sizeof(cv::Mat), MALLOC_CAP_SPIRAM  );
cv::Mat* frame = (cv::Mat*)heap_caps_malloc(sizeof(cv::Mat), MALLOC_CAP_SPIRAM  );
std::vector<cv::Point2f>* cur_points = (std::vector<cv::Point2f>*)heap_caps_malloc(sizeof(std::vector<cv::Point2f>), MALLOC_CAP_SPIRAM  );

void init_opencv_features2d()
{
    printf("OpenCV info print: %s", cv::getBuildInformation().c_str());
    *orb_detector = cv::ORB::create();
    (*orb_detector)->setFastThreshold(29);
    (*orb_detector)->setNLevels(1);
    (*orb_detector)->setMaxFeatures(1);
    (*orb_detector)->setScaleFactor(1.2);
    (*orb_detector)->setEdgeThreshold(35);
    (*orb_detector)->setScoreType(cv::ORB::ScoreType::FAST_SCORE);
    (*orb_detector)->setFirstLevel(0);
    *bfmatcher = cv::BFMatcher::create(cv::NORM_HAMMING);

}

void detect_features(const uint8_t* data, int width, int height, bool processing)
{
    frame =  new cv::Mat(height, width, CV_8UC3, (void*)data);
    (*orb_detector)->detect(*frame, *cur_keypoints);
    (*orb_detector)->compute(*frame, *cur_keypoints, *cur_descriptors);
    
    cv::KeyPoint::convert(*cur_keypoints, *cur_points);
    cv::undistortPoints(*cur_points, *cur_points, *CameraMatrix, *DistCoeffs);
    // free(cur_points);
    // free(cur_keypoints);
    // cv::Mat rawData(height, width, CV_8UC1, (void*)data, width);
    // cv::Mat newData;

    // orb_detector->detect(rawData, *cur_keypoints);
    // orb_detector->compute(rawData, *cur_keypoints, cur_descriptors);

    // std::vector<cv::Point2f>* cur_points = new std::vector<cv::Point2f>;
    // std::vector<cv::KeyPoint>* cur_undistorted_keypoints = new std::vector<cv::KeyPoint>;

    // // Convert keypoints to vector of Point2f
    // cv::KeyPoint::convert(*cur_keypoints, *cur_points);


    // // Undistort keypoints
    // cv::undistortPoints(*cur_points, *cur_points, CameraMatrix, DistCoeffs);

    // // Convert vector of Point2f back to keypoints
    // cv::KeyPoint::convert(*cur_points, *cur_undistorted_keypoints);


}

void parse_camera_matrix(const char* camera_matrix) {

    std::stringstream camera_matrix_string(camera_matrix);
    std::string value;
    std::string string1;
    std::string string2;
    std::vector<float> camera_matrix_values;
    std::vector<float> dist_coeffs_values;

    // while(std::getline(camera_matrix_string, value, ';')) {
    //     camera_matrix_values.push_back(std::stof(value));
    // }


    std::getline(camera_matrix_string, string1, '|');
    printf("%s\n", string1.c_str());
    std::stringstream string_test(string1);
    while(std::getline(string_test, value, ';')) {
        camera_matrix_values.push_back(std::stof(value));
    }

    std::getline(camera_matrix_string, string2, '|');
    printf("%s\n", string2.c_str());
    std::stringstream string_test2(string2);
    while(std::getline(string_test2, value, ';')) {
        dist_coeffs_values.push_back(std::stof(value));
    }

    printf("Camera matrix values\n");

    *CameraMatrix = cv::Mat(3, 3, CV_32FC1, camera_matrix_values.data());
    for(int i = 0; i < CameraMatrix->rows; i++) {
        for(int j = 0; j < CameraMatrix->cols; j++) {
            printf("| %f |", CameraMatrix->at<float>(i,j));
        }
        printf("\n");
    }

    printf("Distortion coefficients values\n");

    *DistCoeffs = cv::Mat(5, 1, CV_32FC1, dist_coeffs_values.data());
    for(int i = 0; i < dist_coeffs_values.size(); i++){
        printf("| %f |", dist_coeffs_values.at(i));
    }
    printf("\n");



}

int compute_position() {
    
    return 1;

}
 