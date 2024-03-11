#include "../LSH/lsh.hpp"

class Hypercube{
    private:
        unordered_map<string, vector<Point>> HashTable;
        vector<HashFunction> HashFunctions;
        vector<Point> Candidates;
        int HashDimension;
        int MaxValuesChecked;
        int Probes;
        int NearestNeighbors;
        int Range;
    public:
        Hypercube(int hashDimension,int maxValuesChecked,int probes,int nearestNeighbors,int range);
        void GenerateRandomVectors(int HashFunctions,int dimension,vector<vector<double>>* RandomVectors);
        double GenerateT(unsigned int w);
        void Hash(Point point);
        double QueryHash(Point point);
        double CentroidHash(CentroidPoint point,vector<double>& Distances);
        void FindClosestNeighbors(vector<double>& Distances,int CentroidID,vector<Point>& Points);
        int WriteToFile(ofstream& MyFile,Point point,vector<double>& Distances,vector<double>& BruteForceTime,vector<double>& Time);
        int HammingDistance(string A,string B);
};

int Program(int NearestNeighbors,char* input_file,char* query_file,char* output_file,Hypercube** hypercube,vector<vector<byte>>& train_set,vector<vector<byte>>& test_set);
void InitializeProgram(int HashDimension,int MaxValuesChecked,int Probes,int NearestNeighbors,int Range,char* input_file,Hypercube** hypercube,vector<vector<byte>>& train_set);