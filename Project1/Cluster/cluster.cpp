#include "cluster.hpp"


/*Constructor to create Cluster by initializing the first centroids*/
Cluster::Cluster(int clusters,vector<vector<byte>>& Data)
{
    int ID;CentroidPoint point;
    point.PointsCount = 0;
    Clusters = clusters;
    Lsh = nullptr;
    hypercube = nullptr;

    /*Initialize centroids*/
    for(int i = 0;i < Clusters; i++)
    {
        /*Get a random image from the dataset*/
        if(i == 0)
        {
            ID = rand() % Data.size();
            point.ID = i; /*Give it a unique ID based on the iteration we are*/
        }
        else
        {
            ID = this->ChooseCentroid2(Data);
            point.ID = i;
            /*if ID is -1 we failed to find a centroid*/
            if(ID == -1)
            {
                cout<<"Error failed to find Centroid!"<<endl;
                /*Exit since the algo will fail*/
                exit(-1);
            }
        }
        CopyVector(point.Vector,Data[ID]);/*Copy the vector*/
        Centroids.push_back(point); /*push it back*/
        point.Vector.clear();
    }
}

/*Destructor to clear any dynamically allocated variables*/
Cluster::~Cluster()
{
    /*Since LSH is created with new,we need to destroy it after we are done using it*/
    if(Lsh != nullptr)
        delete Lsh;
    /*Delete hypercube if used*/
    if(hypercube != nullptr)
        delete hypercube;
}

int Cluster::ChooseCentroid2(vector<vector<byte>>& Data)
{
    vector<double> minDistances(Data.size(),numeric_limits<double>::max());
    vector<double> Probabilties;
    double distance = 0.0,probability = 0.0;

    //P(0) = 0.0
    Probabilties.push_back(0.0);

    for(int i = 0;i < Data.size(); i++)
    {
        for(int j = 0;j < Centroids.size(); j++)
        {
            distance = this->L2Norm(Data[i],Centroids[j].Vector);
            if(distance < minDistances[i]) 
                minDistances[i] = distance;
        }
        for(int counter = 0;counter < i; counter++)
            probability += pow(minDistances[i],2);
        Probabilties.push_back(probability);
    }

    double UniformlyDistributedValue = GetUniformlyDistributedValue(Probabilties.back());

    for(int i = 1;i < Probabilties.size(); i++)
    {
        if(Probabilties[i - 1] < UniformlyDistributedValue && Probabilties[i] >= UniformlyDistributedValue)
            return i;
    }

    return -1;
}




/*Kmeans++ initialization*/
int Cluster::ChooseCentroid(vector<vector<byte>>& Data)
{
    /*set min distances to max*/
    vector<double> minDistances(Data.size(),numeric_limits<double>::max());
    double distance = 0.0,sumDistances = 0.0,probability = 0.0;
    /*For each data point in the dataset*/
    for(int i = 0;i < Data.size(); i++)
    {
        for(int j = 0;j < Centroids.size(); j++)
        {
            /*find the l2 distance between each currently declared centroid*/
            distance = this->L2Norm(Data[i],Centroids[j].Vector);
            /*keep the minimum*/
            minDistances[i] = min(minDistances[i],distance);
        }
        /*sum the minimum distances*/
        sumDistances += minDistances[i];
    }
        
    for(int i = 0;i < Data.size(); i++)
    {
        /*find the cumulative propability*/
        probability += minDistances[i] / sumDistances;
        /*if the propability is greater than the random value*/
        if(probability >= GenerateRandomValue())
            return i; /*return the data item*/
    }
    /*it should never reach here*/
    return -1;
}



