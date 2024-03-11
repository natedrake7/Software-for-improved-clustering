#include "graph_search.hpp"

int main(int argc,char** argv)
{
    srand((unsigned int)(time(nullptr)));

    //Variable Declarations
    char* input_file;char* query_file;char* latent_input_file;char* latent_query_file;char* output_file;char* method = new char(4);bool latent = false;
    int GraphNearestNeighbors,Expansions,RandomRestarts,NearestNeighbors,TankCandidates;
    vector<vector<byte>> Images,Queries,LatentImages,LatentQueries;
    vector<tuple<GraphPoint*, double>> ExpansionPoints;
    vector<tuple<double,int>> BruteForceLatentToNormalSpace;
    vector<double> TrueDistances,time,BruteForceTime,BruteForceDistances,TrueBruteForceTime;
    GraphPoint QueryPoint,LatentQueryPoint;

    //Get arguments from command line
    GetArgs(argc,argv,&input_file,&query_file,&output_file,&GraphNearestNeighbors,&Expansions,
            &RandomRestarts,&NearestNeighbors,&TankCandidates,&method,&latent,&latent_input_file,&latent_query_file);
    //Open files and check if they exist
    if(latent)
    {
        if(OpenLatentFile(latent_input_file,&LatentImages,false) == -1)
            return -1;
        if(OpenLatentFile(latent_query_file,&LatentQueries,true) == -1)
            return -1;
        LatentImages.erase(LatentImages.begin() + 2000,LatentImages.end());
    }

    if(OpenFile(input_file,&Images,false) == -1)
        return -1;
    if(OpenFile(query_file,&Queries,true) == -1)
        return -1;
        
    Images.erase(Images.begin() + 2000,Images.end());
    //initialize graph depending on method
    Graph* graph;
    if(strcmp(method,"GNNS") == 0)
        latent ? graph = new Graph(GraphNearestNeighbors,NearestNeighbors,LatentImages) : graph = new Graph(GraphNearestNeighbors,NearestNeighbors,Images);
    else //initialize graph for MRNG
        latent ? graph = new Graph(LatentImages) : graph = new Graph(Images);

    ofstream MyFile(output_file);

    MyFile<<method<<" results"<<endl; //Write method
    if(strcmp(method,"GNNS") == 0)
    {
        for(int i = 0;i < Queries.size(); i++)
        {
            QueryPoint.PointID = i;
            QueryPoint.Vector = &Queries[i];
            if(latent)
            {
                LatentQueryPoint.PointID = i;
                LatentQueryPoint.Vector = &LatentQueries[i];
                time.push_back(GNNS(ExpansionPoints,&LatentQueryPoint,graph,Expansions,RandomRestarts,NearestNeighbors,10));
                BruteForceTime.push_back(BruteForce(&BruteForceLatentToNormalSpace,LatentImages,*LatentQueryPoint.Vector,NearestNeighbors));
                TrueBruteForceTime.push_back(BruteForce(&BruteForceDistances,Images,*QueryPoint.Vector,NearestNeighbors));
                WriteToFile(MyFile,time,BruteForceTime,method,ExpansionPoints,BruteForceLatentToNormalSpace,BruteForceDistances,TrueBruteForceTime,&QueryPoint,Images);
                ClearVectors(time,BruteForceTime,ExpansionPoints,TrueDistances,TrueBruteForceTime,BruteForceDistances);
            }
            else
            {
                time.push_back(GNNS(ExpansionPoints,&QueryPoint,graph,Expansions,RandomRestarts,NearestNeighbors,10));
                BruteForceTime.push_back(BruteForce(&TrueDistances,Images,*QueryPoint.Vector,NearestNeighbors));
                WriteToFile(MyFile,time,BruteForceTime,method,ExpansionPoints,TrueDistances,&QueryPoint);
                ClearVectors(time,BruteForceTime,ExpansionPoints,TrueDistances);
            }
        }
        MyFile.close();
        delete method;
        return 0;
    }
    for(int i = 0;i < Queries.size(); i++)
    {
        QueryPoint.PointID = i;
        QueryPoint.Vector = &Queries[i];
        if(latent)
        {
            LatentQueryPoint.PointID = i;
            LatentQueryPoint.Vector = &LatentQueries[i];
            time.push_back(SearchOnGraph(ExpansionPoints,&LatentQueryPoint,graph,NearestNeighbors,TankCandidates));
            BruteForceTime.push_back(BruteForce(&BruteForceLatentToNormalSpace,LatentImages,*LatentQueryPoint.Vector,NearestNeighbors));
            TrueBruteForceTime.push_back(BruteForce(&BruteForceDistances,Images,*QueryPoint.Vector,NearestNeighbors));
            WriteToFile(MyFile,time,BruteForceTime,method,ExpansionPoints,BruteForceLatentToNormalSpace,BruteForceDistances,TrueBruteForceTime,&QueryPoint,Images);
            ClearVectors(time,BruteForceTime,ExpansionPoints,TrueDistances,TrueBruteForceTime,BruteForceDistances);
        }
        else
        {
            time.push_back(SearchOnGraph(ExpansionPoints,&QueryPoint,graph,NearestNeighbors,TankCandidates));
            BruteForceTime.push_back(BruteForce(&TrueDistances,Images,*QueryPoint.Vector,NearestNeighbors));
            WriteToFile(MyFile,time,BruteForceTime,method,ExpansionPoints,TrueDistances,&QueryPoint);
            ClearVectors(time,BruteForceTime,ExpansionPoints,TrueDistances);
        }
    }
    MyFile.close();
    delete method;
    return 0;
}

