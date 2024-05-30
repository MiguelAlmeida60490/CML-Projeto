
class xml_algorithms {
public:
	static const int NUM_EDGES_IMAGES = 5;
	static const int NUM_GABOR_IMAGES = 6;
	static const int RGB = 3;
	int avg_l;
	int* avgColor = new int[RGB];
	int* avgEdges = new int[NUM_EDGES_IMAGES];
	int* devEdges = new int[NUM_EDGES_IMAGES];
	int* avgGabor = new int[NUM_GABOR_IMAGES];
	int* devGabor = new int[NUM_GABOR_IMAGES];
	int* getColor(ofImage img);
	int getLuminance();
	void setFilter(ofImage img, bool edgesFilter);
	int* getVarianceEdges();
	int* getAvgEdges();
	int* getVarianceGabor();
	int* getAvgGabor();
private:
	void setAverageFilter(ofImage* images[], bool edgesFilter);
	void setVarianceFilter(ofImage* images[], bool edgesFilter);
};