/*Assign each point to its nearest centroid*/
void Cluster::InitializeClusters(vector<vector<byte>>& Data)
{
    Point point;
    double distance;
    /*For all our data*/
    for(int i = 0;i < Data.size(); i++)
    {
        /*Initalize values of point variable*/
        point.Vector = &Data[i];
        point.Distance = numeric_limits<double>::max(); /*We want to minimize the distance so we initialize it distance to the max value of double*/
        point.PointID = i;
        point.Assigned = false;
        for(int j = 0;j < Centroids.size(); j++)
        {
            /*Calculate the L2 norm of the current centroid and the current image*/
            distance = L2Norm(*point.Vector,Centroids[j].Vector);
            /*If it is less than the previous centroid*/
            if(distance < point.Distance)
            {
                /*Assign the current centroid and distance*/
                point.Distance = distance;
                point.ClusterID = j;
            }
        }
        /*At the end the best centroid will be assigned,so add the point to the array*/
        Points.push_back(point);
    }
}

/*Check if centroids remain the same after update*/
/*If they do,the algorithm has converged*/
int Cluster::Converge()
{
    int count = 0;
    for(int i = 0;i < Centroids.size(); i++)
    {
        /*Check the number of centroids that remain the same*/
        if(Centroids[i].Vector == OldCentroids[i].Vector)
            count++;
    }
    cout<<"Same centroids count is: "<<count<<endl;
    /*if all centroids remain the same,the algorithm has converged*/
    if(count == Centroids.size())
        return 1;
    /*if not,continue*/
    return 0;
}

/*By the assigned method,calls the appropriate function*/
void Cluster::Assignment(char* algorithm)
{
    if(strcmp(algorithm,"Classic") == 0)
        this->ClassicAlgorithm();
    else if(strcmp(algorithm,"LSH") == 0)
        this->LSH_Algorithm();
    else if(strcmp(algorithm,"Hypercube") == 0)
        this->HyperCube_Algorithm();
}

/*Lloyd's algorithm (Brute force)*/
void Cluster::ClassicAlgorithm()
{
    double distance;
    int count = 0;
    /*For all our data*/
    for(int i = 0;i < Points.size(); i++)
    {
        Points[i].Distance = numeric_limits<double>::max();
        for(int j = 0;j < Centroids.size(); j++)
        {
            /*Calculate the L2 norm of the current centroid and the current image*/
            distance = L2Norm(*Points[i].Vector,Centroids[j].Vector);
            /*If it is less than the previous centroid*/
            if(distance < Points[i].Distance)
            {
                /*Assign the current centroid and distance*/
                Points[i].Distance = distance;
                Points[i].ClusterID = j;
            }
        }
    }
}

/*Macqueen update method*/
void Cluster::Update()
{
    OldCentroids = Centroids;
    for(int i = 0;i < Centroids.size();i++)
    {
        /*Set all values of the centroid to 0,since a new centroid will take place*/
        /*Will need to change if the ending method is convergence*/
        fill(Centroids[i].Vector.begin(),Centroids[i].Vector.end(),0.0);
        /*find the total number of points to divide by*/
        Centroids[i].PointsCount = 0;
    }
    for(int i = 0;i < Points.size(); i++)
    {
        /*for all items in the vector*/
        for(int j = 0;j < Points[i].Vector->size(); j++)/*find the sum of each point in the R^d vector*/
            Centroids[Points[i].ClusterID].Vector[j] += (int)(*Points[i].Vector)[j];
        Centroids[Points[i].ClusterID].PointsCount++;/*increment the number of points in the cluster*/
    }

    for(int i = 0;i < Centroids.size(); i++)
    {   /*for each centroid*/
        if(Centroids[i].PointsCount != 0)
        {
            for(int j = 0;j < Centroids[i].Vector.size(); j++)/*divide each datapoint in the centroid vector by the number of points in the cluster*/
                Centroids[i].Vector[j] /= Centroids[i].PointsCount;/*this helps us find the mean of the cluster*/
        }
        cout<<"Centroid "<<i + 1<<" Points: "<<Centroids[i].PointsCount<<endl;
    }
}

/*These are the functions that utilize the LSH class*/
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/*Initializes LSH*/
void Cluster::InitializeLSH(int HashFunctions,int HashTables,int Range)
{
    Lsh = new LSH(HashTables, HashFunctions, Range, Points.size(), Points.size(),Points[0].Vector->size());
    for(int i = 0; i < Points.size(); i++)
        Lsh->Hash(Points[i]);
}

