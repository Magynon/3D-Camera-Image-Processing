#include "Filter.h"

Filter::Filter(){}

Filter::~Filter(){}

void Filter::filterColor(cv::Vec4b* colorData, int width, int height) {
	int offset = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cv::Vec4b color = colorData[offset];
			uchar gray = (color[0] + color[1] + color[2]) / 3;
			colorData[offset] = cv::Vec4b(gray, gray, gray, color[3]);
			offset++;

		}
	}
}

void Filter::filterColor(cv::Mat colorImage) {
	int width = colorImage.cols;
	int height = colorImage.rows;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cv::Vec4b color = colorImage.at<cv::Vec4b>(y, x);
			uchar gray = (color[0] + color[1] + color[2]) / 3;
			colorImage.at<cv::Vec4b>(y, x) = cv::Vec4b(gray, gray, gray, color[3]);
		}
	}
}

void Filter::filterGrayscale(uchar* grayscaleData, uchar* auxData, int width, int height) {

	int offset, offset_neighb;


	for (int y = 2; y < height - 2; y++)
	{
		for (int x = 2; x < width - 2; x++)
		{

			offset = y * width + x;
			int suma = 0;
			for (int j = y - 2; j <= y + 2; j++)
			{
				for (int i = x - 2; i <= x + 2; i++)
				{
					offset_neighb = j * width + i;
					suma += grayscaleData[offset_neighb];
				}
			}
			suma /= 25;
			auxData[offset] = (uchar)suma;  //calculeaza rezultatul filtrat intr-un array auxiliar
		}
	}

	//dupa ce ai terminat cu toata filtrarea, copiaza inapoi in grayscaleData rezultatele filtrate
	for (int y = 2; y < height - 2; y++)
	{
		for (int x = 2; x < width - 2; x++)
		{
			offset = y * width + x;
			grayscaleData[offset] = auxData[offset];
		}
	}

}

// Filtrare ordonata (filtru median) intr-o vecinatate 3x3
void Filter::orderedFiltering(uchar* grayscaleData, uchar* auxData, int width, int height)
{
	int offset = 0, neigh[9], count, tmp;

	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			count = 0;

			for (int i = 0; i < 9; i++) {
				neigh[i] = 0;
			}

			for (int j = y - 1; j <= y + 1; j++)
			{
				for (int i = x - 1; i <= x + 1; i++)
				{
					neigh[count++] = grayscaleData[offset];
				}
			}

			// order ascending
			for (int i = 0; i < 8; i++) {
				for (int j = i + 1; j < 9; j++) {
					if (neigh[i] > neigh[j]) {
						tmp = neigh[i];
						neigh[i] = neigh[j];
						neigh[j] = tmp;
					}
				}
			}
			auxData[offset++] = neigh[5];
		}
	}

	//dupa ce ai terminat cu toata filtrarea, copiaza inapoi in grayscaleData rezultatele filtrate
	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			offset = y * width + x;
			grayscaleData[offset] = auxData[offset];
		}
	}
}

// Filtrare medie (medie aritmetica) pe imaginea color
void Filter::averageFiltering(cv::Vec4b* colorData, cv::Vec4b* auxData, int width, int height)
{
	float sumB, sumG, sumR;
	int offset = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (y > 0 && y < height - 1 && x > 0 && x < width - 1) {
				sumB = 0;
				sumG = 0;
				sumR = 0;
				for (int j = y - 1; j <= y + 1; j++)
				{
					for (int i = x - 1; i <= x + 1; i++)
					{
						sumB += colorData[j * width + i][0];
						sumG += colorData[j * width + i][1];
						sumR += colorData[j * width + i][2];
					}
				}
				sumB /= 9;
				sumG /= 9;
				sumR /= 9;
				auxData[offset] = cv::Vec4b(sumB, sumG, sumR, 1);
			}
			else {
				auxData[offset] = colorData[offset];
			}
			offset++;
		}
	}

	//dupa ce ai terminat cu toata filtrarea, copiaza inapoi rezultatele filtrate
	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			offset = y * width + x;
			colorData[offset] = auxData[offset];
		}
	}
}