double GNNS(vector<tuple<GraphPoint*, double>>& ExpansionPoints,GraphPoint* QueryPoint,Graph* graph,int Expansions,int RandomRestarts,int NearestNeighbors,int GreedySteps)
{
    auto start_time = chrono::high_resolution_clock().now();
    int Y = 0;
    for(int i = 0;i < RandomRestarts; i++) //For a number of random restarts
    {
        Y = rand() % graph->GetGraphSize(); //get a uniformly distributed point in the graph
        for(int t = 0;t < GreedySteps; t++) //for a number of greedy steps
        {
            Y = graph->GetClosestNeighbors(Y,Expansions,ExpansionPoints,QueryPoint); //find the closest neighbor of Y to Query Point
            if(Y == -1) //if no neighbor was found ,restart with a new point
                break;
        }
    }
    /*Dev,should never be true*/
    if(ExpansionPoints.size() <= 0)
    {
        cout<<"No points were found!"<<endl;
        exit(0);
    }

    sort(ExpansionPoints.begin(),ExpansionPoints.end(),NeighborsComparisonFunction); //Sort all the explored nodes
        
    /*Set IsExpanded to 0 for the next query point,since we are using pointers to the values of the GraphVector and not copied values*/
    for(int i = 0; i < ExpansionPoints.size(); i++)
        get<0>(ExpansionPoints[i])->IsExpanded = false;

    if(NearestNeighbors < ExpansionPoints.size()) //if the neighbors we found are more than the nearest neighbors we want
        ExpansionPoints.erase(ExpansionPoints.begin() + NearestNeighbors,ExpansionPoints.end()); //Delete the rest of them(vector is sorted by Distance to query point,so we delete after NearestNeighbors)

    auto end_time = chrono::high_resolution_clock().now();
    chrono::duration<double> duration = end_time - start_time;

    return duration.count() * 1000;
}


double SearchOnGraph(vector<tuple<GraphPoint*,double>>& PoolOfCandidates,GraphPoint* QueryPoint,Graph* graph,int NearestNeighbors,int TankCandidates)
{
    auto start_time = chrono::high_resolution_clock().now();
    int StartPoint = rand() % graph->GetGraphSize(); //get a uniformly distributed point in the graph
    bool flag;
    vector<GraphPoint>& graphVector = graph->GetGraphVector();
    set<int> checkedNodes; // To keep track of checked nodes
    PoolOfCandidates.push_back(tuple(&graphVector[StartPoint],PNorm(graphVector[StartPoint].Vector,QueryPoint->Vector,2))); // Initialize PoolOfCandidates with the starting point

    int i=0; // i is the index for the first unchecked node in PoolOfCandidates

    while (i < TankCandidates)
    {
        flag = true;
        // Find the first unchecked node in PoolOfCandidates
        for (int j = 0; j < PoolOfCandidates.size(); ++j)
        {
            if (checkedNodes.find(get<0>(PoolOfCandidates[j])->PointID) == checkedNodes.end())
            {
                i = j;
                flag = false;
                break;
            }
        }

        if(flag)
            break; // No unchecked nodes remaining

        const GraphPoint* p = get<0>(PoolOfCandidates[i]);
        checkedNodes.insert(p->PointID);    // Mark p as checked

         // Explore neighbors of p
        for (GraphPoint* neighbor : graph->GetNeighbors(p->PointID))
        {
            if (checkedNodes.find(neighbor->PointID) == checkedNodes.end())
                PoolOfCandidates.push_back(tuple(neighbor,PNorm(neighbor->Vector,QueryPoint->Vector,2)));
        }

        // Sort PoolOfCandidates in ascending order of distance to the query point
        sort(PoolOfCandidates.begin(), PoolOfCandidates.end(),NeighborsComparisonFunction);

        // Resize PoolOfCandidates to NearestNeighbors, if needed
        if (PoolOfCandidates.size() > TankCandidates)
            PoolOfCandidates.resize(TankCandidates);
    }
    //PoolOfCandidates contains the NearestNeighbors candidates sorted by distance to the query point
    PoolOfCandidates.erase(PoolOfCandidates.begin() + NearestNeighbors,PoolOfCandidates.end());

    auto end_time = chrono::high_resolution_clock().now();
    chrono::duration<double> duration = end_time - start_time;

    return duration.count() * 1000;
}