/*Hashes the updated centroids and finds the neighbors in that range*/
/*todo Hash(distance)*/
void Cluster::LSH_Algorithm()
{
    vector<double> Distance;
    for(int i = 0;i < Centroids.size(); i++)
    {
        Lsh->Hash(Centroids[i],&Distance);
        Lsh->FindClosestNeighbors(Centroids[i].ID,Distance,Points);
    }
    Lsh->DoubleRange();
}

/*These are functions used by Hypercube*/
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void Cluster::InitializeHyperCube(int HashDimension,int Probes,int MaxNumChecked,int Range)
{
    hypercube = new Hypercube(HashDimension,MaxNumChecked,Probes,Points.size(),Range);
    for(int i = 0;i < Points.size(); i++)
        hypercube->Hash(Points[i]);
}

void Cluster::HyperCube_Algorithm()
{
    vector<double> Distances;
    for(int i = 0;i < Centroids.size(); i++)
    {
        hypercube->CentroidHash(Centroids[i],Distances);
        hypercube->FindClosestNeighbors(Distances,Centroids[i].ID,Points);
    }
}


/*These are miscellanious functions*/
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/*Assign all unassigned points by the LSH or Hypercube algorithm,to the nearest Centroid*/
/*This algorithm takes place after converge*/
void Cluster::Outliers()
{
    double distance = 0.0;
    for(int i = 0;i < Centroids.size(); i++)
        Centroids[i].PointsCount = 0;
    for(int i = 0;i < Points.size(); i++)
    {
        /*If point has not been assigned by the LSH to the nearest centroid*/
        if(Points[i].Assigned == false)
        {
            /*Brute force to find nearest centroid*/
            for(int j = 0;j < Centroids.size(); j++)
            {
                distance = this->L2Norm(*Points[i].Vector,Centroids[j].Vector);
                /*Find the min distance*/
                if(distance < Points[i].Distance)
                {
                    /*Assign the current centroid and distance*/
                    Points[i].Distance = distance;
                    Points[i].ClusterID = Centroids[j].ID;
                }
            }
        }
        Centroids[Points[i].ClusterID].PointsCount++;
    }
}

/*Calculate the euclidean distance betweeen 2 vectors in R^d*/
double Cluster::L2Norm(vector<byte>& A,vector<double>& B)
{
    double sum = 0.0,diff = 0.0;

    /*Both vectors must be of the same size*/
    if(A.size() != B.size())
    {
        cout<<"Vector sizes do not match!"<<endl
            <<"Vector A has size: "<<A.size()<<endl
            <<"Vector B has size: "<<B.size()<<endl;
        return -1.0;
    }

    /*iterate over both of the vectors*/
    for(int i = 0;i < A.size(); i++)
    {
        /*find the absolute of the difference between the 2 points*/
        diff = (double)A[i] - (double)B[i];
        sum += diff*diff;
    }
    /*else return the n-nth root of the sum*/
    return sqrt(sum);
}

/*Calculate the euclidean distance betweeen 2 vectors in R^d(this is used in the Silhouette computation)*/
double Cluster::L2Norm(vector<byte>& A,vector<byte>& B)
{
    double sum = 0.0,diff = 0.0;

    /*Both vectors must be of the same size*/
    if(A.size() != B.size())
    {
        cout<<"Vector sizes do not match!"<<endl
            <<"Vector A has size: "<<A.size()<<endl
            <<"Vector B has size: "<<B.size()<<endl;
        return -1.0;
    }

    /*iterate over both of the vectors*/
    for(int i = 0;i < A.size(); i++)
    {
        /*find the absolute of the difference between the 2 points*/
        diff = (double)A[i] - (double)B[i];
        sum += diff*diff;
    }
    /*else return the n-nth root of the sum*/
    return sqrt(sum);
}

/*Returns a point by its ID*/
void Cluster::GetPointByID(int ID,Point* Point)
{
    /*If the ID is in the vector size*/
    if(ID < Points.size())
        Point = &Points[ID]; /*Assign it to the Point variable*/
    else /*Error message and point is empty*/
        cout<<"Point with ID: "<<ID<<" not found!"<<endl;
}

