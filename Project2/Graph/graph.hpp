#include <iostream>
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
#include "../LSH/lsh.hpp"

using namespace std;

bool NeighborsComparisonFunction(tuple<GraphPoint*,double>& A,tuple<GraphPoint*,double>& B);

class Graph{
    private:
        vector<GraphPoint>* GraphVector;
    public:
        Graph(int GraphNearestNeighbors,int NearestNeighbors,vector<vector<byte>>& Images);
        Graph(vector<vector<byte>>& Images);
        ~Graph();
        vector<GraphPoint*> GetNeighbors(int PointID);
        vector<GraphPoint>& GetGraphVector();
        int GetGraphSize();
        int GetClosestNeighbors(int PointID,int Expansions,vector<tuple<GraphPoint*, double>>& ExpansionPoints,GraphPoint* QueryPoint);
        int GetNeighborsCount(int PointID);
        void MRNG();
        vector<GraphPoint*> GetSortedPointsByDistance(const GraphPoint& queryPoint);
};