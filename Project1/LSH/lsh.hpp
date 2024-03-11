#include "misc.hpp"

class LSH{
    private:
        vector<vector<vector<Point>>> hashTables;
        vector<HashFunction> hashFunctions;
        vector<vector<int>> HashFunctionVariables;
        vector<Point> Candidates;
        vector<Point*> ClusterCandidates;
        int Range;
        int NearestNeighbors;
        int TableSize;

    public:
         /*Constructor*/
        LSH(int HashTables,int HashFunctions,int Range,int NearestNeighbors,int ImageCount,int ImageSize);
        /*Standard hash used by LSH when called from Lsh_main*/
        double Hash(vector<byte>& Vector,int PointID,bool test);
        int FindHashValue(vector<int> HashCodes,vector<int> RandomValues,int* ID);
        double GenerateT(unsigned int w);
        int GenerateR();
        void GenerateRandomVectors(int HashFunctions,int dimension,vector<vector<double>>* RandomVectors);
        int WriteToFile(ofstream& MyFile,Point point,vector<double>& Distances,vector<double>& BruteForceTime,vector<double>& Time);

        /*These Overloaded instances are used when LSH is called by Cluster*/
        /*Hash a point*/
        void Hash(Point Point);
        /*Hash a Centroid*/
        void Hash(CentroidPoint Point,vector<double>* Distance);
        /*Find neighbors*/
        void FindClosestNeighbors(int CentroidID,vector<double>& Distance,vector<Point>& Points);
        void DoubleRange();

};

/*Used when std::sort is called*/
bool ComparisonFunctionByDistance(const Point& A,const Point& B);
bool ComparisonFunctionByCluster(const Point& A,const Point& B);

/*Initilization for lsh_main*/
void InitializeProgram(int K,int L,int N,int R,char* input_file,LSH** Lsh,vector<vector<byte>>& train_set);
/*Main run*/
int Program(int N,char* input_file,char* query_file,char* output_file,LSH** Lsh,vector<vector<byte>>& train_set,vector<vector<byte>>& test_set);