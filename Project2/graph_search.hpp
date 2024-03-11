#include "./Graph/graph.hpp"

double GNNS(vector<tuple<GraphPoint*, double>>& ExpansionPoints,GraphPoint* QueryPoint,Graph* graph,int Expansions,int RandomRestarts,int NearestNeighbors,int GreedySteps);
double SearchOnGraph(vector<tuple<GraphPoint*,double>>& PoolOfCandidates,GraphPoint* QueryPoint,Graph* graph,int NearestNeighbors,int TankCandidates);