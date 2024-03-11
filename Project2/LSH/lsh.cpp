#include "lsh.hpp"

LSH::LSH(int HashTables,int HashFunctions,int range,int nearestNeighbors,int ImageCount,int graphNearestNeighbors,int ImageSize)
{
    /*Resize the hash table and hash functions vectors to be of size L*/
    hashTables.resize(HashTables);
    HashFunctionVariables.resize(HashTables);
    NearestNeighbors = nearestNeighbors;
    Range = range;
    TableSize = (int)(ImageCount/32);
    GraphNearestNeighbors = graphNearestNeighbors;

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
double LSH::Hash(GraphPoint& Point,bool AssignToParent)
{
    vector<int> HashCodes;
    if(AssignToParent)
        Candidates.clear();
    /*Clear candidates vector from the previously stored values*/
    /*For each hash function return the hash values produced when the current vector is hashed*/
    for(int i = 0;i < hashFunctions.size();i++)
        HashCodes.push_back(hashFunctions[i].Hash(Point.Vector));
    /*and for each hash table find the g(p) value*/
    for(int i = 0;i < hashTables.size();i++)
    {
        /*Find the corresponding bucket of the vector we hashed*/
        int bucket = this->FindHashValue(HashCodes,HashFunctionVariables[i]);
        if(AssignToParent == false) /*push the item in the hash table*/
            hashTables[i][bucket].push_back(&Point);
        else /*else this is a query element*/
        {
            /*Check the bucket where the hash function leads us*/
            for(int j = 0;j < hashTables[i][bucket].size();j++)
            {
                if(hashTables[i][bucket][j]->PointID != Point.PointID)
                {
                    /*Create a bucket element instance to store the values of the element in the bucket*/
                    hashTables[i][bucket][j]->Distance = PNorm(Point.Vector,hashTables[i][bucket][j]->Vector,2);
                    /*Add it to the Potential candidates vector*/
                    Candidates.push_back(hashTables[i][bucket][j]);
                }
            }
        }
    }
   if(AssignToParent)
    {
        if(Candidates.size() <= 0)
            return 0.0;
        sort(Candidates.begin(),Candidates.end(),ComparisonFunctionByDistance);
        for(int i = 0;i < GraphNearestNeighbors; i++)
            Point.Neighbors.push_back(Candidates[i]);
    }
    return 0.0;
}

/*Finds the hash value given the hashcodes(which are the values returned by the hash functions)*/
int LSH::FindHashValue(vector<int> HashCodes,vector<int> RandomValues)
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