/*Copies a byte vector to double*/
void CopyVector(vector<double>& A,vector<byte>& B)
{
    /*Copy data from byte type to double type*/
    for(int i = 0;i < B.size(); i++)
        A.push_back((int)B[i]);
}

/*Random value generator*/
double GenerateRandomValue()
{
    double randomValue = 0.001 + ((rand() % 100 + 1) / 1000.0);
    return (double)(rand() / RAND_MAX + randomValue);
}

double GetUniformlyDistributedValue(double LastProbability)
{
    std::random_device rd;
    std::mt19937_64 gen(rd());

    std::uniform_real_distribution<double> distribution(0.0, LastProbability);

    return distribution(gen);
}

/*Calculate the silhouette metric*/
/*Need to find a way to sum the distances between all items in the same cluster and also find the nearest cluster (outside ofcourse)
the current one*/
void Cluster::Silhouette(vector<double>* dists,double* TotalSilhouette,vector<vector<Point>>& OrganizedPoints)
{
    OrganizedPoints.resize(Centroids.size());
    for(int i = 0;i < Points.size(); i++)
    {
        for(int j = 0;j < Centroids.size(); j++)
        {
            if(Points[i].ClusterID == Centroids[j].ID)
                OrganizedPoints[j].push_back(Points[i]);
        }
    }
    double ClusterSilhouette = 0.0,Silhouette = 0.0,a = 0.0,b = 0.0,distance = 0.0,ClusterDistance = numeric_limits<double>::max();
    int ClosestClusterID = 0;
    /*The OrganizedPoints vector is organized in the following way*/
    /*Points of the i-th row are points assigned to the i-th Centroid*/
    for(int i = 0;i < OrganizedPoints.size(); i++)
    {
        ClusterSilhouette = 0.0;
        /*For each point in the cluster*/
        for(int j = 0;j < OrganizedPoints[i].size(); j++)
        {
            Silhouette = 0.0,a = 0.0,b = 0.0,distance = 0.0,ClusterDistance = numeric_limits<double>::max();
            ClosestClusterID = 0;
            /*find the average distance between the point and all the others in the cluster*/
            for(int k = 0; k < OrganizedPoints[i].size(); k++)
            {
                if(k != j)
                    a += L2Norm(*OrganizedPoints[i][j].Vector,*OrganizedPoints[i][k].Vector);
            }
            a /= Centroids[i].PointsCount;
            /*Find the nearest cluster*/
            for(int k = 0;k < Centroids.size(); k++)
            {
                if(OrganizedPoints[i][j].ClusterID != Centroids[k].ID)
                {  
                    distance = this->L2Norm(*OrganizedPoints[i][j].Vector,Centroids[k].Vector);
                    if(distance < ClusterDistance)
                    {
                        ClusterDistance = distance;
                        ClosestClusterID = Centroids[k].ID;
                    }
                }
            }
            /*Find the average distance between that point and all the others from the nearest cluster*/
            for(int k = 0;k < OrganizedPoints[ClosestClusterID].size(); k++)
                b += L2Norm(*OrganizedPoints[i][j].Vector,*OrganizedPoints[ClosestClusterID][k].Vector);
            b /= Centroids[ClosestClusterID].PointsCount;
            if(a > b)
                Silhouette = (b / a) - 1;
            else if(a < b)
                Silhouette = 1 - (a / b);
            else
                Silhouette = 0;
            ClusterSilhouette += Silhouette;
            if(j % 1000 == 0 && j != 0)
                cout<<"Currently at iteration "<<j<<" of: "<<OrganizedPoints[i].size()<<endl;
        }
        ClusterSilhouette /= Centroids[i].PointsCount;
        dists->push_back(ClusterSilhouette);
        cout<<"Silhouette value for cluster "<<i<<" is: "<<ClusterSilhouette<<endl;
    }
    for(int i = 0; i < dists->size(); i++)
        *TotalSilhouette += (*dists)[i];
    *TotalSilhouette /= Centroids.size();
}