// Filtrare Gaussiana adaptiva pentru imaginea de adancime
void Filter::filterDepthByDistance(cv::Vec4b* depthData, cv::Vec4b* auxData, float* depthMeasure, int width, int height)
{
	int offset = 0, counter = 0;
	float sum = 0;
	int threeByThree[9] = {
		1, 2, 1,
		2, 4, 2,
		1, 2, 1
	};
	int fiveByFive[25] = {
		1, 4, 7, 4, 1,
		4, 16, 26, 16, 4,
		7, 26, 41, 26, 7,
		4, 16, 26, 16, 4,
		1, 4, 7, 4, 1
	};
	int sevenBySeven[49] = {
		0, 0, 1, 2, 1, 0, 0,
		0, 3, 13, 22, 13, 3, 0,
		1, 13, 59, 97, 59, 13, 1,
		2, 22, 97, 159, 97, 22, 2,
		1, 13, 59, 97, 59, 13, 1,
		0, 3, 13, 22, 13, 3, 0,
		0, 0, 1, 2, 1, 0, 0
	};
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			counter = 0;
			sum = 0;

			if (depthMeasure[offset] < 2)
			{
				//filtrare 3x3 

				
				if (y > 0 && y < height - 1 && x > 0 && x < width - 1) {
					for (int j = y - 1; j <= y + 1; j++)
					{
						for (int i = x - 1; i <= x + 1; i++)
						{
							sum += threeByThree[counter++] * depthData[j * width + i][0];
						}
					}
					sum /= 16;
					auxData[offset] = cv::Vec4b(sum, sum, sum, 1);
				}
				else {
					auxData[offset] = depthData[offset];
				}
				
				
			}
			else
				if (depthMeasure[offset] < 5)
				{
					//filtrare 5x5

					
					if (y > 1 && y < height - 2 && x > 1 && x < width - 2) {
						for (int j = y - 2; j <= y + 2; j++)
						{
							for (int i = x - 2; i <= x + 2; i++)
							{
								sum += fiveByFive[counter++] * depthData[j * width + i][0];
							}
						}
						sum /= 273;
						auxData[offset] = cv::Vec4b(sum, sum, sum, 1);
					}
					else {
						auxData[offset] = depthData[offset];
					}
					
				}
				else
				{
					//filtrare 7x7

					
					if (y > 2 && y < height - 3 && x > 2 && x < width - 3) {
						for (int j = y - 3; j <= y + 3; j++)
						{
							for (int i = x - 3; i <= x + 3; i++)
							{
								sum += sevenBySeven[counter++] * depthData[j * width + i][0];
							}
						}
						sum /= 1003;
						auxData[offset] = cv::Vec4b(sum, sum, sum, 1);
					}
					else {
						auxData[offset] = depthData[offset];
					}
					
				}
			offset++;

		}
	}
	//dupa ce ai terminat cu toata filtrarea, copiaza inapoi rezultatele filtrate
	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			offset = y * width + x;
			depthData[offset] = auxData[offset];
		}
	}
}

// Extractii de muchii din img grayscale 
void Filter::extractEdges(uchar* grayscaleData, uchar* edgesData, int width, int height, char method[]) {
	int offset = 0, valx, valy, count, finalVal;
	int dx[9] = {
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
	};

	int dy[9] = {
		1, 2, 1,
		0, 0, 0,
		-1, -2, -1
	};

	if (strcmp(method, "sobel") == 0) {
		int dx[9] = {
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
		};

		int dy[9] = {
			1, 2, 1,
			0, 0, 0,
			-1, -2, -1
		};
	}
	else if (strcmp(method, "prewitt") == 0) {
		int dx[9] = {
		1, 0, -1,
		1, 0, -1,
		1, 0, -1
		};

		int dy[9] = {
			1, 1, 1,
			0, 0, 0,
			-1, -1, -1
		};
	}
	else if (strcmp(method, "roberts") == 0) {
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (y < height - 1 && x < width - 1)
				{
					edgesData[offset] = abs(grayscaleData[offset] - grayscaleData[offset + width + 1])
						+ abs(grayscaleData[offset + 1] - grayscaleData[offset + width - 1]);
				}
				else {
					edgesData[offset] = grayscaleData[offset];
				}
				offset++;
			}
		}
		return;
	}
	// sobel is default
	else {
		int dx[9] = {
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
		};

		int dy[9] = {
			1, 2, 1,
			0, 0, 0,
			-1, -2, -1
		};
	}
	

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{

			// if pixel not on edge
			if (y > 0 && y < height - 1 && x > 0 && x < width - 1) {
				valx = 0;
				valy = 0;
				count = 0;

				for (int j = y - 1; j <= y + 1; j++)
				{
					for (int i = x - 1; i <= x + 1; i++)
					{
						valx += dx[count] * grayscaleData[j * width + i];
						valy += dy[count++] * grayscaleData[j * width + i];
					}
				}

				finalVal = abs(valx) + abs(valy);
				// finalVal = sqrt(valx * valx + valy * valy);
				edgesData[offset] = finalVal;
			}
			else {
				edgesData[offset] = grayscaleData[offset];
			}
			offset++;
		}
	}

}

