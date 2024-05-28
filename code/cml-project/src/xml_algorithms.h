
class xml_algorithms {
public:
	const int NUM_EDGES_IMAGES = 5;
	const int NUM_GABOR_IMAGES = 6;
	const int RGB = 3;
	int avg_l;
	int* avgColor = new int[RGB];
	double* avgEdges = new double[NUM_EDGES_IMAGES];
	double* devEdges = new double[NUM_EDGES_IMAGES];
	double* avgGabor = new double[NUM_GABOR_IMAGES];
	double* devGabor = new double[NUM_GABOR_IMAGES];
	int* getColor(ofImage img);
	int getLuminance();
	void setFilter(ofImage img, bool edgesFilter);
	double* getVarianceEdges();
	double* getAvgEdges();
	double* getVarianceGabor();
	double* getAvgGabor();
private:
	void setAverageFilter(ofImage* images[], bool edgesFilter);
	void setVarianceFilter(ofImage* images[], bool edgesFilter);
};