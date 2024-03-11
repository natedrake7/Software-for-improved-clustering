#include "graph.hpp"

Graph::Graph(int GraphNearestNeighbors,int NearestNeighbors,vector<vector<byte>>& Images)
{
    int HashTables = 4,HashFunctions = 5;
    LSH Lsh(HashTables,HashFunctions,10000,NearestNeighbors,Images.size(),GraphNearestNeighbors,Images[0].size());
    GraphPoint NewPoint;
    GraphVector = new vector<GraphPoint>;
    GraphVector->resize(Images.size());
    for(int i = 0;i < Images.size(); i++)
    {
        /*Assign Values to a GraphPoint variable*/
        NewPoint.PointID = i; //This is the primary key of the image
        NewPoint.Vector = &Images[i]; //Vector of the image

        (*GraphVector)[i] = NewPoint;
        Lsh.Hash((*GraphVector)[i],false); //Hash the Pointer
    }
    for(int i = 0;i < GraphVector->size(); i++)
        Lsh.Hash((*GraphVector)[i],true);
    
    /*Dev,should delete after(checks if all points exist in graph)*/
    for(int i = 0;i < GraphVector->size(); i++)
    {
        if(this->GetNeighborsCount(i) <= 0)
        {
            cout<<"Failed to initialize Graph Properly!"<<endl;
            exit(-1);
        }
    }
}

Graph::Graph(vector<vector<byte>>& Images)
{
    GraphPoint NewPoint;
    GraphPoint* Point;
    GraphVector = new vector<GraphPoint>;
    GraphVector->resize(Images.size());
    for(int i = 0;i < Images.size(); i++)
    {
        /*Assign Values to a GraphPoint variable*/
        NewPoint.PointID = i; //This is the primary key of the image
        NewPoint.Vector = &Images[i]; //Vector of the image
        (*GraphVector)[i] = NewPoint;
    }
    this->MRNG();
    
    /*Dev,should delete after(checks if all points exist in graph)*/
    for(int i = 0;i < GraphVector->size(); i++)
    {
        if(this->GetNeighborsCount(i) <= 0)
        {
            cout<<"Failed to initialize Graph Properly!"<<endl;
            exit(-1);
        }
    }
}

Graph::~Graph()
{
    delete GraphVector;
}

vector<GraphPoint*>Graph::GetNeighbors(int PointID)
{
    if(PointID > GraphVector->size() || PointID < 0)
    {
        cout<<"Invalid PointID!"<<endl;
        exit(-1);
    }
    return (*GraphVector)[PointID].Neighbors;
}

int Graph::GetClosestNeighbors(int PointID,int Expansions,vector<tuple<GraphPoint*, double>>& ExpansionPoints,GraphPoint* QueryPoint)
{
    vector<GraphPoint*> Neighbors = this->GetNeighbors(PointID);
    if(Neighbors.size() == 0) //If Datapoint has no neighbors,don't iterate the remaining greedy points(0 neighbors over and over)
        return -1;

   // vector<tuple<GraphPoint*, double>> CurrentExpansions;
    int count = 0,IterationsCount = 0;

    if(Neighbors.size() > Expansions) //Find the min of both
        IterationsCount = Expansions;
    else
        IterationsCount = Neighbors.size();

    double MinDistance = numeric_limits<double>::max();
    int MinDistancePointID = -1;
    double L2Norm = 0.0;
    for(int i = 0;i < IterationsCount; i++)
    {
        if(!Neighbors[i]->IsExpanded) //If Datapoint hasn't been explored
        {
            //Add it to the vector
            L2Norm = PNorm(Neighbors[i]->Vector, QueryPoint->Vector,2);
            ExpansionPoints.push_back(tuple(Neighbors[i],L2Norm));
            //CurrentExpansions.push_back(tuple(Neighbors[i],L2Norm))
            //Find the min L2 Distance from the neighbors
            if(L2Norm < MinDistance)
            {
                //store it as the new min distance
                MinDistance = L2Norm;
                //store the PointID of the graph
                MinDistancePointID = Neighbors[i]->PointID;
            }
            Neighbors[i]->IsExpanded = true; //mark it as explored
        }
        else
            count++; //else check the number of explored neighbors this point has
    }
    if(count == IterationsCount) //If the number of explored neighbors is the number of expansions or the number of neighbors(no new datapoints will appear on the rest of the greedy iterations)
        return -1;  //break from the loop
    //sort(CurrentExpansions.begin(),CurrentExpansions.end(),NeighborsComparisonFunction); //else sort the current expansion array which has the neighbors for the current Point
    return MinDistancePointID;   //return the Min Distance Point ID
}