// Compute normals with 5x5 surrounding reference
void Filter::computeNormals(cv::Vec4f* pointCloudData, cv::Vec4f* myNormalMeasureData, int width, int height)
{
	int offset;
	cv::Vec4f ox_up1, ox_up2, ox_up3;
	cv::Vec4f ox_dw1, ox_dw2, ox_dw3;
	cv::Vec4f oy_l1, oy_l2, oy_l3;
	cv::Vec4f oy_r1, oy_r2, oy_r3;
	cv::Vec4f corner_up_l, corner_up_r, corner_down_l, corner_down_r;
	glm::vec3 oxUpAvg, oxDwAvg, oyLAvg, oyRAvg;
	glm::vec3 horVector, vertVector, normal;

	for (int y = 2; y < height - 2; y++) {
		for (int x = 2; x < width - 2; x++) {
			offset = y * width + x;

			corner_up_l = pointCloudData[offset - 2 * width - 2];
			corner_up_r = pointCloudData[offset - 2 * width + 2];
			corner_down_l = pointCloudData[offset + 2 * width - 2];
			corner_down_r = pointCloudData[offset + 2 * width + 2];

			ox_up1 = pointCloudData[offset - 2 * width - 1];
			ox_up2 = pointCloudData[offset - 2 * width];
			ox_up3 = pointCloudData[offset - 2 * width + 1];
			oxUpAvg = glm::vec3((corner_up_l[0] + ox_up1[0] + ox_up2[0] + ox_up3[0] + corner_up_r[0]) / 5.f,
				(corner_up_l[1] + ox_up1[1] + ox_up2[1] + ox_up3[1] + corner_up_r[1]) / 5.f,
				(corner_up_l[2] + ox_up1[2] + ox_up2[2] + ox_up3[2] + corner_up_r[2]) / 5.f);

			ox_dw1 = pointCloudData[offset + 2 * width - 1];
			ox_dw2 = pointCloudData[offset + 2 * width];
			ox_dw3 = pointCloudData[offset + 2 * width + 1];
			oxDwAvg = glm::vec3((corner_down_l[0] + ox_dw1[0] + ox_dw2[0] + ox_dw3[0] + corner_down_r[0]) / 5.f,
				(corner_down_l[1] + ox_dw1[1] + ox_dw2[1] + ox_dw3[1] + corner_down_r[1]) / 5.f,
				(corner_down_l[2] + ox_dw1[2] + ox_dw2[2] + ox_dw3[2] + corner_down_r[2]) / 5.f);

			oy_l1 = pointCloudData[offset - width - 2];
			oy_l2 = pointCloudData[offset - 2];
			oy_l3 = pointCloudData[offset + width - 2];
			oyLAvg = glm::vec3((corner_up_l[0] + oy_l1[0] + oy_l2[0] + oy_l3[0] + corner_down_l[0]) / 5.f,
				(corner_up_l[1] + oy_l1[1] + oy_l2[1] + oy_l3[1] + corner_down_l[1]) / 5.f,
				(corner_up_l[2] + oy_l1[2] + oy_l2[2] + oy_l3[2] + corner_down_l[2]) / 5.f);

			oy_r1 = pointCloudData[offset - width + 2];
			oy_r2 = pointCloudData[offset + 2];
			oy_r3 = pointCloudData[offset + width + 2];
			oyRAvg = glm::vec3((corner_up_r[0] + oy_r1[0] + oy_r2[0] + oy_r3[0] + corner_down_r[0]) / 5.f,
				(corner_up_r[1] + oy_r1[1] + oy_r2[1] + oy_r3[1] + corner_down_r[1]) / 5.f,
				(corner_up_r[2] + oy_r1[2] + oy_r2[2] + oy_r3[2] + corner_down_r[2]) / 5.f);

			horVector = oyRAvg - oyLAvg;
			vertVector = oxUpAvg - oxDwAvg;
			normal = glm::cross(horVector, vertVector);

			if (glm::length(normal) > 0.0001) {
				normal = glm::normalize(normal);
			}

			myNormalMeasureData[offset] = cv::Vec4f(normal.x, normal.y, normal.z, 1);
		}
	}
}

// Extractie muchii din img normale
void Filter::extractEdgesFromNormals(cv::Vec4f* myNormalMeasureData, uchar* normalEdges, int width, int height)
{
	int offset;
	glm::vec3 normals[3][3];
	float hor, ver;

	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			offset = y * width + x;

			// copiez normalele intr-un array pt. ease of use
			for (int j = y - 1; j <= y + 1; j++) {
				for (int i = x - 1; i <= x + 1; i++) {
					normals[j - y + 1][i - x + 1] = glm::vec3(myNormalMeasureData[j*width + i][0], myNormalMeasureData[j * width + i][1], myNormalMeasureData[j * width + i][2]);

					// normez, daca este cazul
					if (glm::length(normals[j - y + 1][i - x + 1]) > 0.0001) {
						normals[j - y + 1][i - x + 1] = glm::normalize(normals[j - y + 1][i - x + 1]);
					}
				}
			}

			// vertical
			ver = 200*(1 - glm::dot(normals[0][0], normals[0][2])) +
				2 * (1 - glm::dot(normals[1][0], normals[1][2])) +
				(1 - glm::dot(normals[2][0], normals[2][2]));

			// horizontal
			hor = 200*(1 - glm::dot(normals[0][0], normals[2][0])) +
				2 * (1 - glm::dot(normals[0][1], normals[2][1])) +
				(1 - glm::dot(normals[0][2], normals[2][2]));

			normalEdges[offset] = abs(hor) + abs(ver);
		}
	}
}

