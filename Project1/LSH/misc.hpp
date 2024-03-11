#include <fstream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <cstdint>
#include <random>
#include <arpa/inet.h>
#include <cmath>
#include <bits/stdc++.h>
#include <chrono>
#include "hash.hpp"

struct Header {
    int32_t magic_number;
    int32_t num_images;
    int32_t num_rows;
    int32_t num_columns;
};

typedef struct Point{
    vector<byte>* Vector;
    int ID;
    int PointID;
    int ClusterID;
    double Distance;
    bool Assigned;
}Point;

typedef struct CentroidPoint{
    vector<double> Vector;
    vector<byte> ClosestPointVector;
    int ID;
    int RealPointID;
    int PointsCount;
}CentroidPoint;

typedef struct LatentHeader{
    int32_t magic_number;
    int32_t num_images;
    int32_t image_length;
}LatentHeader;


void GetArgs(int argc,char** argv,char** input_file,char** query_file,char** output_file,int* K,int* L,int* N,int* R);
void GetArgs(int argc,char** argv,char** input_file,char** query_file,char** output_file,int* HashDimension,int* MaxValuesChecked,int* Probes,int* NearestNeighbors,int* Range);
void OpenFile(char* filename,vector<vector<byte>>* images,bool test);
int OpenLatentFile(char* filename,vector<vector<byte>>* images,bool test);
double BruteForce(vector<double>* Distances,vector<vector<byte>> Points,vector<byte> QueryPoint,int NearestNeighbors);
double PNorm(vector<byte>& A,vector<byte>& B,int p);
double PNorm(vector<double>& A,vector<byte>& B,int p);