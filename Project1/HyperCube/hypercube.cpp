#include "hypercube.hpp"

Hypercube::Hypercube(int hashDimension,int maxValuesChecked,int probes,int nearestNeighbors,int range)
{
    HashDimension = hashDimension;
    MaxValuesChecked = maxValuesChecked;
    Probes = probes;
    NearestNeighbors = nearestNeighbors;
    Range = range;
    /*Generate Random vectors used in hashing*/
    vector<vector<double>> RandomVectors;
    GenerateRandomVectors(HashDimension,784,&RandomVectors);
    unsigned int w = 5;
    double t = 0.0;

    for(int i = 0;i< HashDimension;i++)
    {
        /*Initialize hash functions*/
        t = this->GenerateT(w);
        HashFunction hashFunc(RandomVectors[i],w,t);
        HashFunctions.push_back(hashFunc);
    }
}


void Hypercube::Hash(Point point)
{
    string HashCodes;
    /*Get the hash code eg([0,1,1,0])*/
    for(int i = 0;i < HashDimension; i++)
        HashCodes.push_back(HashFunctions[i].HashCube(*point.Vector));
    HashTable[HashCodes].push_back(point);
}

double Hypercube::QueryHash(Point point)
{
    /*Count time for execution*/
    auto start_time = chrono::high_resolution_clock().now();
    string HashCode;
    vector<string> Keys;

    /*Since Candidates is in private,clear so we dont have overlapping values*/
    Candidates.clear();

    /*Get the hash code eg([0,1,1,0])*/
    for(int i = 0;i < HashDimension; i++)
        HashCode.push_back(HashFunctions[i].HashCube(*point.Vector));
    /*If the bucket doesn't have enough Points to be checked*/
    if(HashTable[HashCode].size() < MaxValuesChecked)
    {
        /*Check nearby vertices for more points*/
        string key;
        /*Max Hamming Distance  = d,which is the dimension of the Keys,created by hashing*/
        for(int i = 0;i < HashDimension; i++)
        {
            /*for all items in the hashtable*/
            for(const auto& entry: HashTable)
            {
                /*If the key is not the hashcode (that means we are not on the same vertice we are searching from)*/
                key = entry.first;
                if(key != HashCode)
                {
                    /*Find the hamming distance and if it is equal than i,store the key*/
                    if(this->HammingDistance(key,HashCode) <= i)//The outer loop,ensures that we increase the hamming distance each time by 1,until we search Probes nearby vertices
                        Keys.push_back(key); //Store the key
                    if(Keys.size() == Probes) //If we reach the desired number of nearby vertices
                        break; //break
                }
            }
            if(Keys.size() == Probes) //also break here
                break; //For optimization and to not search more vertices than needed
        }
    }
    
    for(int i = 0;i < HashTable[HashCode].size(); i++)
    {
        Candidates.push_back(HashTable[HashCode][i]); //Add the potential candidates to a vector
        if(Candidates.size() == MaxValuesChecked) //if we reach the desired number early
            break;  //dont waste more time
    }
    /*If we havent reached the desired number of Candidates*/
    if(Candidates.size() < MaxValuesChecked)
    {
        /*Search the nearby vertices we found*/
        for(int i = 0;i < Probes; i++)
        {
            /*For each item in the bucket*/
            for(int j = 0;j < HashTable[Keys[i]].size(); j++)
            {
                /*Append a candidate*/
                Candidates.push_back(HashTable[Keys[i]][j]);
                if(Candidates.size() == MaxValuesChecked) //if we reach the desired number of Candidates
                    break;//break
            }
            if(Candidates.size() == MaxValuesChecked) //Again this time
                break;  //Break from the outerloop too,so we dont waste time
        }
    }
    for(int i = 0;i < Candidates.size(); i++) //For all candidates
        Candidates[i].Distance = PNorm(*Candidates[i].Vector,*point.Vector,2); //Find the euclidean norm between them and the query point

    auto end_time = chrono::high_resolution_clock().now();
    chrono::duration<double> duration = end_time - start_time;

    return duration.count() * 1000; //return time in milliseconds
}