int Graph::GetGraphSize(){ return GraphVector->size();}

int Graph::GetNeighborsCount(int PointID)
{   
    if(PointID > GraphVector->size() || PointID < 0)
    {
        cout<<"Invalid PointID!"<<endl;
        exit(-1);
    }
    return (*GraphVector)[PointID].Neighbors.size();
}

vector<GraphPoint>& Graph::GetGraphVector() {
        return *GraphVector;
}

vector<GraphPoint*> Graph::GetSortedPointsByDistance(const GraphPoint& Point) {
    // Calculate distances and create a vector of tuples
    vector<tuple<GraphPoint*, double>> distancesAndPoints;
    for (GraphPoint& node : *GraphVector) {
        double distance = PNorm(Point.Vector, node.Vector, 2);  
        distancesAndPoints.push_back(tuple(&node, distance));
    }

    // Sort the vector of tuples based on distances
    sort(distancesAndPoints.begin(), distancesAndPoints.end(),NeighborsComparisonFunction); //TODO change comparison functions

    // Extract the sorted GraphPoint* objects into a new vector
    vector<GraphPoint*> sortedPoints;
    
    for (const auto& tuple : distancesAndPoints) 
    {
        GraphPoint* currentNode = get<0>(tuple);
        if (currentNode != &Point)
            sortedPoints.push_back(currentNode);
    }

    return sortedPoints;
}

void Graph::MRNG()
{
    int GraphSize = this->GetGraphSize();
    vector<GraphPoint>& graphVector = this->GetGraphVector();
    bool flag = true;

    for (int i = 0; i < GraphSize; i++) 
    {
        GraphPoint& p = graphVector[i];
        vector<GraphPoint*> SortedByCurrentPointDist = this->GetSortedPointsByDistance(p);

        if (!SortedByCurrentPointDist.empty()) {
            p.Neighbors.push_back(SortedByCurrentPointDist.front());    //Lp
            // Remove the first element from SortedByCurrentPointDist
            SortedByCurrentPointDist.erase(SortedByCurrentPointDist.begin());  //Rp
        }

        vector<GraphPoint*> NeighborsCandidates = SortedByCurrentPointDist;     //Rp-Lp
        for (const GraphPoint* neighbor : p.Neighbors)
        {
            auto it = find(NeighborsCandidates.begin(), NeighborsCandidates.end(), neighbor);
            if (it != NeighborsCandidates.end())
                NeighborsCandidates.erase(it);
        }

        for (int k = 0; k < NeighborsCandidates.size(); k++) 
        {   
            flag = true;
            const GraphPoint& r = *(NeighborsCandidates[k]);
            for (int j = 0; j < p.Neighbors.size(); j++) 
            {
                const GraphPoint& t = *(p.Neighbors[j]);
                double pr = PNorm(r.Vector, p.Vector,2);
                double pt = PNorm(t.Vector, p.Vector,2);
                double rt = PNorm(r.Vector, t.Vector,2);

                if (pr > pt && pr > rt) {
                    flag = false;
                    break;
                }
            }
            if(flag)
                p.Neighbors.push_back(NeighborsCandidates[k]);  //add to Lp (as Neighbor)
        }
    }
}

bool NeighborsComparisonFunction(tuple<GraphPoint*,double>& A,tuple<GraphPoint*,double>& B) { return get<1>(A) < get<1>(B) ;}
