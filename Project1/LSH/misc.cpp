#include "misc.hpp"


/*Basic Pnorm for LSH algorithm*/
double PNorm(vector<byte>& A,vector<byte>& B,int p)
{
    double sum = 0.0,diff = 0.0;

    /*Both vectors must be of the same size*/
    if(A.size() != B.size())
    {
        cout<<"Vector sizes do not match!"<<endl
            <<"A has size: "<<A.size()<<endl
            <<"B has size: "<<B.size()<<endl;
        return -1.0;
    }

    /*iterate over both of the vectors*/
    for(int i = 0;i < A.size(); i++)
    {
        /*find the absolute of the difference between the 2 points*/
        diff = abs((double)A[i] - (double)B[i]);
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
bool ComparisonFunctionByDistance(const Point& A,const Point& B)
{
    /*Compare the distances to evaluate which one is closer to the query element*/
    return A.Distance < B.Distance;
}

bool ComparisonFunctionByCluster(const Point& A,const Point& B)
{
    return A.ClusterID < B.ClusterID;
}

/*Basic brute force algorithm that compares all points to the query point and returns the N closest ones*/
double BruteForce(vector<double>* Distances,vector<vector<byte>> Points,vector<byte> QueryPoint,int NearestNeighbors)
{   
    /*Count ms it takes to complete*/
    auto start_time = chrono::high_resolution_clock().now();
    /*For all points in the dataset*/
    for(int i = 0;i < Points.size(); i++)
        Distances->push_back(PNorm(Points[i],QueryPoint,2));
    /*Sort the points*/
    sort(Distances->begin(),Distances->end());

    Distances->erase(Distances->begin() + NearestNeighbors,Distances->end());
        
    auto end_time = chrono::high_resolution_clock().now();

    chrono::duration<double> duration = end_time - start_time;
    /*Return the duration of the brute force search*/
    return duration.count() * 1000;
}


/*These functions are used to get the arguments from the command line as well as*/
            /*to open the binary files given as input*/
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



/*Gets all the arguments from the command line (doesnt matter the order they are used)*/
/*Used by LSH Main*/
void GetArgs(int argc,char** argv,char** input_file,char** query_file,char** output_file,int* K,int* L,int* N,int* R)
{   
    /*Booleans to mark which value will be passed*/
    bool input_bool = false,query_bool = false,K_bool = false,
         L_bool = false,output_bool = false,N_bool = false,R_bool = false;
    
    if(argc < 2) //will need to change to 7 later
    {
        cout<<"Please enter arguments!"<<endl;
        exit(-1);
    }

    *K = 4,*L=5,*N=1,*R = 10000; //Default values are passed
                                 //so even if user doesnt input them,these will be used

    for(int i=0;i < argc; i++)
    {
        if(input_bool) //If the corresponding bool is true
        {
            *input_file = argv[i]; //Get the appropriate value from the argv array
            input_bool = false; //mark the boolean false so it is excluded next time
        }
        else if(query_bool)
        {
            *query_file = argv[i];
            query_bool = false;
        }
        else if(K_bool)
        {
            *K = atoi(argv[i]);
            K_bool = false;
        }
        else if(L_bool)
        {
            *L = atoi(argv[i]);
            L_bool = false;
        }
        else if(output_bool)
        {
            *output_file = argv[i];
            output_bool = false;
        }
        else if(N_bool)
        {
            *N = atoi(argv[i]);
            N_bool = false;
        }
        else if(R_bool)
        {
            *R = atoi(argv[i]);
            R_bool = false;   
        }
        else if(strcmp(argv[i],"-d") == 0) //this checks if the argv arg is the parameter before the actual value we want
            input_bool = true; //if so,mark the appropriate boolean as true so on the next iteration we access the correct value
        else if(strcmp(argv[i],"-q") == 0)
            query_bool = true;
        else if(strcmp(argv[i],"-k") == 0)
            K_bool = true;
        else if(strcmp(argv[i],"-L") == 0)
            L_bool = true; 
        else if(strcmp(argv[i],"-o") == 0)
            output_bool = true;  
        else if(strcmp(argv[i],"-N") == 0)
            N_bool = true;  
        else if(strcmp(argv[i],"-R") == 0)
            R_bool = true;                   
    }
}

/*Gets all the arguments from the command line (doesnt matter the order they are used)*/
void GetArgs(int argc,char** argv,char** input_file,char** query_file,char** output_file,int* HashDimension,int* MaxValuesChecked,int* Probes,int* NearestNeighbors,int* Range)
{   
    /*Booleans to mark which value will be passed*/
    bool input_bool = false,query_bool = false,HashDimension_bool = false,
         MaxValuesChecked_bool = false,output_bool = false,NearestNeighbors_bool = false,Range_bool = false,Probes_bool = false;
    
    if(argc < 2) //will need to change to 7 later
    {
        cout<<"Please enter arguments!"<<endl;
        exit(-1);
    }

    *HashDimension = 14,*MaxValuesChecked = 10,*Probes = 2,*NearestNeighbors = 1,*Range = 10000; //Default values are passed
                                 //so even if user doesnt input them,these will be used

    for(int i=0;i < argc; i++)
    {
        if(input_bool) //If the corresponding bool is true
        {
            *input_file = argv[i]; //Get the appropriate value from the argv array
            input_bool = false; //mark the boolean false so it is excluded next time
        }
        else if(query_bool)
        {
            *query_file = argv[i];
            query_bool = false;
        }
        else if(HashDimension_bool)
        {
            *HashDimension = atoi(argv[i]);
            HashDimension_bool = false;
        }
        else if(MaxValuesChecked_bool)
        {
            *MaxValuesChecked = atoi(argv[i]);
            MaxValuesChecked_bool = false;
        }
        else if(output_bool)
        {
            *output_file = argv[i];
            output_bool = false;
        }
        else if(NearestNeighbors_bool)
        {
            *NearestNeighbors = atoi(argv[i]);
            NearestNeighbors_bool = false;
        }
        else if(Probes_bool)
        {
            *Probes = atoi(argv[i]);
            Probes_bool = false;   
        }
        else if(Range_bool)
        {
            *Range = atoi(argv[i]);
            Range_bool = false;
        }
        else if(strcmp(argv[i],"-d") == 0) //this checks if the argv arg is the parameter before the actual value we want
            input_bool = true; //if so,mark the appropriate boolean as true so on the next iteration we access the correct value
        else if(strcmp(argv[i],"-q") == 0)
            query_bool = true;
        else if(strcmp(argv[i],"-k") == 0)
            HashDimension_bool = true;
        else if(strcmp(argv[i],"-M") == 0)
            MaxValuesChecked_bool = true; 
        else if(strcmp(argv[i],"-o") == 0)
            output_bool = true;  
        else if(strcmp(argv[i],"-N") == 0)
            NearestNeighbors_bool = true;  
        else if(strcmp(argv[i],"-R") == 0)
            Range_bool = true;
        else if(strcmp(argv[i],"-probes") == 0)
            Probes_bool = true;         
    }
}

/*Opens a binary file and returns a 2D vector with all its images*/
void OpenFile(char* filename,vector<vector<byte>>* images,bool test)
{
    Header header; //A header with the file metadata
    ifstream file(filename, ios::binary); //open the file as binary
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