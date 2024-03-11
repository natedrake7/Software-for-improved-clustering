#include "hash.hpp"

/*Constructor*/
HashFunction::HashFunction(vector<double>& randomVector,unsigned int window,double t)
{
    /*Initialize variables*/
    RandomVector = randomVector;
    Window = window;
    T = t;
}

/*Hash for all Points in the vector*/
int HashFunction::Hash(vector<byte>& Vector)
{
    double dot_product = 0.0;
    /*calculate the dot product between the current vector and the random vector of the hash function*/
    for(int i = 0;i < RandomVector.size(); i++)
        dot_product += (int)(Vector[i]) * RandomVector[i];
    /*return the h(p) normalized by getting its absolute value*/
    return abs((dot_product + T) / Window);
}

/*Hash for Centroids(Overloaded for Cluster)*/
int HashFunction::Hash(vector<double>& Vector)
{
    double dot_product = 0.0;
    /*calculate the dot product between the current vector and the random vector of the hash function*/
    for(int i = 0;i < RandomVector.size(); i++)
        dot_product += (Vector[i]) * RandomVector[i];
    /*return the h(p) normalized by getting its absolute value*/
    return abs((dot_product + T) / Window);
}

/*Overloaded instance for hypercube*/
char HashFunction::HashCube(vector<byte>& Vector)
{
    double dot_product = 0.0;
    /*calculate the dot product between the current vector and the random vector of the hash function*/
    for(int i = 0;i < RandomVector.size(); i++)
        dot_product += (int)(Vector[i]) * RandomVector[i];
    /*return the h(p) normalized by getting its absolute value*/
    dot_product += T;
    dot_product /= Window;

    if(dot_product > 0)
        return '1';

    return '0';
}

char HashFunction::HashCube(vector<double>& Vector)
{
    double dot_product = 0.0;
    /*calculate the dot product between the current vector and the random vector of the hash function*/
    for(int i = 0;i < RandomVector.size(); i++)
        dot_product += (Vector[i]) * RandomVector[i];
    /*return the h(p) normalized by getting its absolute value*/
    dot_product += T;
    dot_product /= Window;

    if(dot_product > 0)
        return '1';

    return '0';   
}