double Hypercube::CentroidHash(CentroidPoint point,vector<double>& Distances)
{
    /*Count time for execution*/
    auto start_time = chrono::high_resolution_clock().now();
    string HashCode;
    vector<string> Keys;

    /*Since Candidates is in private,clear so we dont have overlapping values*/
    Candidates.clear();

    /*Get the hash code eg([0,1,1,0])*/
    for(int i = 0;i < HashDimension; i++)
        HashCode.push_back(HashFunctions[i].HashCube(point.Vector));
    /*If the bucket doesn't have enough Points to be checked*/
    if(HashTable[HashCode].size() < MaxValuesChecked)
    {
        /*Check nearby vertices for more points*/
        string key;
        /*Max Hamming Distance  = d,which is the dimension of the Keys,created by hashing*/
        for(int i = 0;i < HashDimension; i++)
        {
            /*for all items in the hashtable*/
            for(const auto& entry: HashTable)
            {
                /*If the key is not the hashcode (that means we are not on the same vertice we are searching from)*/
                key = entry.first;
                if(key != HashCode)
                {
                    /*Find the hamming distance and if it is equal than i,store the key*/
                    if(this->HammingDistance(key,HashCode) <= i)//The outer loop,ensures that we increase the hamming distance each time by 1,until we search Probes nearby vertices
                        Keys.push_back(key); //Store the key
                    if(Keys.size() == Probes) //If we reach the desired number of nearby vertices
                        break; //break
                }
            }
            if(Keys.size() == Probes) //also break here
                break; //For optimization and to not search more vertices than needed
        }
    }
    
    for(int i = 0;i < HashTable[HashCode].size(); i++)
    {
        Candidates.push_back(HashTable[HashCode][i]); //Add the potential candidates to a vector
        if(Candidates.size() == MaxValuesChecked) //if we reach the desired number early
            break;  //dont waste more time
    }
    /*If we havent reached the desired number of Candidates*/
    if(Candidates.size() < MaxValuesChecked)
    {
        /*Search the nearby vertices we found*/
        for(int i = 0;i < Probes; i++)
        {
            /*For each item in the bucket*/
            for(int j = 0;j < HashTable[Keys[i]].size(); j++)
            {
                /*Append a candidate*/
                Candidates.push_back(HashTable[Keys[i]][j]);
                if(Candidates.size() == MaxValuesChecked) //if we reach the desired number of Candidates
                    break;//break
            }
            if(Candidates.size() == MaxValuesChecked) //Again this time
                break;  //Break from the outerloop too,so we dont waste time
        }
    }
    for(int i = 0;i < Candidates.size(); i++) //For all candidates
        Distances.push_back((point.Vector,*Candidates[i].Vector,2)); //Find the euclidean norm between them and the query point

    auto end_time = chrono::high_resolution_clock().now();
    chrono::duration<double> duration = end_time - start_time;

    return duration.count() * 1000; //return time in milliseconds    
}

void Hypercube::FindClosestNeighbors(vector<double>& Distances,int CentroidID,vector<Point>& Points)
{
    for(int i = 0;i < Candidates.size(); i++)
    {
        /*The distance of a Candidate as well as the candidate are in the exact same position in the vector(this is done by the way they are pushed back in Hash)*/
        if(Candidates[i].Distance <= Range) //&& Candidates[i].Distance > Distances[i]
        {
            /*The cluster candidates are pointers to the hash table not the Point Vector,so we need the point vector*/
            /*Nevertheless the Values remain the same,so the PointID corresponds to the cell of the point in the points vector*/
            Points[Candidates[i].PointID].ClusterID = CentroidID;
            Points[Candidates[i].PointID].Distance = Distances[i];
            Points[Candidates[i].PointID].Assigned = true;
        }
    }
    Range *= 2;
}


int Hypercube::WriteToFile(ofstream& MyFile,Point point,vector<double>& Distances,vector<double>& BruteForceTime,vector<double>& Time)
{
    int max = 0;
    /*If this is not checked,seg fault will happen*/
    if(Candidates.size() == 0)
    {
        cout<<"No candidates were found!"<<endl;
        return -1;
    }

    sort(Candidates.begin(),Candidates.end(),ComparisonFunctionByDistance);
    MyFile<<"Query: "<<point.PointID<<endl;
    double AverageApproxDist = 0.0,AverageTrueDist = 0.0;
    if(NearestNeighbors > Candidates.size())
        max = Candidates.size();
    else/*else if the candidates are more than the neighbors we want*/
        max = NearestNeighbors;/*we must check up until the number of neighbors*/
         
    /*Size of candidates is ensured to be = MaxValuesChecked from QueryHash*/
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

    return 0;
}

/*Hamming Distance essentially checks each digit of a binary value and returns the number of different digits*/
/*Here since we store keys as strings of {0,1},strings are used as input*/
int Hypercube::HammingDistance(string A,string B)
{
    int count = 0;
    if(A.size() != B.size())
    {
        cout<<"Key sizes do not match!"<<endl;
        return -1;
    }
    for(int i = 0;i < A.size(); i++)
    {
        if(A[i] != B[i])
            count++;
    }

    return count;
}


void Hypercube::GenerateRandomVectors(int HashFunctions,int dimension,vector<vector<double>>* RandomVectors)
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

double Hypercube::GenerateT(unsigned int w)
{
    /*Normal distribution to generate a random value*/
    default_random_engine generator;
    uniform_real_distribution<double> distribution(0.0,w);

    return  distribution(generator);
}