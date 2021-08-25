#pragma once
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>

class Filter {
public:
	Filter();
	~Filter();

	static void filterColor(cv::Vec4b* colorData, int width, int height);
	static void filterColor(cv::Mat colorImage);


	static void filterGrayscale(uchar* grayscaleData, uchar* auxData, int width, int height);
	static void filterDepthByDistance(cv::Vec4b* depthData, cv::Vec4b* auxData, float* depthMeasure, int width, int height);
	static void averageFiltering(cv::Vec4b* colorData, cv::Vec4b* auxData, int width, int height);
	static void orderedFiltering(uchar* grayscaleData, uchar* auxData, int width, int height);

	static void extractEdges(uchar* grayscaleData, uchar* edgesData, int width, int height, char method[]);
	static void Filter::computeNormals(cv::Vec4f* pointCloudData, cv::Vec4f* myNormalMeasureData, int width, int height);
	static void Filter::extractEdgesFromNormals(cv::Vec4f* myNormalMeasureData, uchar* normalEdges, int width, int height);
	static void Filter::segmentation(cv::Vec4b* normalData, cv::Vec4f* pointCloudData, cv::Vec4f* auxData, int width, int height);
};