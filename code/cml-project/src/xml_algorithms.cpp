
#define _USE_MATH_DEFINES

#include "ofMain.h"
#include "opencv2/highgui.hpp";
#include "ofxCv.h";
#include "xml_algorithms.h";

using namespace cv;
using namespace ofxCv;


int* xml_algorithms::getColor(ofImage img){
	ofPixels pixels = img.getPixels();

	int r = 0, g = 0, b = 0, l = 0;
	for (int x = 0; x < pixels.getWidth(); x++) {
		for (int y = 0; y < pixels.getHeight(); y++) {
			ofColor color = pixels.getColor(x, y);

			r += color.r;
			g += color.g;
			b += color.b;
		}
	}

	int pixels_size = pixels.getWidth() * pixels.getHeight();

	 avg_l = round((0.2125 * r + 0.7154 * g + 0.0721 * b) / pixels_size);

	 avgColor[0] = round(r/ pixels_size);
	 avgColor[1] = round(g / pixels_size);
	 avgColor[2] = round(b / pixels_size);

	return avgColor;
}

int xml_algorithms::getLuminance() {
	return avg_l;
}


void xml_algorithms::setFilter(ofImage img, bool edgesFilter) {
	Mat mat, output;
	vector<Mat> destArray;

	ofImage imageOf1, imageOf2, imageOf3, imageOf4, imageOf5, imageOf6;

	imageOf1.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_GRAYSCALE);
	imageOf2.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_GRAYSCALE);
	imageOf3.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_GRAYSCALE);
	imageOf4.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_GRAYSCALE);
	imageOf5.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_GRAYSCALE);
	imageOf6.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_GRAYSCALE);


	mat = toCv(img);

	if(edgesFilter) {
	double filter1[2][2] = { -1, 1, -1 , 1 };
	double filter2[2][2] = { -1, -1, 1 , 1 };
	double filter3[2][2] = { sqrt(2), 0, 0, -sqrt(2) };
	double filter4[2][2] = { 0, sqrt(2), -sqrt(2) , 0 };
	double filter5[2][2] = { 2, -2, -2,2 };
	

	Mat kernel1(2, 2, CV_64F, filter1);
	Mat kernel2(2, 2, CV_64F, filter2);
	Mat kernel3(2, 2, CV_64F, filter3);
	Mat kernel4(2, 2, CV_64F, filter4);
	Mat kernel5(2, 2, CV_64F, filter5);

	for (int i = 0; i < NUM_EDGES_IMAGES;i++) {
		filter2D(mat, output, -1, kernel1, Point(-1, -1), 0.0, BORDER_DEFAULT);

		destArray.push_back(output);
	}

	}
	else {
		int kernel_size = 15;
		Size k_size = Size(kernel_size, kernel_size);

		int frequency = 4;
		double theta[] = { 0,30 * M_PI / 180,60 * M_PI / 180,90 * M_PI / 180,120 * M_PI / 180,150 * M_PI / 180 };
		double sigma = 5;
		double phase = 0;
		//18 ou 24 filtros

		for (int j = 0; j < NUM_GABOR_IMAGES; j++)
		{
			Mat kernel1;
			Mat dest;
			kernel1 = getGaborKernel(k_size, sigma, theta[j], frequency, phase, CV_64F);
			filter2D(mat, dest, -1, kernel1, Point(-1, -1), 0, BORDER_DEFAULT);

			destArray.push_back(dest);

		}
		toOf(destArray[5], imageOf6);
		imageOf6.setFromPixels(destArray[5].data, destArray[5].cols, destArray[5].rows, OF_IMAGE_GRAYSCALE);

	}
	toOf(destArray[0], imageOf1);
	toOf(destArray[1], imageOf2);
	toOf(destArray[2], imageOf3);
	toOf(destArray[3], imageOf4);
	toOf(destArray[4], imageOf5);


	imageOf1.setFromPixels(destArray[0].data, destArray[0].cols, destArray[0].rows, OF_IMAGE_GRAYSCALE);
	imageOf2.setFromPixels(destArray[1].data, destArray[1].cols, destArray[1].rows, OF_IMAGE_GRAYSCALE);
	imageOf3.setFromPixels(destArray[2].data, destArray[2].cols, destArray[2].rows, OF_IMAGE_GRAYSCALE);
	imageOf4.setFromPixels(destArray[3].data, destArray[3].cols, destArray[3].rows, OF_IMAGE_GRAYSCALE);
	imageOf5.setFromPixels(destArray[4].data, destArray[4].cols, destArray[4].rows, OF_IMAGE_GRAYSCALE);


	if (edgesFilter) {
		ofImage* images[] = { &imageOf1, &imageOf2, &imageOf3, &imageOf4, &imageOf5 };
		setAverageFilter(images, true);
		setVarianceFilter(images, true);
	}
	else {
		ofImage* images[] = { &imageOf1, &imageOf2, &imageOf3, &imageOf4, &imageOf5, &imageOf6 };
		setAverageFilter(images, false);
		setVarianceFilter(images, false);
	}
}


