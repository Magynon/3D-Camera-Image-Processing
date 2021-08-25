///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/************************************************************
** This sample demonstrates how to read a SVO video file. **
** We use OpenCV to display the video.					   **
*************************************************************/

// ZED include
#include <sl/Camera.hpp>

// Sample includes
#include <opencv2/opencv.hpp>
#include "utils.hpp"
#include "Filter.h"

// Using namespace
using namespace sl;
using namespace std;

void print(string msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, string msg_suffix = "");

int main(int argc, char **argv) {
    // Create ZED objects
    Camera zed;
    InitParameters init_parameters;
    init_parameters.input.setFromSVOFile("//path//to//yor//file");
    init_parameters.depth_mode = sl::DEPTH_MODE::PERFORMANCE;
    init_parameters.coordinate_units = UNIT::METER;

    // Open the camera
    auto returned_state = zed.open(init_parameters);
    if (returned_state != ERROR_CODE::SUCCESS) {
        print("Camera Open", returned_state, "Exit program.");
        return EXIT_FAILURE;
    }

    auto resolution = zed.getCameraInformation().camera_configuration.resolution;
    /*static Mat color_image(resolution, MAT_TYPE::U8_C4, MEM::CPU);
    static cv::Mat color_image_ocv = slMat2cvMat(color_image);

    static Mat depth_image(resolution, MAT_TYPE::U8_C4, MEM::CPU);
    static cv::Mat depth_image_ocv = slMat2cvMat(depth_image);

    static Mat grayscale_image(resolution, MAT_TYPE::U8_C1, MEM::CPU);
    static cv::Mat grayscale_image_ocv = slMat2cvMat(grayscale_image);

    static Mat depth_measure(resolution, MAT_TYPE::F32_C1, MEM::CPU);
    static cv::Mat depth_measure_ocv = slMat2cvMat(depth_measure);

    static cv::Mat edges_image_ocv = cv::Mat(resolution.height, resolution.width, CV_8U);

    static Mat normal_measure(resolution, MAT_TYPE::F32_C4, MEM::CPU);
    static cv::Mat normal_measure_ocv = slMat2cvMat(normal_measure);*/

    static Mat point_cloud(resolution, MAT_TYPE::F32_C4, MEM::CPU);
    static cv::Mat point_cloud_ocv = slMat2cvMat(point_cloud);

    static Mat normal_image(resolution, MAT_TYPE::U8_C4, MEM::CPU);
    static cv::Mat normal_image_ocv = slMat2cvMat(normal_image);

    /*static cv::Mat my_normal_measure_ocv = cv::Mat(resolution.height, resolution.width, CV_32FC4);
    static cv::Mat my_normal_image_ocv = cv::Mat(resolution.height, resolution.width, CV_8UC4);*/

    static cv::Mat segmentation_ocv = cv::Mat(resolution.height, resolution.width, CV_32FC4);

    /*static cv::Vec4b* colorData;
    static cv::Vec4b* depthData;
    static uchar* auxData0 = new uchar[resolution.width * resolution.height];
    static cv::Vec4b* auxData1 = new cv::Vec4b[resolution.width * resolution.height];
    static uchar* grayscaleData;
    static float* depthMeasure;
    static uchar* edgesData;*/

    //static cv::Vec4f* normalMeasureData;
    static cv::Vec4b* normalImageData;
    static cv::Vec4f* pointCloudData;
    //static cv::Vec4f* myNormalMeasureData;
    //static cv::Vec4b* myNormalImageData;
    static cv::Vec4f* mySegmentation;


    int svo_frame_rate = zed.getInitParameters().camera_fps;
    int nb_frames = zed.getSVONumberOfFrames();
    print("[Info] SVO contains " +to_string(nb_frames)+" frames");

    // Start SVO playback

     while (true) {
        returned_state = zed.grab();
        if (returned_state == ERROR_CODE::SUCCESS) {
            int svo_position = zed.getSVOPosition();
            // Get Zed images
            //zed.retrieveImage(depth_image, VIEW::DEPTH, MEM::CPU, resolution);
            //zed.retrieveImage(color_image, VIEW::LEFT, MEM::CPU, resolution);
            //zed.retrieveImage(grayscale_image, VIEW::LEFT_GRAY, MEM::CPU, resolution);
            //zed.retrieveMeasure(depth_measure, MEASURE::DEPTH, MEM::CPU, resolution);
            //zed.retrieveMeasure(normal_measure, MEASURE::NORMALS, MEM::CPU, resolution);
            zed.retrieveImage(normal_image , VIEW::NORMALS, MEM::CPU, resolution);
            zed.retrieveMeasure(point_cloud, MEASURE::XYZ, MEM::CPU, resolution);


            //colorData = (cv::Vec4b*)color_image_ocv.data;
            //depthData = (cv::Vec4b*)depth_image_ocv.data;
            //grayscaleData = (uchar*)grayscale_image_ocv.data;
            //depthMeasure = (float*)depth_measure_ocv.data;
            //edgesData = (uchar*)edges_image_ocv.data;
            //normalMeasureData = (cv::Vec4f*)normal_measure_ocv.data;
            normalImageData = (cv::Vec4b*)normal_image_ocv.data;
            pointCloudData = (cv::Vec4f*)point_cloud_ocv.data;
            //myNormalMeasureData = (cv::Vec4f*)my_normal_measure_ocv.data;
            //myNormalImageData = (cv::Vec4b*)my_normal_image_ocv.data;
            mySegmentation = (cv::Vec4f*)segmentation_ocv.data;


            //Filter::filterColor(colorData, resolution.width, resolution.height);
            //Filter::filterColor(color_image_ocv);
            //Filter::filterGrayscale(grayscaleData, auxData0, resolution.width, resolution.height);
            //Filter::filterDepthByDistance(depthData, auxData1, depthMeasure, resolution.width, resolution.height);
            //Filter::averageFiltering(colorData, auxData1, resolution.width, resolution.height);
            //Filter::orderedFiltering(grayscaleData, auxData0, resolution.width, resolution.height);
            //Filter::extractEdges(grayscaleData, edgesData, resolution.width, resolution.height, "prewitt");
            //Filter::computeNormals(pointCloudData, myNormalMeasureData, resolution.width, resolution.height);
            //Filter::extractEdgesFromNormals(normalMeasureData, edgesData, resolution.width, resolution.height);
            Filter::segmentation(normalImageData, pointCloudData, mySegmentation, resolution.width, resolution.height);

            // Display opencv images
            //cv::imshow("Color", color_image_ocv);
            //cv::imshow("Depth", depth_image_ocv);
            //cv::imshow("Grayscale", grayscale_image_ocv);
            //cv::imshow("Edges", edges_image_ocv);
            //cv::imshow("Normals Measure", normal_measure_ocv);
            //cv::imshow("Normals Image", normal_image_ocv);
            //cv::imshow("MyNormals Measure", my_normal_measure_ocv);
            //cv::imshow("MyNormals Image", my_normal_image_ocv);
            cv::imshow("seg image", segmentation_ocv);
            cv::waitKey(1);
            
            

            ProgressBar((float)(svo_position / (float)nb_frames), 30);
        }
        else if (returned_state == sl::ERROR_CODE::END_OF_SVOFILE_REACHED)
        {
            print("SVO end has been reached. Looping back to 0\n");
            zed.setSVOPosition(0);
        }
        else {
            print("Grab ZED : ", returned_state);
            break;
        }
     } 
    zed.close();
    return EXIT_SUCCESS;
}

void print(string msg_prefix, ERROR_CODE err_code, string msg_suffix) {
    cout <<"[Sample]";
    if (err_code != ERROR_CODE::SUCCESS)
        cout << "[Error] ";
    else
        cout<<" ";
    cout << msg_prefix << " ";
    if (err_code != ERROR_CODE::SUCCESS) {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
}