/*TODO: Organized points have different sizes than centroid points count?*/
 void Cluster::Silhouette(vector<double>* dists,double* TotalSilhouette)
 {
    vector<vector<Point>> OrganizedPoints;
    OrganizedPoints.resize(Centroids.size());
    for(int i = 0;i < Points.size(); i++)
    {
        for(int j = 0;j < Centroids.size(); j++)
        {
            if(Points[i].ClusterID == Centroids[j].ID)
                OrganizedPoints[j].push_back(Points[i]);
        }
    }
    double ClusterSilhouette = 0.0,Silhouette = 0.0,a = 0.0,b = 0.0,distance = 0.0,ClusterDistance = numeric_limits<double>::max();
    int ClosestClusterID = 0;
    /*The OrganizedPoints vector is organized in the following way*/
    /*Points of the i-th row are points assigned to the i-th Centroid*/
    for(int i = 0;i < OrganizedPoints.size(); i++)
    {
        ClusterSilhouette = 0.0;
        for(int j = 0;j < OrganizedPoints[i].size(); j++)
        {
            Silhouette = 0.0,a = 0.0,b = 0.0,distance = 0.0,ClusterDistance = numeric_limits<double>::max();
            ClosestClusterID = 0;
            for(int k = 0; k < OrganizedPoints[i].size(); k++)
            {
                if(k != j)
                    a += L2Norm(*OrganizedPoints[i][j].Vector,*OrganizedPoints[i][k].Vector);
            }
            a /= Centroids[i].PointsCount;
            for(int k = 0;k < Centroids.size(); k++)
            {
                if(OrganizedPoints[i][j].ClusterID != Centroids[k].ID)
                {  
                    distance = this->L2Norm(*OrganizedPoints[i][j].Vector,Centroids[k].Vector);
                    if(distance < ClusterDistance)
                    {
                        ClusterDistance = distance;
                        ClosestClusterID = Centroids[k].ID;
                    }
                }
            }
            for(int k = 0;k < OrganizedPoints[ClosestClusterID].size(); k++)
                b += L2Norm(*OrganizedPoints[i][j].Vector,*OrganizedPoints[ClosestClusterID][k].Vector);
            b /= Centroids[ClosestClusterID].PointsCount;
            if(a > b)
                Silhouette = (b / a) - 1;
            else if(a < b)
                Silhouette = 1 - (a / b);
            else
                Silhouette = 0;
            ClusterSilhouette += Silhouette;
            if(j % 1000 == 0 && j != 0)
                cout<<"Currently at iteration "<<j<<" of: "<<OrganizedPoints[i].size()<<endl;
        }
        ClusterSilhouette /= Centroids[i].PointsCount;
        dists->push_back(ClusterSilhouette);
        cout<<ClusterSilhouette<<endl;
    }
    for(int i = 0; i < dists->size(); i++)
        *TotalSilhouette += (*dists)[i];
    *TotalSilhouette /= Centroids.size();
}

