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

typedef struct Header {
    int32_t magic_number;
    int32_t num_images;
    int32_t num_rows;
    int32_t num_columns;
}Header;

typedef struct LatentHeader{
    int32_t magic_number;
    int32_t num_images;
    int32_t image_length;
}LatentHeader;

typedef struct GraphPoint{
    vector<byte>* Vector;
    double Distance;
    int PointID;
    bool IsExpanded = false;
    vector<GraphPoint*> Neighbors;
}GraphPoint;

typedef struct LatentPoint{
    vector<byte> Vector;
    int PointID;
    LatentPoint(size_t vectorSize) : Vector(vectorSize) {
    }
}LatentPoint;

void WriteToFile(ostream& MyFile,vector<double>& time,vector<double>& BruteForceTime,char* method,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances,GraphPoint* QueryPoint);
void WriteToFile(ostream& MyFile,vector<double>& time,vector<double>& BruteForceTime,char* method,vector<tuple<GraphPoint*, double>>& ExpansionPoints,
                vector<tuple<double,int>>& BruteForceLatentToNormalSpace,vector<double>& TrueBruteForceDistances,vector<double>& TrueBruteForceTime,GraphPoint* QueryPoint,vector<vector<byte>>& Images);
int GetNewFiles(char** query_file,char** output_file);
int OpenLatentFile(char* filename,vector<vector<byte>>* images,bool test);
int CheckFileExistance(char* filename,bool QueryFile,bool Latent,vector<vector<byte>>& Vector);
void ClearVectors(vector<double>& time,vector<double>& BruteForceTime,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances);
void ClearVectors(vector<double>& time,vector<double>& BruteForceTime,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances,
                  vector<double>& TrueBruteForceTime,vector<double>& BruteForceDistance);
void GetArgs(int argc,char** argv,char** input_file,char** query_file,char** output_file,int* GraphNearestNeighbors,int* Extensions,
             int* RandomRestarts,int* NearestNeighbors,int* TankCandidates,char** method,bool* latent,char** latent_input_file,char** latent_query_file);
int OpenFile(char* filename,vector<vector<byte>>* images,bool test);
double BruteForce(vector<double>* Distances,vector<vector<byte>> Points,vector<byte> QueryPoint,int NearestNeighbors);
double BruteForce(vector<tuple<double,int>>* Distances,vector<vector<byte>> Points,vector<byte> QueryPoint,int NearestNeighbors);
double PNorm(vector<byte>* A,vector<byte>* B,int p);
double PNorm(vector<double>& A,vector<byte>& B,int p);
bool CandidatesComparisonFunciton(GraphPoint* A, GraphPoint* B);
bool CandidatesComparisonFuncitonForTuple(tuple<double,int>& A,tuple<double,int>& B);