#include "misc.hpp"


/*Basic Pnorm for LSH algorithm*/
double PNorm(vector<byte>* A,vector<byte>* B,int p)
{
    double sum = 0.0,diff = 0.0;
    /*Both vectors must be of the same size*/
    if(A->size() != B->size())
    {
        cout<<"Vector sizes do not match!"<<endl;
        cout<<"A has size: "<<A->size()<<"while B has size: "<<B->size()<<endl;
        return -1.0;
    }

    /*iterate over both of the vectors*/
    for(int i = 0;i < A->size(); i++)
    {
        /*find the absolute of the difference between the 2 points*/
        diff = abs((double)(*A)[i] - (double)(*B)[i]);
        /*if p is 0,we essentially check for no 0 values*/
        if(p == 0 && diff > 1e-6)
            sum += 1.0;
        else /*else find the diff to the power of p and add it to the sum*/
            sum += pow(diff,p);
    }
    /*if p is 0 retun just the sum*/
    if(p == 0)
        return sum;
    /*else return the n-nth root of the sum*/
    return pow(sum,1.0/p);
}

/*Implementation for Cluster file*/
double PNorm(vector<double>& A,vector<byte>& B,int p)
{
    double sum = 0.0,diff = 0.0;

    /*Both vectors must be of the same size*/
    if(A.size() != B.size())
    {
        cout<<"Vector sizes do not match!"<<endl;
        return -1.0;
    }

    /*iterate over both of the vectors*/
    for(int i = 0;i < A.size(); i++)
    {
        /*find the absolute of the difference between the 2 points*/
        diff = abs(A[i] - (double)B[i]);
        /*if p is 0,we essentially check for no 0 values*/
        if(p == 0 && diff > 1e-6)
            sum += 1.0;
        else /*else find the diff to the power of p and add it to the sum*/
            sum += pow(diff,p);
    }
    /*if p is 0 retun just the sum*/
    if(p == 0)
        return sum;
    /*else return the n-nth root of the sum*/
    return pow(sum,1.0/p);
}

/*Comparison function called in sort*/
bool ComparisonFunctionByDistance(const GraphPoint* A,const GraphPoint* B)
{
    /*Compare the distances to evaluate which one is closer to the query element*/
    return A->Distance < B->Distance;
}

bool CandidatesComparisonFunciton(GraphPoint* A, GraphPoint* B) { return A->Distance < B->Distance ;}

/*Basic brute force algorithm that compares all points to the query point and returns the N closest ones*/
double BruteForce(vector<double>* Distances,vector<vector<byte>> Points,vector<byte> QueryPoint,int NearestNeighbors)
{   
    /*Count ms it takes to complete*/
    auto start_time = chrono::high_resolution_clock().now();
    /*For all points in the dataset*/
    for(int i = 0;i < Points.size(); i++)
        Distances->push_back(PNorm(&Points[i],&QueryPoint,2));
    /*Sort the points*/
    sort(Distances->begin(),Distances->end());

    Distances->erase(Distances->begin() + NearestNeighbors,Distances->end());
        
    auto end_time = chrono::high_resolution_clock().now();

    chrono::duration<double> duration = end_time - start_time;
    /*Return the duration of the brute force search*/
    return duration.count() * 1000;
}

double BruteForce(vector<tuple<double,int>>* Distances,vector<vector<byte>> Points,vector<byte> QueryPoint,int NearestNeighbors)
{   
    /*Count ms it takes to complete*/
    auto start_time = chrono::high_resolution_clock().now();
    /*For all points in the dataset*/
    for(int i = 0;i < Points.size(); i++)
        Distances->push_back(tuple(PNorm(&Points[i],&QueryPoint,2),i));
    /*Sort the points*/
    sort(Distances->begin(),Distances->end(),CandidatesComparisonFuncitonForTuple);

    Distances->erase(Distances->begin() + NearestNeighbors,Distances->end());
        
    auto end_time = chrono::high_resolution_clock().now();

    chrono::duration<double> duration = end_time - start_time;
    /*Return the duration of the brute force search*/
    return duration.count() * 1000;
}

bool CandidatesComparisonFuncitonForTuple(tuple<double,int>& A,tuple<double,int>& B)
{
    return get<0>(A) < get<0>(B);
}