void Cluster::SilhouetteLatent(vector<double>* dists,double* TotalSilhouette,vector<vector<byte>>& TruePoints)
{
    vector<vector<Point>> OrganizedPoints;
    OrganizedPoints.resize(Centroids.size());
    Point TruePoint;
    double CentroidDistance = 0.0,MinCentroidDistance = numeric_limits<double>::max();
    int CentroidID = -1;

    for(int i = 0;i < Centroids.size(); i++)
    {
        for(int j = 0;j < Points.size(); j++)
        {
            CentroidDistance = PNorm(Centroids[i].Vector,*Points[j].Vector,2);
            if(CentroidDistance < MinCentroidDistance)
            {
                MinCentroidDistance = CentroidDistance;
                CentroidID = j;
            }
        }
        Centroids[i].RealPointID = CentroidID;
        Centroids[i].Vector.clear();
        Centroids[i].ClosestPointVector = TruePoints[CentroidID];
        CentroidDistance = 0.0;
        MinCentroidDistance = numeric_limits<double>::max();
        CentroidID = -1;
    }
    

    for(int i = 0;i < Points.size(); i++)
    {
        for(int j = 0;j < Centroids.size(); j++)
        {
            if(Points[i].ClusterID == Centroids[j].ID)
            {
                TruePoint.PointID = Points[i].PointID;
                TruePoint.Assigned = Points[i].Assigned;
                TruePoint.ClusterID = Points[i].ClusterID;
                TruePoint.Distance = Points[i].Distance;
                TruePoint.Vector = &TruePoints[i];
                OrganizedPoints[j].push_back(TruePoint);
            }
        }
    }

    double ClusterSilhouette = 0.0,Silhouette = 0.0,a = 0.0,b = 0.0,distance = 0.0,ClusterDistance = numeric_limits<double>::max();
    int ClosestClusterID = 0;
    /*The OrganizedPoints vector is organized in the following way*/
    /*Points of the i-th row are points assigned to the i-th Centroid*/
    for(int i = 0;i < OrganizedPoints.size(); i++)
    {
        ClusterSilhouette = 0.0;
        for(int j = 0;j < OrganizedPoints[i].size(); j++)
        {
            Silhouette = 0.0,a = 0.0,b = 0.0,distance = 0.0,ClusterDistance = numeric_limits<double>::max();
            ClosestClusterID = 0;
            for(int k = 0; k < OrganizedPoints[i].size(); k++)
            {
                if(k != j)
                    a += L2Norm(*OrganizedPoints[i][j].Vector,*OrganizedPoints[i][k].Vector);
            }
            a /= Centroids[i].PointsCount;
            for(int k = 0;k < Centroids.size(); k++)
            {
                if(OrganizedPoints[i][j].ClusterID != Centroids[k].ID)
                {  
                    distance = this->L2Norm(*OrganizedPoints[i][j].Vector,Centroids[k].ClosestPointVector);
                    if(distance < ClusterDistance)
                    {
                        ClusterDistance = distance;
                        ClosestClusterID = Centroids[k].ID;
                    }
                }
            }
            for(int k = 0;k < OrganizedPoints[ClosestClusterID].size(); k++)
                b += L2Norm(*OrganizedPoints[i][j].Vector,*OrganizedPoints[ClosestClusterID][k].Vector);
            b /= Centroids[ClosestClusterID].PointsCount;
            if(a > b)
                Silhouette = (b / a) - 1;
            else if(a < b)
                Silhouette = 1 - (a / b);
            else
                Silhouette = 0;
            ClusterSilhouette += Silhouette;
            if(j % 1000 == 0 && j != 0)
                cout<<"Currently at iteration "<<j<<" of: "<<OrganizedPoints[i].size()<<endl;
        }
        ClusterSilhouette /= Centroids[i].PointsCount;
        dists->push_back(ClusterSilhouette);
        cout<<ClusterSilhouette<<endl;
    }
    for(int i = 0; i < dists->size(); i++)
        *TotalSilhouette += (*dists)[i];
    *TotalSilhouette /= Centroids.size();
}


/*Write to file funtion used when complete is not selected*/
void Cluster::WriteToFile(double time,char* method,char* output_file)
{
    vector<double> SilhouetteDistances;
    double TotalSilhoutte = 0.0;
    this->Silhouette(&SilhouetteDistances,&TotalSilhoutte);

    ofstream Myfile(output_file);

    /*Write general data to file*/
    Myfile<<"Algorithm: "<<method<<endl;
    /*Write clustering time to file*/
    Myfile<<"clustering_time: "<<time<<"s"<<endl;

    /*Write Silhouette distances to file*/
    Myfile<<"Silhouette: ";
    for(int i = 0;i < SilhouetteDistances.size(); i++)
        Myfile<<SilhouetteDistances[i]<<", ";

    Myfile<<TotalSilhoutte<<endl;
    Myfile<<"ObjectiveFunction: "<<this->ObjectiveFunction()<<endl;

    Myfile.close();
}


