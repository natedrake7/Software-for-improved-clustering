#include "hypercube.hpp"

int main(int argc,char** argv)
{
    char* input_file;char* query_file;char* output_file; //Declarations for variables
    int HashDimension,MaxValuesChecked,Probes,NearestNeighbors,Range;
    vector<vector<byte>> train_set,test_set;

    /*Get arguments from input*/
    GetArgs(argc,argv,&input_file,&query_file,&output_file,&HashDimension,&MaxValuesChecked,&Probes,&NearestNeighbors,&Range);

    /*Seed rand so the values of R later in hypercube are random*/
   // srand(static_cast<unsigned int>(time(nullptr)));
   
    /*Initialize HyperCube and hash train set*/
    Hypercube* hypercube;
    InitializeProgram(HashDimension,MaxValuesChecked,Probes,NearestNeighbors,Range,input_file,&hypercube,train_set);
    string input,output;
    while(1)
    {
        Program(NearestNeighbors,input_file,query_file,output_file,&hypercube,train_set,test_set);
        cout<<"Execution for query file: "<<query_file<<" finished!"<<endl
            <<"If you would like to exit type exit,else type the query file you wish to input: ";
        cin>>input;
        if(input == "exit" || input == "Exit")
            break;
        strcpy(query_file,input.c_str());
        cout<<"If you would like to specify a new output file,type the name of the file"<<endl
            <<"Else type continue (Note that the contents of the current file will be overwritten): ";
        cin>>output;
        if(!(output == "continue" || output == "Continue"))
            strcpy(output_file,output.c_str());
        test_set.clear();
    }
    delete hypercube; 
}


int Program(int NearestNeighbors,char* input_file,char* query_file,char* output_file,Hypercube** hypercube,vector<vector<byte>>& train_set,vector<vector<byte>>& test_set)
{
    Point test_point;
    vector<double> Distances,Time,BruteForceTime;

    /*Open test set*/
    OpenFile(query_file,&test_set,true);
    /*Open output file*/
    ofstream MyFile(output_file);
    double BruteForce_duration,Hypercube_duration;
    Point point;
    for(int i = 0; i < test_set.size();i++)
    {
        /*Print into the output file*/
        /*Should be 10 images*/
        test_point.Vector = &test_set[i];
        test_point.PointID = i;
        BruteForceTime.push_back(BruteForce(&Distances,train_set,test_set[i],NearestNeighbors));
        Time.push_back((*hypercube)->QueryHash(test_point));

        (*hypercube)->WriteToFile(MyFile,test_point,Distances,BruteForceTime,Time);
        Distances.clear();
    }
    /*Close file*/
    MyFile.close();
    return 0;
}


void InitializeProgram(int HashDimension,int MaxValuesChecked,int Probes,int NearestNeighbors,int Range,char* input_file,Hypercube** hypercube,vector<vector<byte>>& train_set)
{
    /*Open train set*/
    OpenFile(input_file,&train_set,false);
    train_set.erase(train_set.begin() + 2000,train_set.end());
    /*Initialize HyperCube class item*/
    *hypercube = new Hypercube(HashDimension,MaxValuesChecked,Probes,NearestNeighbors,Range);
    Point point;
    for(int i = 0;i < train_set.size();i++)
    {
        /*hash points in hypercube hash table*/
        point.Vector = &train_set[i];
        point.PointID = i;
        (*hypercube)->Hash(point); 
    }
}