/*These functions are used to get the arguments from the command line as well as*/
            /*to open the binary files given as input*/
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



/*Gets all the arguments from the command line (doesnt matter the order they are used)*/
void GetArgs(int argc,char** argv,char** input_file,char** query_file,char** output_file,int* GraphNearestNeighbors,int* Extensions,
             int* RandomRestarts,int* NearestNeighbors,int* TankCandidates,char** method,bool* latent,char** latent_input_file,char** latent_query_file)
{    
    if(argc < 2) //will need to change to 7 later
    {
        cout<<"Please enter arguments!"<<endl;
        exit(-1);
    }

    *GraphNearestNeighbors = 50,*Extensions = 30,*RandomRestarts = 1,*NearestNeighbors = 1,*TankCandidates = 20; //Default values are passed
                                 //so even if user doesnt input them,these will be use

    for(int i=0;i < argc; i++)
    {   
        if(strcmp(argv[i],"-d") == 0) //this checks if the argv arg is the parameter before the actual value we want
        {
            *input_file = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i],"-q") == 0)
        {
            *query_file = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i],"-k") == 0)
        {
            *GraphNearestNeighbors = atoi(argv[i + 1]);
            i++;
        }
        else if(strcmp(argv[i],"-E") == 0)
        {
            *Extensions = atoi(argv[i + 1]);
            i++;
        }
        else if(strcmp(argv[i],"-o") == 0)
        {
            *output_file = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i],"-R") == 0)
        {
            *RandomRestarts = atoi(argv[i + 1]);
            i++;
        }
        else if(strcmp(argv[i],"-N") == 0)
        {
            *NearestNeighbors = atoi(argv[i + 1]);
            i++;
        }
        else if(strcmp(argv[i],"-l") == 0)
        {
            *TankCandidates = atoi(argv[i + 1]);
            i++;
        }
        else if(strcmp(argv[i],"-latent") == 0)
        {
            *latent = true;
            i++;
        }
        else if(strcmp(argv[i],"-od") == 0)
        {
            *latent_input_file = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i],"-oq") == 0)
        {
            *latent_query_file = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i],"-m") == 0)
        {
            if(atoi(argv[i + 1]) == 1)
                strcpy(*method,"GNNS");
            else if(atoi(argv[i + 1]) == 2)
                strcpy(*method,"MRNG");
            else
            {
                cout<<"Invalid method!"<<endl;
                exit(-1);
            }

        }
    }
}


/*Opens a binary file and returns a 2D vector with all its images*/
int OpenFile(char* filename,vector<vector<byte>>* images,bool test)
{
    Header header; //A header with the file metadata
    ifstream file(filename, ios::binary); //open the file as binary
    if(!file.is_open())
    {
        cout<<"Failed to open file: "<<filename<<endl
            <<"Make sure the file exists!"<<endl;
        return -1;
    }
    file.read(reinterpret_cast<char*>(&header), sizeof(Header)); //get the header bytes

    header.magic_number = htonl(header.magic_number); //Convert the values to 32 bit
    header.num_images = htonl(header.num_images);
    header.num_rows = htonl(header.num_rows);
    header.num_columns = htonl(header.num_columns);

    if(test)
        header.num_images = 10;
    vector<byte> temp(header.num_columns*header.num_rows); //a temp vector to store the pixels

    while (!file.eof()) 
    {
        file.read(reinterpret_cast<char*>(temp.data()), header.num_columns*header.num_rows*sizeof(byte)); //Read an entire image as a vector

        images->push_back(temp); //Store it in the images vector

        if (images->size() == header.num_images) //if we have all the images 
            break; //break the loop
    }
    file.close(); //close the file descriptor
    return 0;
}

