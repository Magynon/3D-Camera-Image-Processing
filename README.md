# 3D-Camera-Image-Processing

This project works with videos recorded by stereo ZED 3D cameras and it implements basic functionalities such as calculating normal map and comparing it to the default ZED SDK normal map, computing various blur methods or segmenting a given image into surfaces.

The project is split in 2 parts:
- image surface segmentation algorithm
- basic image processing functions

## **Image surface segmentation**

This algorithm uses low-level operations working with **point cloud data** and **normal vectors**. The traversal is done in a *breadth-first manner*. Each pixel's properties (physical distance and normal vector) are checked with its neighbours and then the pixel is either added to an already existing neighbouring surface or a new surface is made. <br>
After this process, there will be surfaces which although bearing many similarities, are still perceived as different. The solution is to:
- remember a surface's neighbours and at the end of the algorithm, 
- go through the surface array,
- analyse the similarities between the current element and its vicinity,
- merge surfaces where necessary.

**Thresholds** (maximum angle between normal vectors in order to be considered identical, maximum distance between surfaces, weights) **are to be adjusted by the user in order to get the best results.**
## **Basic image processing**
Several functions have been implemented:

### Blurs
- Gaussian masks (3x3, 5x5, 7x7)
- adaptive Gaussian masks
- average blur
- ordered blur

### Edge detection
- Sobel
- Prewitt
- Roberts

### Normal vector computing
- using a 5x5 mask
- computing normal deviations (using the dot product)
