#include <iostream>
#include <vector>


using namespace std;

class HashFunction{
    private:
        vector<double> RandomVector;
        unsigned int Window;
        double T;

    public:
        HashFunction(){};
        HashFunction(vector<double>& randomVector,unsigned int window,double t);
        int Hash(vector<byte>& Vector);
        int Hash(vector<double>& Vector);
        char HashCube(vector<byte>& Vector);
        char HashCube(vector<double>& Vector);
};
