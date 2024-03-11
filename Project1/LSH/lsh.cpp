#include "lsh.hpp"

LSH::LSH(int HashTables,int HashFunctions,int range,int nearestNeighbors,int ImageCount,int ImageSize)
{
    /*Resize the hash table and hash functions vectors to be of size L*/
    hashTables.resize(HashTables);
    HashFunctionVariables.resize(HashTables);
    NearestNeighbors = nearestNeighbors;
    Range = range;
    TableSize = (int)(ImageCount/32);

    /*Generate Random vectors used in hashing*/
    vector<vector<double>> RandomVectors;
    GenerateRandomVectors(HashFunctions,ImageSize,&RandomVectors);
    unsigned int w = 5;
    double t = 0.0;
    int r = 0;

    /*Createa hash tables and hash variables(r_i)*/
    for(int i = 0;i < HashTables;i++)
    {
        /*Resize each hash table to be of bucket size*/
        hashTables[i].resize(TableSize);
        for(int j = 0;j < HashFunctions; j++)
        {
            /*Generate random r_i*/
            r = this->GenerateR();
            HashFunctionVariables[i].push_back(r);
        }
    }
    for(int j = 0;j< HashFunctions;j++)
    {
        /*Initialize hash functions*/
        t = this->GenerateT(w);
        HashFunction hashFunc(RandomVectors[j],w,t);
        hashFunctions.push_back(hashFunc);
    }

}

/*Generate r_i random for each h_i and hash table*/
int LSH::GenerateR()
{
    return rand() % 100 + 1;
}

/*This is the standard instance of Hash called by the LSH*/
double LSH::Hash(vector<byte>& Vector,int PointID,bool test)
{
    auto start_time = chrono::high_resolution_clock().now();
    vector<int> HashCodes;Point Candidate;
    int ID;
    /*Clear candidates vector from the previously stored values*/
    Candidates.clear();
    /*For each hash function return the hash values produced when the current vector is hashed*/
    for(int i = 0;i < hashFunctions.size();i++)
        HashCodes.push_back(hashFunctions[i].Hash(Vector));
    /*and for each hash table find the g(p) value*/
    for(int i = 0;i < hashTables.size();i++)
    {
        /*Find the corresponding bucket of the vector we hashed*/
        int bucket = this->FindHashValue(HashCodes,HashFunctionVariables[i],&ID);
        if(test == false) /*push the item in the hash table*/
        {
            Point element;
            element.Vector = &Vector;
            element.PointID = PointID;
            element.ID = ID;
            hashTables[i][bucket].push_back(element);
        }
        else /*else this is a query element*/
        {
            /*Check the bucket where the hash function leads us*/
            for(int j = 0;j < hashTables[i][bucket].size();j++)
            {
              //  if(hashTables[i][bucket][j].ID == ID)
              //  {
                    /*Create a bucket element instance to store the values of the element in the bucket*/
                    hashTables[i][bucket][j].Distance = PNorm(Vector,*hashTables[i][bucket][j].Vector,2);
                    /*Add it to the Potential candidates vector*/
                    Candidates.push_back(hashTables[i][bucket][j]);
             //   }
            }
        }
    }
    auto end_time = chrono::high_resolution_clock().now();
    chrono::duration<double> duration = end_time - start_time;

    return duration.count() * 1000;
}

/*Given a File,a query point and the N-nearest points found by brute force algo,writes items to file*/
int LSH::WriteToFile(ofstream& MyFile,Point point,vector<double>& Distances,vector<double>& BruteForceTime,vector<double>& Time)
{
    int max = 0;
    /*If this is not checked,seg fault will happen*/
    if(Candidates.size() == 0)
    {
        cout<<"No candidates were found!"<<endl;
        return -1;
    }
    /*Need to implement to find the N nearest neighbors in the R range*/
    sort(Candidates.begin(),Candidates.end(),ComparisonFunctionByDistance);

    /*Implement Brute force search here to find the true distances between points*/


    MyFile<<"Query: "<<point.PointID<<endl;
    /*If the number of neighbors we want is more than the potential candidates*/
    /*We must check only up to the size of the candidates*/
    if(NearestNeighbors > Candidates.size())
        max = Candidates.size();
    else/*else if the candidates are more than the neighbors we want*/
        max = NearestNeighbors;/*we must check up until the number of neighbors*/
         
    double AverageApproxDist = 0.0,AverageTrueDist = 0.0;
    for(int i = 0;i < max; i++)
    {
        /*Check if they are within range*/
        if(Candidates[i].Distance <= Range)
        {
            MyFile<<"Nearest Neighbor-"<<i<<": "<<Candidates[i].PointID<<endl
                  <<"distanceApproximate: "<<Candidates[i].Distance<<endl
                  <<"distanceTrue: "<<Distances[i]<<endl;
            AverageApproxDist += Candidates[i].Distance;
            AverageTrueDist += Distances[i];
        }
    }
    double AlgorithmAverageTime = 0.0,BruteForceAverageTime = 0.0;

    AverageApproxDist /= max;
    AverageTrueDist /= max;

    for(int i = 0;i < Time.size();i++)
    {
        AlgorithmAverageTime += Time[i];
        BruteForceAverageTime += BruteForceTime[i];
    }

    AlgorithmAverageTime /= Time.size();
    BruteForceAverageTime /= BruteForceTime.size();

    double AF = AverageApproxDist / AverageTrueDist;

    MyFile<<"tAverageApproximate: "<<AlgorithmAverageTime<<"ms"<<endl
          <<"tAverageTrue: "<<BruteForceAverageTime<<"ms"<<endl
          <<"AF: "<<AF<<endl;
   /* MyFile<<Range<<"-near Neighbors: "<<endl;
    for(int i = 0;i < Candidates.size(); i++)
        MyFile<<Candidates[i].PointID<<endl;*/

    return 0;
}