/*Opens a binary file and returns a 2D vector with all its images*/
int OpenLatentFile(char* filename,vector<vector<byte>>* images,bool test)
{
    LatentHeader header; //A header with the file metadata
    ifstream file(filename, ios::binary); //open the file as binary
    if(!file.is_open())
    {
        cout<<"Failed to open file: "<<filename<<endl
            <<"Make sure the file exists!"<<endl;
        return -1;
    }
    file.read(reinterpret_cast<char*>(&header), sizeof(Header)); //get the header bytes

    header.magic_number = htonl(header.magic_number); //Convert the values to 32 bit
    header.num_images = htonl(header.num_images);
    header.image_length = htonl(header.image_length);


    if(test)
        header.num_images = 10;

    vector<byte> temp(header.image_length); //a temp vector to store the pixels
    while (!file.eof()) 
    {
        file.read(reinterpret_cast<char*>(temp.data()), header.image_length*sizeof(byte)); //Read an entire image as a vector

        images->push_back(temp); //Store it in the images vector
        if(temp.size() != header.image_length)
        {
            cout<<"ERROR!Invalid input size!"<<endl;
            exit(-1);
        }

        if (images->size() == header.num_images) //if we have all the images 
            break; //break the loop
    }
    file.close(); //close the file descriptor
    return 0;
}

void ClearVectors(vector<double>& time,vector<double>& BruteForceTime,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances,
                  vector<double>& TrueBruteForceTime,vector<double>& BruteForceDistance)
{
    ExpansionPoints.clear();
    TrueDistances.clear();
    time.clear();
    BruteForceTime.clear();
    TrueBruteForceTime.clear();
    BruteForceDistance.clear();
}

void ClearVectors(vector<double>& time,vector<double>& BruteForceTime,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances)
{
    ExpansionPoints.clear();
    TrueDistances.clear();
    time.clear();
    BruteForceTime.clear();
}

int CheckFileExistance(char* filename,bool QueryFile,bool Latent,vector<vector<byte>>& Vector)
{
    string input_file_string,input;
    //If file is a query file and its not in the latent dimension
    if(QueryFile && !Latent)
    {
        //check if open fails,and require user to either exit or input a correct query file,and extract 10 firt items
        while(OpenFile(filename,&Vector,QueryFile) == -1)
        {
            cout<<"Incorrect query file was given"<<endl
                <<"Please give a new query file or type exit to exit: ";
            cin>>input;
            if(input == "exit" || input == "Exit")
                return -1;
            strcpy(filename,input.c_str());
        }
        return 0;
    }
    else if(Latent)
    {
        while(OpenLatentFile(filename,&Vector,QueryFile) == -1)
        {
            cout<<"Incorrect latent file was given"<<endl
                <<"Please give a new query file or type exit to exit: ";
            cin>>input;
            if(input == "exit" || input == "Exit")
                return -1;
            strcpy(filename,input.c_str());
        }
        return 0;
    }
    while(OpenFile(filename,&Vector,QueryFile) == -1)
    {
        cout<<"Please give a correct input file or type exit to exit: ";
        cin>>input_file_string;
        if(input_file_string == "exit" || input_file_string == "Exit")
            return -1;
        strcpy(filename,input_file_string.c_str());
    }
    return 0;
}

int GetNewFiles(char** query_file,char** output_file)
{
    string input,output;
    cout<<"Execution for query file: "<<*query_file<<" finished!"<<endl
        <<"If you would like to exit type exit,else type the query file you wish to input: ";
    cin>>input;
    if(input == "exit" || input == "Exit")
         return -1;
    strcpy(*query_file,input.c_str());
    cout<<"If you would like to specify a new output file,type the name of the file"<<endl
        <<"Else type continue (Note that the contents of the current file will be overwritten): ";
    cin>>output;
    if(!(output == "continue" || output == "Continue"))
        strcpy(*output_file,output.c_str());
    return 0;
}