//Objective Function for normal space
double Cluster::ObjectiveFunction()
{
    double currentDistance = 0.0,minDistance = numeric_limits<double>::max();
    double Norm = 0.0;
    for(int i = 0;i < Points.size(); i++)
    {
        for(int j = 0;j < Centroids.size(); j++)
        {
            currentDistance = PNorm(Centroids[j].Vector,*Points[i].Vector,2);
            if(currentDistance < minDistance)
                minDistance = currentDistance;
        }
        Norm += pow(minDistance,2);
        minDistance = numeric_limits<double>::max();
        currentDistance = 0.0;
    }
    return Norm;
}

//Objective Function for latent space(conversion from latent to normal space first)
double Cluster::ObjectiveFunction(vector<vector<byte>>& TruePoints)
{
    double currentDistance = 0.0,minDistance = numeric_limits<double>::max();
    double Norm = 0.0;
    for(int i = 0;i < TruePoints.size(); i++)
    {
        for(int j = 0;j < Centroids.size(); j++)
        {
            currentDistance = PNorm(TruePoints[Centroids[j].RealPointID],TruePoints[i],2);
            if(currentDistance < minDistance)
                minDistance = currentDistance;
        }
        Norm += pow(minDistance,2);
        minDistance = numeric_limits<double>::max();
        currentDistance = 0.0;
    }
    return Norm;
}

/*Write to file function used when complete is selected*/
void Cluster::WriteToFile(double time,char* method,char* output_file,bool complete)
{
    vector<double> SilhouetteDistances;
    vector<vector<Point>> OrganizedPoints;
    double TotalSilhouette;

    /*Get silhouette values as well as the points organized by centroids*/
    this->Silhouette(&SilhouetteDistances,&TotalSilhouette,OrganizedPoints);

    ofstream Myfile(output_file);
    Myfile<<"Algorithm: "<<method<<endl;
    for(int i = 0;i < Centroids.size(); i++)
    {
        /*Write the number of points assigned to the centoid*/
        Myfile<<"CLUSTER-"<<i<<"{size: "<<Centroids[i].PointsCount<<",centroid: ";
            for(int j = 0; j < Centroids[i].Vector.size(); j++)//As well as the centroid vector
                Myfile<<Centroids[i].Vector[j]<<" ";
        Myfile<<"}"<<endl;
    }
    /*Write clustering time*/
    Myfile<<"clustering_time: "<<time<<"s"<<endl;

    /*Write Silhouette distances to file*/
    Myfile<<"Silhouette: [";
    for(int i = 0;i < SilhouetteDistances.size(); i++)
        Myfile<<SilhouetteDistances[i]<<", ";

    Myfile<<TotalSilhouette;
    Myfile<<"]"<<endl;
    /*Write the points organized by Cluster and the vector of the centroid*/
    for(int i = 0;i < OrganizedPoints.size(); i++)
    {
        Myfile<<"CLUSTER-"<<i<<" {";
        for(int j = 0;j < Centroids[i].Vector.size(); j++)
            Myfile<<Centroids[i].Vector[j]<<" ";
        for(int j = 0; j < OrganizedPoints[i].size(); j++)
            Myfile<<", "<<OrganizedPoints[i][j].PointID;
        Myfile<<"}"<<endl;
    }
    Myfile.close();
}

void Cluster::WriteToFileLatent(double time,char* method,char* output_file,vector<vector<byte>>& Points)
{
    vector<double> SilhouetteDistances;
    double TotalSilhoutte = 0.0;
    this->SilhouetteLatent(&SilhouetteDistances,&TotalSilhoutte,Points);
    ofstream Myfile(output_file);

    /*Write general data to file*/
    Myfile<<"Algorithm: "<<method<<endl;
    /*Write clustering time to file*/
    Myfile<<"clustering_time: "<<time<<"s"<<endl;

    /*Write Silhouette distances to file*/
    Myfile<<"Silhouette: ";
    for(int i = 0;i < SilhouetteDistances.size(); i++)
        Myfile<<SilhouetteDistances[i]<<", ";

    Myfile<<TotalSilhoutte<<endl;
    Myfile<<"ObjectiveFunction: "<<this->ObjectiveFunction(Points)<<endl;

    Myfile.close();
}