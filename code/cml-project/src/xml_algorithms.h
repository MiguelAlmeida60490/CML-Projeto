
class xml_algorithms {
public:
	//CONSTANTS
	static const int NUM_EDGES_IMAGES = 5;
	static const int NUM_GABOR_IMAGES = 6;
	static const int RGB = 3;

	int* getColor(ofImage img);
	int getLuminance();
	void setFilter(ofImage img, bool edgesFilter);
	int getMatches(ofImage img1, ofImage img2);
	int* getVarianceEdges();
	int* getAvgEdges();
	int* getVarianceGabor();
	int* getAvgGabor();
private:
	int avg_l;
	int* avgColor = new int[RGB];
	int* avgEdges = new int[NUM_EDGES_IMAGES];
	int* devEdges = new int[NUM_EDGES_IMAGES];
	int* avgGabor = new int[NUM_GABOR_IMAGES];
	int* devGabor = new int[NUM_GABOR_IMAGES];
	void setAverageFilter(ofImage* images[], bool edgesFilter);
	void setVarianceFilter(ofImage* images[], bool edgesFilter);
};