//  --------------
// | SEGMENTATION |
//  --------------

float thresh = 0.5; // can be tweaked
float w1 = 1, w2 = 1; // can be tweaked

typedef struct Node {
	int val;
	Node* next;
} Node;

typedef struct {
	glm::vec3 avgNormal;
	glm::vec3 avgPos;
	int noOfPixels, id, idPostMerge;
	Node* adjList, *endOfList;
} Surface;

typedef struct {
	int noOfSurfaces;
	Surface** array;
} SurfaceArray;

// free alloc'ed memory
void freeEverything(SurfaceArray* surfaceArray, int* surfaceMatrix) {
	Node* tmp, *tmp1;
	for (int i = 0; i < surfaceArray->noOfSurfaces; i++) {
		tmp = surfaceArray->array[i]->adjList;
		while (tmp) {
			tmp1 = tmp;
			tmp = tmp->next;
			free(tmp1);
		}
		free(surfaceArray->array[i]);
	}
	free(surfaceArray->array);
	free(surfaceArray);
	free(surfaceMatrix);
}

// calculate minimum of 4 numbers
float minimum4(float a, float b, float c, float d) {
	float min1 = std::min(a, b);
	float min2 = std::min(c, d);
	return std::min(min1, min2);
}

// calculate minimum of 3 numbers
float minimum3(float a, float b, float c) {
	float min = std::min(a, b);
	return std::min(min, c);
}

// calculate normal cost
float normalCost(cv::Vec4b pixelNormalData, Surface* surface) {
	// printf("Surface currently compared: %d\n", surface->id);

	glm::vec3 pixelNormal = glm::vec3(pixelNormalData[0], pixelNormalData[1], pixelNormalData[2]);
	// printf("pixelNormal: x - %f, y - %f, z - %f\n", pixelNormal[0], pixelNormal[1], pixelNormal[2]);

	glm::vec3 surfaceNormal = glm::vec3(surface->avgNormal[0], surface->avgNormal[1], surface->avgNormal[2]);
	// printf("surfaceNormal: x - %f, y - %f, z - %f\n", surfaceNormal[0], surfaceNormal[1], surfaceNormal[2]);

	// printf("Normal: %f;\n", (1 - glm::dot(pixelNormal, surfaceNormal)));

	return (1 - glm::dot(pixelNormal, surfaceNormal));
}

// calculate position cost
float positionCost(cv::Vec4b pixelCloudData, cv::Vec4b neighCloudData) {
	glm::vec3 pixelPos = glm::vec3(pixelCloudData[0], pixelCloudData[1], pixelCloudData[2]);
	// printf("pixelPos: x - %f, y - %f, z - %f\n", pixelPos[0], pixelPos[1], pixelPos[2]);

	glm::vec3 neighPos = glm::vec3(neighCloudData[0], neighCloudData[1], neighCloudData[2]);
	// printf("neighPos: x - %f, y - %f, z - %f\n", neighPos[0], neighPos[1], neighPos[2]);

	// printf("Position: %f;\n", glm::distance(pixelPos, neighPos));

	return glm::distance(pixelPos, neighPos);
}

// initialize empty surface array
SurfaceArray* initSurfaceArray() {
	SurfaceArray* surfaceArray = (SurfaceArray*)malloc(sizeof(SurfaceArray));
	if (surfaceArray != NULL) {
		surfaceArray->noOfSurfaces = 0;
		surfaceArray->array = (Surface**)malloc(0);
	} 
	else {
		// printf("Error alloc'ing mem for surface array\n");
	}
	return surfaceArray;
}

// add empty surface to surface array
void createNewSurface(SurfaceArray** surfaceArray) {

	// increase number of surfaces in the array
	(*surfaceArray)->noOfSurfaces++;

	Surface** temp1 = (Surface**)realloc((*surfaceArray)->array, (*surfaceArray)->noOfSurfaces * sizeof(Surface*));
	Surface* temp2 = (Surface*)malloc(sizeof(Surface));

	if (temp1 != NULL) {
		// realloc the array, add space for one more surface
		(*surfaceArray)->array = temp1;

		if (temp2 != NULL) {
			// alloc space for one more surface
			temp2->avgNormal = glm::vec3(0, 0, 0);
			temp2->avgPos = glm::vec3(0, 0, 0);
			temp2->noOfPixels = 0;
			temp2->id = (*surfaceArray)->noOfSurfaces - 1;
			temp2->idPostMerge = temp2->id;
			temp2->adjList = (Node*)malloc(sizeof(Node));
			temp2->adjList->next = NULL;
			temp2->endOfList = temp2->adjList;
			(*surfaceArray)->array[(*surfaceArray)->noOfSurfaces - 1] = temp2;
		}
		else {
			// printf("Error alloc'ing mem for surface\n");
		}
	}
	else {
		// printf("Error realloc'ing mem for surface array\n");
	}
}