void WriteToFile(ostream& MyFile,vector<double>& time,vector<double>& BruteForceTime,char* method,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances,GraphPoint* QueryPoint)
{
    MyFile<<"Query: "<<QueryPoint->PointID<<endl; //Write Query Point ID
    double AverageApproxDist = 0.0,AverageTrueDist = 0.0;
    for(int i = 0;i < ExpansionPoints.size(); i++)//Write Input Point ID,Distance by method,Distance by BruteForce
    {
        MyFile<<"Nearest Neighbor-"<<i<<": "<<get<0>(ExpansionPoints[i])->PointID<<endl
              <<"distanceApproximate: "<<get<1>(ExpansionPoints[i])<<endl
              <<"distanceTrue: "<<TrueDistances[i]<<endl;
        AverageApproxDist += get<1>(ExpansionPoints[i]);
        AverageTrueDist += TrueDistances[i];
    }
    double AlgorithmAverageTime = 0.0,BruteForceAverageTime = 0.0;
    if(time.size() != BruteForceTime.size()) //This should be true all the time but since we find the average value in the same loop below
        cout<<"Incorrect Distances sizes!"<<endl; //we should check,but nothing changes
    for(int i = 0;i < time.size();i++)
    {
        AlgorithmAverageTime += time[i];
        BruteForceAverageTime += BruteForceTime[i];
    }
    AverageApproxDist /= ExpansionPoints.size();
    AverageTrueDist /= TrueDistances.size();

    double AF = AverageApproxDist / AverageTrueDist; //MAF is the fraction of the approximate nearest neighbor / true nearest neighbor
    AlgorithmAverageTime /= time.size(); //Compute average time
    BruteForceAverageTime /= BruteForceTime.size();//for brute force as well as method
    MyFile<<"tAverageApproximate: "<<AlgorithmAverageTime<<"ms"<<endl
          <<"tAverageTrue: "<<BruteForceAverageTime<<"ms"<<endl
          <<"AF: "<<AF<<endl;
}

void WriteToFile(ostream& MyFile,vector<double>& time,vector<double>& BruteForceTime,char* method,vector<tuple<GraphPoint*, double>>& ExpansionPoints,
                vector<tuple<double,int>>& BruteForceLatentToNormalSpace,vector<double>& TrueBruteForceDistances,vector<double>& TrueBruteForceTime,GraphPoint* QueryPoint,vector<vector<byte>>& Images)
{
    MyFile<<"Query: "<<QueryPoint->PointID<<endl; //Write Query Point ID
    double LatentAverageDistance = 0.0,AverageBruteForceDistance = 0.0;
    double ApproxDistNormalSpace = 0.0,TrueDistLatentToNormalSpace = 0.0;
    for(int i = 0;i < ExpansionPoints.size(); i++)//Write Input Point ID,Distance by method,Distance by BruteForce
    {
        ApproxDistNormalSpace = PNorm(QueryPoint->Vector,&Images[get<0>(ExpansionPoints[i])->PointID],2);
        TrueDistLatentToNormalSpace = PNorm(QueryPoint->Vector,&Images[get<1>(BruteForceLatentToNormalSpace[i])],2);
        MyFile<<"Nearest Neighbor-"<<i<<": "<<get<0>(ExpansionPoints[i])->PointID<<endl
              <<"distanceApproximate: "<<ApproxDistNormalSpace<<endl
              <<"distanceLatentTrue: "<<TrueDistLatentToNormalSpace<<endl
              <<"distanceTrue: "<<TrueBruteForceDistances[i]<<endl;
        LatentAverageDistance += ApproxDistNormalSpace;
        AverageBruteForceDistance += TrueBruteForceDistances[i];
    }

    LatentAverageDistance /= ExpansionPoints.size();
    AverageBruteForceDistance /= TrueBruteForceDistances.size();

    double AlgorithmAverageTime = 0.0,BruteForceAverageTime = 0.0,TrueBruteForceAverageTime = 0.0;
    if(time.size() != BruteForceTime.size()) //This should be true all the time but since we find the average value in the same loop below
        cout<<"Incorrect Distances sizes!"<<endl; //we should check,but nothing changes

    for(int i = 0;i < time.size();i++)
    {
        AlgorithmAverageTime += time[i];
        BruteForceAverageTime += BruteForceTime[i];
        TrueBruteForceAverageTime += TrueBruteForceTime[i];
    }

    double AF = LatentAverageDistance / AverageBruteForceDistance; //MAF is the fraction of the approximate nearest neighbor / true nearest neighbor
    AlgorithmAverageTime /= time.size(); //Compute average time
    BruteForceAverageTime /= BruteForceTime.size();//for brute force as well as method
    TrueBruteForceAverageTime /= TrueBruteForceTime.size();

    MyFile<<"tAverageApproximate: "<<AlgorithmAverageTime<<"ms"<<endl
          <<"tAverageLatentTrue: "<<BruteForceAverageTime<<"ms"<<endl
          <<"tAverageTrue: "<<TrueBruteForceAverageTime<<"ms"<<endl
          <<"AF: "<<AF<<endl;
}