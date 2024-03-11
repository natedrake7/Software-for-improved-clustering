#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <arpa/inet.h>
#include <cmath>
#include "../HyperCube/hypercube.hpp"

using namespace std;

typedef struct Config{
    int Clusters;
    int Hash_tables;
    int Hash_functions;
    int Max_m_hypercube;
    int Hypercube_dim;
    int Probes;
}Config;

/*Gets args*/
void GetArgs(int argc,char** argv,char** input_file,char** latent_input,char** output_file,bool* complete,char** method,Config* config,bool* Latent);
/*Reads data from the config file*/
int ReadConfig(char* filename,Config* config);
/*Converts vector from byte type to double*/
void CopyVector(vector<double>& A,vector<byte>& B);
/*Get a random value*/
double GenerateRandomValue();

double GetUniformlyDistributedValue(double LastProbability);

class Cluster{
    private:
        int Clusters;
        vector<CentroidPoint> Centroids;
        vector<CentroidPoint> OldCentroids;
        vector<Point> Points;
        LSH* Lsh;
        Hypercube* hypercube;
    public:
        /*Constructor-Destructor*/
        Cluster(int clusters,vector<vector<byte>>& Data);
        ~Cluster();

        /*Initialization functions for Centroids and first assignment of points to clusters*/
        int ChooseCentroid(vector<vector<byte>>& Data);
        int ChooseCentroid2(vector<vector<byte>>& Data);
        void InitializeClusters(vector<vector<byte>>& Data);
        /*Check which algorithm to call(may change)*/
        void Assignment(char* algorithm);

        /*Lloyd's algorithm (Brute force)*/
        void ClassicAlgorithm();

        /*LSH algorithms*/
        void InitializeLSH(int HashFunctions,int HashTables,int Range);       
        void LSH_Algorithm();
        void Outliers();

        /*HyperCube algorithms*/
        void InitializeHyperCube(int HashDimension,int Probes,int MaxNumChecked,int Range);
        void HyperCube_Algorithm();

        /*MacQueen update method and convergence check*/
        void Update();        
        int Converge();

        /*Write to output file,and if complete is selected write */
        void WriteToFile(double time,char* method,char* output_file);
        void WriteToFile(double time,char* method,char* output_file,bool complete);       
        void WriteToFileLatent(double time,char* method,char* output_file,vector<vector<byte>>& Points);

        /*Compute Silhouette for the clustering(if complete is selected the 2nd instance is used,check function description for more)*/
        void Silhouette(vector<double>* dists,double* TotalSilhouette);
        void Silhouette(vector<double>* dists,double* TotalSilhouette,vector<vector<Point>>& OrganizedPoints);
        void SilhouetteLatent(vector<double>* dists,double* TotalSilhouette,vector<vector<byte>>& TruePoints);

        /*Computes the Objective Function for the Normal Dimension*/
        double ObjectiveFunction();
        /*Computes the Objective Function for the latent dimension,by getting the points in the normal dimension*/
        double ObjectiveFunction(vector<vector<byte>>& Points);

        /*Calculate Euclidean norm (overload for when using a Centroid in computations)*/
        double L2Norm(vector<byte>& A,vector<byte>& B);
        double L2Norm(vector<byte>& A,vector<double>& B);

        /*May delete later*/
        void GetPointByID(int ID,Point* Point);
};