// initialize pixel-surface matrix with -1
int* initSurfaceMatrix(int width, int height) {
	int* surfaceMatrix = (int*)malloc((unsigned long long)(width * height) * sizeof(int));

	if (surfaceMatrix != NULL) {
		for (int i = 0; i < width * height; i++) {
			surfaceMatrix[i] = -1;
		}
	}
	else {
		// printf("Error alloc'ing mem for surface matrix\n");
	}
	return surfaceMatrix;
	
}

int alreadyInAdjList(Node *adjList, int val) {
	Node* tmp = adjList;
	while (tmp) {
		if (val == tmp->val) {
			return 1;
		}
		tmp = tmp->next;
	}
	return 0;
}

Node* newNode(int val) {
	Node* tmp = (Node*)malloc(sizeof(Node));
	tmp->next = NULL;
	tmp->val = val;
	return tmp;
}

// add pixel to surface
void addPixelToSurface(int *hashOfPixel, int indexOfSurface, SurfaceArray* surfaceArray, cv::Vec4b pixelNormalData, cv::Vec4f pixelCloudData) {
	Surface* surface = surfaceArray->array[indexOfSurface];
	glm::vec3 pixelNormal = glm::vec3(pixelNormalData[0], pixelNormalData[1], pixelNormalData[2]);
	glm::vec3 pixelPosition = glm::vec3(pixelCloudData[0], pixelCloudData[1], pixelCloudData[2]);

	// update pixel-surface matrix with corresponding surface index
	*hashOfPixel = indexOfSurface;

	// calculate new average normal for the current surface
	surface->avgNormal = (surface->avgNormal * (float)(surface->noOfPixels) + pixelNormal) / (float)(surface->noOfPixels + 1);

	// check if new normal is below normalized
	if (glm::length(surface->avgNormal) > 0.00001) {
		surface->avgNormal = glm::normalize(surface->avgNormal);
	}

	// calculate new average position for the current surface
	surface->avgPos = (surface->avgPos * (float)(surface->noOfPixels) + pixelPosition) / (float)(surface->noOfPixels + 1);

	// increase pixel count for the current surface
	surface->noOfPixels++;
}