void xml_algorithms::setAverageFilter(ofImage* images[], bool edgesFilter) {
	
	int width = images[0]->getPixels().getWidth();
	int height = images[0]->getPixels().getHeight();

	int pixels_size = width * height;
	int r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0, r3 = 0, g3 = 0, b3 = 0, r4 = 0, g4 = 0, b4 = 0, r5 = 0, g5 = 0, b5 = 0, r6 = 0, g6 = 0, b6 = 0;

	//MEDIA
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			ofColor color1 = images[0]->getPixels().getColor(x, y);
			ofColor color2 = images[1]->getPixels().getColor(x, y);
			ofColor color3 = images[2]->getPixels().getColor(x, y);
			ofColor color4 = images[3]->getPixels().getColor(x, y);
			ofColor color5 = images[4]->getPixels().getColor(x, y);

			if (!edgesFilter) {
				ofColor color6 = images[5]->getPixels().getColor(x, y);

				r6 += color6.r;
				g6 += color6.g;
				b6 += color6.b;
			}
			r1 += color1.r;
			g1 += color1.g;
			b1 += color1.b;

			r2 += color2.r;
			g2 += color2.g;
			b2 += color2.b;

			r3 += color3.r;
			g3 += color3.g;
			b3 += color3.b;

			r4 += color4.r;
			g4 += color4.g;
			b4 += color4.b;

			r5 += color5.r;
			g5 += color5.g;
			b5 += color5.b;
		}
	}

	int avg_l1 = round((0.2125 * r1 + 0.7154 * g1 + 0.0721 * b1) / pixels_size);
	int avg_l2 = round((0.2125 * r2 + 0.7154 * g2 + 0.0721 * b2) / pixels_size);
	int avg_l3 = round((0.2125 * r3 + 0.7154 * g3 + 0.0721 * b3) / pixels_size);
	int avg_l4 = round((0.2125 * r4 + 0.7154 * g4 + 0.0721 * b4) / pixels_size);
	int avg_l5 = round((0.2125 * r5 + 0.7154 * g5 + 0.0721 * b5) / pixels_size);

	if(edgesFilter){
	avgEdges[0] = avg_l1;
	avgEdges[1] = avg_l2;
	avgEdges[2] = avg_l3;
	avgEdges[3] = avg_l4;
	avgEdges[4] = avg_l5;
	}
	else {
		int avg_l6 = round((0.2125 * r5 + 0.7154 * g5 + 0.0721 * b5) / pixels_size);

		avgGabor[0] = avg_l1;
		avgGabor[1] = avg_l2;
		avgGabor[2] = avg_l3;
		avgGabor[3] = avg_l4;
		avgGabor[4] = avg_l5;
		avgGabor[5] = avg_l6;
	}
}

void xml_algorithms::setVarianceFilter(ofImage* images[], bool edgesFilter) {
	int width = images[0]->getPixels().getWidth();
	int height = images[0]->getPixels().getHeight();


	int pixels_size = width * height;

	double d1 = 0, d2 = 0, d3 = 0, d4 = 0, d5 = 0, d6=0;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			ofColor color1 = images[0]->getPixels().getColor(x, y);
			ofColor color2 = images[1]->getPixels().getColor(x, y);
			ofColor color3 = images[2]->getPixels().getColor(x, y);
			ofColor color4 = images[3]->getPixels().getColor(x, y);
			ofColor color5 = images[4]->getPixels().getColor(x, y);

			double l1 = 0.2125 * color1.r + 0.7154 * color1.g + 0.0721 * color1.b;
			double l2 = 0.2125 * color2.r + 0.7154 * color2.g + 0.0721 * color2.b;
			double l3 = 0.2125 * color3.r + 0.7154 * color3.g + 0.0721 * color3.b;
			double l4 = 0.2125 * color4.r + 0.7154 * color4.g + 0.0721 * color4.b;
			double l5 = 0.2125 * color5.r + 0.7154 * color5.g + 0.0721 * color5.b;

			if (edgesFilter) {
				d1 += pow((l1 - avgEdges[0]), 2);
				d2 += pow((l2 - avgEdges[1]), 2);
				d3 += pow((l3 - avgEdges[2]), 2);
				d4 += pow((l4 - avgEdges[3]), 2);
				d5 += pow((l5 - avgEdges[4]), 2);
			}
	
			else{
				ofColor color6 = images[5]->getPixels().getColor(x, y);
				double l6 = 0.2125 * color6.r + 0.7154 * color6.g + 0.0721 * color6.b;

				d1 += pow((l1 - avgGabor[0]), 2);
				d2 += pow((l2 - avgGabor[1]), 2);
				d3 += pow((l3 - avgGabor[2]), 2);
				d4 += pow((l4 - avgGabor[3]), 2);
				d5 += pow((l5 - avgGabor[4]), 2);
				d6 += pow((l6 - avgGabor[5]), 2);
			}
		}
	}


	int dev1 = round(sqrt(d1 / pixels_size));
	int dev2 = round(sqrt(d2 / pixels_size));
	int dev3 = round(sqrt(d3 / pixels_size));
	int dev4 = round(sqrt(d4 / pixels_size));
	int dev5 = round(sqrt(d5 / pixels_size));

	if (edgesFilter) {
		devEdges[0] = dev1;
		devEdges[1] = dev2;
		devEdges[2] = dev3;
		devEdges[3] = dev4;
		devEdges[4] = dev5;
	}
	else {
		int dev6 = round(sqrt(d6 / pixels_size));

		devGabor[0] = dev1;
		devGabor[1] = dev2;
		devGabor[2] = dev3;
		devGabor[3] = dev4;
		devGabor[4] = dev5;
		devGabor[5] = dev6;
	}

}

int* xml_algorithms::getVarianceEdges() {
	return devEdges;
}

int* xml_algorithms::getAvgEdges() {
	return avgEdges;
}

int* xml_algorithms::getVarianceGabor() {
	return devGabor;
}

int* xml_algorithms::getAvgGabor() {
	return avgGabor;
}
