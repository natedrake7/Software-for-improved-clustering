#include "lsh.hpp"

int main(int argc,char** argv)
{
    char* input_file;char* query_file;char* output_file; //Declarations for variables
    int K,L,N,R;
    vector<vector<byte>> train_set,test_set;
    /*Get arguments from input*/
    GetArgs(argc,argv,&input_file,&query_file,&output_file,&K,&L,&N,&R);
    /*Seed rand so the values of R later in LSH are random*/
   // srand(static_cast<unsigned int>(time(nullptr)));
    /*Initialize LSH and hash train set*/
    LSH* Lsh;
    InitializeProgram(K,L,N,R,input_file,&Lsh,train_set);
    string input,output;
    while(1)
    {
        Program(N,input_file,query_file,output_file,&Lsh,train_set,test_set);
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
    delete Lsh; 
}


int Program(int N,char* input_file,char* query_file,char* output_file,LSH** Lsh,vector<vector<byte>>& train_set,vector<vector<byte>>& test_set)
{
    Point test_point;
    vector<double> Distances,Time,BruteForceTime;
    /*Open test set*/
    OpenFile(query_file,&test_set,true);
    /*Open output file*/
    ofstream MyFile(output_file);
    double BruteForce_duration,LSH_duration;
    for(int i = 0; i < test_set.size();i++)
    {
        /*Print into the output file*/
        /*Should be 10 images*/
        BruteForceTime.push_back(BruteForce(&Distances,train_set,test_set[i],N));
        Time.push_back((*Lsh)->Hash(test_set[i],i,true));
        test_point.Vector = &test_set[i];
        test_point.PointID = i;

        (*Lsh)->WriteToFile(MyFile,test_point,Distances,BruteForceTime,Time);
        Distances.clear();
        Time.clear();
        BruteForceTime.clear();
    }


    /*Close file*/
    MyFile.close();
    return 0;
}

void InitializeProgram(int K,int L,int N,int R,char* input_file,LSH** Lsh,vector<vector<byte>>& train_set)
{
    /*Open train set*/
    OpenFile(input_file,&train_set,false);
    train_set.erase(train_set.begin() + 2000,train_set.end());
    /*Initialize LSH class item*/
    *Lsh = new LSH(L,K,R,N,train_set.size(),train_set[0].size());
    /*Hash the training set in the LSH item*/
    for(int i = 0;i < train_set.size();i++)
    {
        /*Runs on for a long time*/
        (*Lsh)->Hash(train_set[i],i,false); 
    }
}