// find correct surface to add pixel to
void findTheRightSurfaceAndAddPixel(int* surfaceMatrix, int offset, SurfaceArray* surfaceArray, cv::Vec4b* normalData, cv::Vec4f* cloudData, int width, int height) {
	cv::Vec4b pixelNormalData = normalData[offset];
	cv::Vec4b pixelCloudData = cloudData[offset];
	Node* adjList;

	// the very first pixel has already been added
	if (offset == 0) {
		return;
	}

	// if pixel is in first row
	else if (offset < width) {
		float min_cost;

		// calculate costs with neighbors in a breadth first manner
		min_cost = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - 1]]) + w2 * positionCost(pixelCloudData, cloudData[offset - 1]);

		// printf("Cost min: %f;\n", min_cost);

		// check if min_cost is bigger then threshold
		if (min_cost > thresh) {

			// if so, make new surface and add pixel to it
			createNewSurface(&surfaceArray);
			int newSurfaceIndex = surfaceArray->noOfSurfaces - 1;
			adjList = surfaceArray->array[newSurfaceIndex]->adjList;
			adjList->val = surfaceMatrix[offset - 1];
			surfaceArray->array[newSurfaceIndex - 1]->endOfList = newNode(newSurfaceIndex);
			addPixelToSurface(&surfaceMatrix[offset], surfaceArray->noOfSurfaces - 1, surfaceArray, pixelNormalData, pixelCloudData);
			// printf("Created new surface: %d\n", surfaceArray->noOfSurfaces - 1);
		}
		else {

			// else, add pixel to already existing surface
			addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - 1], surfaceArray, pixelNormalData, pixelCloudData);
			// printf("1 ROW Surface added to: %d\n", surfaceMatrix[offset - 1]);
			return;
		}
	}

	// if pixel is in first column
	else if (offset % width == 0) {
		float cost_up, cost_right_up, min_cost;


		// calculate costs with neighbors in a breadth first manner
		cost_up = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - width]]) + w2 * positionCost(pixelCloudData, cloudData[offset - width]);
		cost_right_up = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - width + 1]]) + w2 * positionCost(pixelCloudData, cloudData[offset - width + 1]);

		// calculate minimum cost
		min_cost = std::min(cost_up, cost_right_up);

		// printf("Cost up: %f; Cost right up: %f; Cost min: %f;\n", cost_up, cost_right_up, min_cost);

		// check if min_cost is bigger then threshold
		if (min_cost > thresh) {

			// if so, make new surface and add pixel to it
			createNewSurface(&surfaceArray);
			int newSurfaceIndex = surfaceArray->noOfSurfaces - 1;
			adjList = surfaceArray->array[newSurfaceIndex]->adjList;

			// initialize the head of the new surface's adjList
			adjList->val = surfaceMatrix[offset - width];
			surfaceArray->array[surfaceMatrix[offset - width]]->endOfList = newNode(newSurfaceIndex);

			if (!alreadyInAdjList(adjList, surfaceMatrix[offset - width + 1])) {
				adjList->next = newNode(surfaceMatrix[offset - width + 1]);
				surfaceArray->array[surfaceMatrix[offset - width + 1]]->endOfList = newNode(newSurfaceIndex);
			}
			
			addPixelToSurface(&surfaceMatrix[offset], surfaceArray->noOfSurfaces - 1, surfaceArray, pixelNormalData, pixelCloudData);
			// printf("Created new surface: %d\n", surfaceArray->noOfSurfaces - 1);
		}
		else {

			// else, add pixel to already existing surface
			if (min_cost == cost_up) {
				addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - width], surfaceArray, pixelNormalData, pixelCloudData);
				// printf("1 COL 1 Surface added to: %d\n", surfaceMatrix[offset - width]);
				return;
			}
			if (min_cost == cost_right_up) {
				addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - width + 1], surfaceArray, pixelNormalData, pixelCloudData);
				// printf("1 COL 2 Surface added to: %d\n", surfaceMatrix[offset - width + 1]);
				return;
			}
		}
	}

	// if pixel is in last column
	else if ((offset + 1) % width == 0) {
		float cost_left_mid, cost_left_up, cost_up, min_cost;


		// calculate costs with neighbors in a breadth first manner
		cost_left_mid = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - 1]]) + w2 * positionCost(pixelCloudData, cloudData[offset - 1]);
		cost_left_up = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - width - 1]]) + w2 * positionCost(pixelCloudData, cloudData[offset - width - 1]);
		cost_up = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - width]]) + w2 * positionCost(pixelCloudData, cloudData[offset - width]);

		// calculate minimum cost
		min_cost = minimum3(cost_left_mid, cost_left_up, cost_up);

		// printf("Cost up: %f; Cost left up: %f; Cost left mid: %f; Cost min: %f;\n", cost_up, cost_left_up, cost_left_mid, min_cost);

		// check if min_cost is bigger then threshold
		if (min_cost > thresh) {

			// if so, make new surface and add pixel to it
			createNewSurface(&surfaceArray);
			int newSurfaceIndex = surfaceArray->noOfSurfaces - 1;
			adjList = surfaceArray->array[newSurfaceIndex]->adjList;

			// initialize the head of the new surface's adjList
			adjList->val = surfaceMatrix[offset - 1];
			surfaceArray->array[surfaceMatrix[offset - 1]]->endOfList = newNode(newSurfaceIndex);

			if (!alreadyInAdjList(adjList, surfaceMatrix[offset - width - 1])) {
				adjList->next = newNode(surfaceMatrix[offset - width - 1]);
				surfaceArray->array[surfaceMatrix[offset - width - 1]]->endOfList = newNode(newSurfaceIndex);
			}
			if (!alreadyInAdjList(adjList, surfaceMatrix[offset - width])) {
				adjList->next = newNode(surfaceMatrix[offset - width]);
				surfaceArray->array[surfaceMatrix[offset - width]]->endOfList = newNode(newSurfaceIndex);
			}

			addPixelToSurface(&surfaceMatrix[offset], surfaceArray->noOfSurfaces - 1, surfaceArray, pixelNormalData, pixelCloudData);
			// printf("Created new surface: %d\n", surfaceArray->noOfSurfaces - 1);
		}
		else {

			// else, add pixel to already existing surface
			if (min_cost == cost_left_mid) {
				addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - 1], surfaceArray, pixelNormalData, pixelCloudData);
				// printf("LAST COL 1 Surface added to: %d\n", surfaceMatrix[offset - 1]);
				return;
			}
			if (min_cost == cost_left_up) {
				addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - width - 1], surfaceArray, pixelNormalData, pixelCloudData);
				// printf("LAST COL 2 Surface added to: %d\n", surfaceMatrix[offset - width - 1]);
				return;
			}
			if (min_cost == cost_up) {
				addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - width], surfaceArray, pixelNormalData, pixelCloudData);
				// printf("LAST COL 3 Surface added to: %d\n", surfaceMatrix[offset - width]);
				return;
			}
		}
	}

	// if pixel is normal
	else {
		float cost_left_mid, cost_left_up, cost_up, cost_right_up, min_cost;

		// calculate costs with neighbors in a breadth first manner
		cost_left_mid = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - 1]]) + w2 * positionCost(pixelCloudData, cloudData[offset - 1]);
		cost_left_up = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - width - 1]]) + w2 * positionCost(pixelCloudData, cloudData[offset - width - 1]);
		cost_up = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - width]]) + w2 * positionCost(pixelCloudData, cloudData[offset - width]);
		cost_right_up = w1 * normalCost(pixelNormalData, surfaceArray->array[surfaceMatrix[offset - width + 1]]) + w2 * positionCost(pixelCloudData, cloudData[offset - width + 1]);

		// calculate minimum cost
		min_cost = minimum4(cost_left_mid, cost_left_up, cost_up, cost_right_up);

		// printf("Cost up: %f; Cost right up: %f; Cost left up: %f; Cost left mid: %f; Cost min: %f;\n", cost_up, cost_right_up, cost_left_up, cost_left_mid, min_cost);

		// check if min_cost is bigger then threshold
		if (min_cost > thresh) {

			// if so, make new surface and add pixel to it
			createNewSurface(&surfaceArray);
			int newSurfaceIndex = surfaceArray->noOfSurfaces - 1;
			adjList = surfaceArray->array[newSurfaceIndex]->adjList;

			// initialize the head of the new surface's adjList
			adjList->val = surfaceMatrix[offset - 1];
			surfaceArray->array[surfaceMatrix[offset - 1]]->endOfList = newNode(newSurfaceIndex);

			if (!alreadyInAdjList(adjList, surfaceMatrix[offset - width - 1])) {
				adjList->next = newNode(surfaceMatrix[offset - width - 1]);
				surfaceArray->array[surfaceMatrix[offset - width - 1]]->endOfList = newNode(newSurfaceIndex);
			}
			if (!alreadyInAdjList(adjList, surfaceMatrix[offset - width])) {
				adjList->next = newNode(surfaceMatrix[offset - width]);
				surfaceArray->array[surfaceMatrix[offset - width]]->endOfList = newNode(newSurfaceIndex);
			}
			if (!alreadyInAdjList(adjList, surfaceMatrix[offset - width + 1])) {
				adjList->next = newNode(surfaceMatrix[offset - width + 1]);
				surfaceArray->array[surfaceMatrix[offset - width + 1]]->endOfList = newNode(newSurfaceIndex);
			}

			addPixelToSurface(&surfaceMatrix[offset], surfaceArray->noOfSurfaces - 1, surfaceArray, pixelNormalData, pixelCloudData);
			// printf("Created new surface: %d\n", surfaceArray->noOfSurfaces - 1);
		}
		else {

			// else, add pixel to already existing surface
			if (min_cost == cost_left_mid) {
				adjList = surfaceArray->array[surfaceMatrix[offset - 1]]->adjList;

				if (!alreadyInAdjList(adjList, surfaceMatrix[offset - width + 1])) {
					adjList->next = newNode(surfaceMatrix[offset - width + 1]);
					surfaceArray->array[surfaceMatrix[offset - width + 1]]->endOfList = newNode(surfaceMatrix[offset - 1]);
				}
				addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - 1], surfaceArray, pixelNormalData, pixelCloudData);
				// printf("NORMAL 1 Surface added to: %d\n", surfaceMatrix[offset - 1]);
				return;
			}
			if (min_cost == cost_left_up) {
				adjList = surfaceArray->array[surfaceMatrix[offset - width - 1]]->adjList;

				if (!alreadyInAdjList(adjList, surfaceMatrix[offset - width + 1])) {
					adjList->next = newNode(surfaceMatrix[offset - width + 1]);
					surfaceArray->array[surfaceMatrix[offset - width + 1]]->endOfList = newNode(surfaceMatrix[offset - width - 1]);
				}

				addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - width - 1], surfaceArray, pixelNormalData, pixelCloudData);
				// printf("NORMAL 2 Surface added to: %d\n", surfaceMatrix[offset - width - 1]);
				return;
			}
			if (min_cost == cost_up) {
				addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - width], surfaceArray, pixelNormalData, pixelCloudData);
				// printf("NORMAL 3 Surface added to: %d\n", surfaceMatrix[offset - width]);
				return;
			}
			if (min_cost == cost_right_up) {
				adjList = surfaceArray->array[surfaceMatrix[offset - width + 1]]->adjList;

				if (!alreadyInAdjList(adjList, surfaceMatrix[offset - width - 1])) {
					adjList->next = newNode(surfaceMatrix[offset - width - 1]);
					surfaceArray->array[surfaceMatrix[offset - width - 1]]->endOfList = newNode(surfaceMatrix[offset - width + 1]);
				}
				if (!alreadyInAdjList(adjList, surfaceMatrix[offset - 1])) {
					adjList->next = newNode(surfaceMatrix[offset - 1]);
					surfaceArray->array[surfaceMatrix[offset - 1]]->endOfList = newNode(surfaceMatrix[offset - width + 1]);
				}

				addPixelToSurface(&surfaceMatrix[offset], surfaceMatrix[offset - width + 1], surfaceArray, pixelNormalData, pixelCloudData);
				// printf("NORMAL 4 Surface added to: %d\n", surfaceMatrix[offset - width + 1]);
				return;
			}
		}
	}
	
}