/*Finds the hash value given the hashcodes(which are the values returned by the hash functions)*/
int LSH::FindHashValue(vector<int> HashCodes,vector<int> RandomValues,int* ID)
{
    /*this computes the g(p) value*/
    int Bucket = 0;
    /*By adding the r_i*h_i(p)*/
    for(int i = 0;i < HashCodes.size(); i++)
        Bucket += RandomValues[i]*HashCodes[i];
    uint64_t result = static_cast<uint64_t>(1) << 32; // 2^32
    result -= 5;
    /*and then by finding the remained with 2^32 - 5*/
    Bucket %= result;
    *ID = Bucket;
    /*and the remainder of that with the tablesize,so the hash value is in the [0,tablesize]*/
    Bucket %= TableSize;

    return Bucket;
}

/*Creates random vectors based on normal distribution*/
void LSH::GenerateRandomVectors(int HashFunctions,int dimension,vector<vector<double>>* RandomVectors)
{
    /*Create a normal distribution generator*/
    default_random_engine generator;
    normal_distribution<double> distribution(0.0, 1.0);

    vector<double> randomVector(dimension);
    /*Create a random vector with normal distribution according to the number of hash functions*/
    /*and the dimnension of the input and query data*/
    for(int i = 0;i < HashFunctions; i++)
    {
        for (int j = 0; j < dimension; j++)
            randomVector[j] = distribution(generator);
        RandomVectors->push_back(randomVector);
    }
}

double LSH::GenerateT(unsigned int w)
{
    /*Normal distribution to generate a random value*/
    default_random_engine generator;
    uniform_real_distribution<double> distribution(0.0,w);

    return  distribution(generator);
}

/*The functions below here are used by the cluster algorithm when assigning*/
                    /*points to centroids*/
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/*This instance is called when the cluster uses LSH*/
/*It is the same as the original Hash function*/
/*But hashes A pre created point item,rather than create a point item and then  store it*/
void LSH::Hash(Point Point)
{
    vector<int> HashCodes;
    double Distance = 0.0;
    int ID;
    /*For each hash function return the hash values produced when the current vector is hashed*/
    for(int i = 0;i < hashFunctions.size();i++)
        HashCodes.push_back(hashFunctions[i].Hash(*Point.Vector));
    /*and for each hash table find the g(p) value*/
    for(int i = 0;i < hashTables.size();i++)
    {
        int bucket = this->FindHashValue(HashCodes,HashFunctionVariables[i],&ID);
        /*and push the address of the vector to the hash table*/
        hashTables[i][bucket].push_back(Point);
    }
}

/*This instance of hash is called with the Centroids as Vectors*/
/*It hashes the Centroid Vector in order to find the Correct Bucket*/
void LSH::Hash(CentroidPoint Point,vector<double>* Distance)
{
    vector<int> HashCodes;
    int ID;
    /*Clear candidates vector from the previously stored values as well as previous distances*/
    ClusterCandidates.clear();
    Distance->clear();
    /*For each hash function return the hash values produced when the current vector is hashed*/
    for(int i = 0;i < hashFunctions.size();i++)
        HashCodes.push_back(hashFunctions[i].Hash(Point.Vector));
    /*and for each hash table find the g(p) value*/
    for(int i = 0;i < hashTables.size();i++)
    {
        int bucket = this->FindHashValue(HashCodes,HashFunctionVariables[i],&ID);
        /*Check the bucket where the hash function leads us*/
        for(int j = 0;j < hashTables[i][bucket].size();j++)
        {
            Distance->push_back(PNorm(Point.Vector,*hashTables[i][bucket][j].Vector,2));/*Store the distance to the Centroid in a variable to compare it with the pre existing distance*/
            ClusterCandidates.push_back(&hashTables[i][bucket][j]); /*Store the address of the potential candidate*/
        }
    }
}

/*This instance of find closest neighbors is called in the cluster algorithm*/
void LSH::FindClosestNeighbors(int CentroidID,vector<double>& Distance,vector<Point>& Points)
{
    for(int i = 0;i < ClusterCandidates.size(); i++)
    {
        /*The distance of a Candidate as well as the candidate are in the exact same position in the vector(this is done by the way they are pushed back in Hash)*/
        if(ClusterCandidates[i]->Distance <= Range) // && ClusterCandidates[i]->Distance > Distance[i]
        {
            /*The cluster candidates are pointers to the hash table not the Point Vector,so we need the point vector*/
            /*Nevertheless the Values remain the same,so the PointID corresponds to the cell of the point in the points vector*/
            Points[ClusterCandidates[i]->PointID].ClusterID = CentroidID;
            Points[ClusterCandidates[i]->PointID].Distance = Distance[i];
            Points[ClusterCandidates[i]->PointID].Assigned = true;
        }
    }
}

void LSH::DoubleRange()
{
    Range *= 2;
}