// merge similar looking surfaces
void mergeSimilarSurfaces(SurfaceArray* surfaceArray, int* surfaceMatrix) {
	float distance1, distance2, distAbsDiff, cos;
	float norm1X, norm1Y, norm1Z, norm2X, norm2Y, norm2Z;
	float point1X, point1Y, point1Z, point2X, point2Y, point2Z;
	Surface* surface1, *surface2;
	Node* cursor;

	// go through each surface
	for (int i = 0; i < surfaceArray->noOfSurfaces; i++) {
		surface1 = surfaceArray->array[surfaceMatrix[i]];

		// find surface1's normal
		norm1X = surface1->avgNormal[0]; norm1Y = surface1->avgNormal[1]; norm1Z = surface1->avgNormal[2];

		// find surface1's avg point
		point1X = surface1->avgPos[0]; point1Y = surface1->avgPos[1]; point1Z = surface1->avgPos[2];

		// skip surface's 0 first neighbour because its empty
		if (i == 0) {
			cursor = surface1->adjList->next;
		}

		// iterate through surface1's neighbours
		while (cursor) {
			surface2 = surfaceArray->array[cursor->val];
			// find surface2's normal
			norm2X = surface2->avgNormal[0]; norm2Y = surface2->avgNormal[1]; norm2Z = surface2->avgNormal[2];

			// find surface2's avg point
			point2X = surface2->avgPos[0]; point2Y = surface2->avgPos[1]; point2Z = surface2->avgPos[2];

			distance1 = abs(-norm1X * point1X - norm1Y * point1Y - norm1Z * point1Z) / sqrt(norm1X * norm1X + norm1Y * norm1Y + norm1Z * norm1Z);
			distance2 = abs(-norm2X * point2X - norm2Y * point2Y - norm2Z * point2Z) / sqrt(norm2X * norm2X + norm2Y * norm2Y + norm2Z * norm2Z);
			distAbsDiff = abs(distance1 - distance2);
			cos = glm::dot(surface1->avgNormal, surface2->avgNormal);

			// check if surfaces are identical
			if (distAbsDiff < 0.000000000001 && cos > 0.9999999999999) {
				//printf("OFFSET: %d, %d | i: %d, %d | dist1 - dist2 = %f; cos = %f\n", offset, surfaceMatrix[offset], i, surfaceMatrix[i], abs(distance1 - distance2), glm::dot(surface1->avgNormal, surface2->avgNormal));
				surface2->idPostMerge = surface1->idPostMerge;
			}

			cursor = cursor->next;
		}
	}
}

// main seg function
void Filter::segmentation(cv::Vec4b* normalData, cv::Vec4f* cloudData, cv::Vec4f* auxData, int width, int height) {
	// printf("Resolution is %d by %d\n", width, height);

	int offset, id;
	SurfaceArray* surfaceArray = initSurfaceArray();
	int* surfaceMatrix = initSurfaceMatrix(width, height);
	
	// add the first pixel to the surface
	createNewSurface(&surfaceArray);
	addPixelToSurface(&surfaceMatrix[0], 0, surfaceArray, normalData[0], cloudData[0]);

	// segmentation process
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			offset = j * width + i;
			// printf("\n\nStopped at %d pixel\n", offset);
			findTheRightSurfaceAndAddPixel(surfaceMatrix, offset, surfaceArray, normalData, cloudData, width, height);
		}
	}

	mergeSimilarSurfaces(surfaceArray, surfaceMatrix);

	Surface* surface;

	// copy data to auxData
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			offset = j * width + i;
			id = surfaceArray->array[surfaceMatrix[offset]]->idPostMerge;
			surface = surfaceArray->array[id];
			auxData[offset] = cv::Vec4f(surface->avgNormal[0], surface->avgNormal[1], surface->avgNormal[2], 1);
		}
	}
	freeEverything(surfaceArray, surfaceMatrix);
}
