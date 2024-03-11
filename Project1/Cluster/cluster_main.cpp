#include "cluster.hpp"

int main(int argc,char** argv)
{
    char* input_file;char* output_file;char* latent_input;
    char* method;Config config;bool complete,Latent = false;
    vector <vector<byte>> Points,LatentPoints;

    srand(static_cast<unsigned int>(time(nullptr)));

    GetArgs(argc,argv,&input_file,&latent_input,&output_file,&complete,&method,&config,&Latent);

    
    if(Latent)
    {
        if(OpenLatentFile(latent_input,&LatentPoints,false) == -1)
        {
            cout<<"Failed to read Latent input file!"<<endl;
            return -1;
        }
        LatentPoints.erase(LatentPoints.begin() + 10000,LatentPoints.end());
    }
    OpenFile(input_file,&Points,false);
    Points.erase(Points.begin() + 10000,Points.end());

    Cluster* cluster;
    if(Latent)
    {
        cluster = new Cluster(config.Clusters,LatentPoints);
        cluster->InitializeClusters(LatentPoints);
    }
    else
    {
        cluster = new Cluster(config.Clusters,Points);
        cluster->InitializeClusters(Points);
    }
    /*Get starting range(since its not given)*/
    int Range = 0;
    cout<<"Please enter the starting Range: ";
    cin>>Range;

    /*Initialize Lsh or hypercube (if used)*/
    /*Lloyd's doesnt need initialization*/
    if(strcmp(method,"LSH") == 0)
        cluster->InitializeLSH(config.Hash_functions,config.Hash_tables,Range);
    else if(strcmp(method,"Hypercube") == 0)
        cluster->InitializeHyperCube(config.Hypercube_dim,config.Probes,config.Max_m_hypercube,Range);
    
    int check,i = 0;
    auto start_time = chrono::high_resolution_clock().now();

    while(1)
    {
        cluster->Update();
        check = cluster->Converge();
        i++;
        if(check == 1)
            break;
        cluster->Assignment(method);
    }

    auto end_time = chrono::high_resolution_clock().now();
    chrono::duration<double> duration = end_time - start_time;
    cluster->Outliers();

    if(complete && !Latent)
        cluster->WriteToFile(duration.count(),method,output_file,complete);
    else if(Latent)
        cluster->WriteToFileLatent(duration.count(),method,output_file,Points);
    else
        cluster->WriteToFile(duration.count(),method,output_file);

    cout<<method<<" algorithm converged after "<<i<<" iterations and in : "<<duration.count()<<" seconds."<<endl;

    delete cluster;
    return 0;
}

void GetArgs(int argc,char** argv,char** input_file,char** latent_input,char** output_file,bool* complete,char** method,Config* config,bool* Latent)
{   
    int check;
    if(argc < 2) //will need to change to 7 later
    {
        cout<<"Please enter arguments!"<<endl;
        exit(-1);
    }


    for(int i=0;i < argc; i++)
    {
        if(strcmp(argv[i],"-i") == 0) //this checks if the argv arg is the parameter before the actual value we want
        {
            *input_file = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i],"-oi") == 0)
        {
            *latent_input = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i],"-c") == 0)
        {
            if(ReadConfig(argv[i + 1],config) == -1)
                exit(-1);
            i++;
        }
        else if(strcmp(argv[i],"-o") == 0)
        {
            *output_file = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i],"-complete") == 0)
        {
            *complete = true;
            i++;
        }
        else if(strcmp(argv[i],"-m") == 0)
        {
            *method = argv[i + 1];
            i++;
        }       
        else if(strcmp(argv[i],"-latent") == 0)
        {
            *Latent = true;
            i++;
        }     
    }
}

int ReadConfig(char* filename,Config* config)
{
    ifstream file(filename);
    /*Error message if the file fails to open*/
    if(!file.is_open())
    {
        cout<<"Error: Failed to open configuration file!"<<endl;
        return -1;
    }

    char buffer[512];
    /*Read from the file*/
    while(file >> buffer)
    {
        /*if the item in the buffer is a string with the value clusters(only 1 exists)*/
        if(strstr(buffer,"clusters"))
        {
            /*get the next item(the integer)*/
            file >> buffer;
            config->Clusters = atoi(buffer);
            /*if atoi returns 0,it is a string and not a number(so it is the next value,which is the string of the hash table integer)*/
            if(config->Clusters == 0) 
            {
                /*Ask for user input to get the number of centroids*/
                cout<<"No K medians value provided!"<<endl
                    <<"Please enter a value for the K medians: ";
                cin>>config->Clusters;
            }
        }
        /*As before,only one string in the config contains the hash_tables needle*/
        if(strstr(buffer,"hash_tables"))
        {
            /*Get the value*/
            file >> buffer;
            config->Hash_tables = atoi(buffer);
            /*if the atoi value is 0,it is not a number*/
            /*So assign default values*/
            if(config->Hash_tables == 0)
                config->Hash_tables = 3;
        }
        /*The same pattern as above continues here and below*/
        if(strstr(buffer,"hash_functions"))
        {
            file >> buffer;
            config->Hash_functions = atoi(buffer);
            /*if the atoi value is 0,it is not a number*/
            /*So assign default values*/
            if(config->Hash_functions == 0)
                config->Hash_functions = 4;
        }
        if(strstr(buffer,"M_hypercube"))
        {
            file >> buffer;
            config->Max_m_hypercube = atoi(buffer);
            /*if the atoi value is 0,it is not a number*/
            /*So assign default values*/            
            if(config->Max_m_hypercube == 0)
                config->Max_m_hypercube = 10;
        }
        if(strstr(buffer,"dimensions"))
        {
            file >> buffer;
            config->Hypercube_dim = atoi(buffer);
            /*if the atoi value is 0,it is not a number*/
            /*So assign default values*/            
            if(config->Hypercube_dim == 0)
                config->Hypercube_dim = 3;
        }
        if(strstr(buffer,"probes"))
        {
            file >> buffer;
            config->Probes = atoi(buffer);
            /*if the atoi value is 0,it is not a number*/
            /*So assign default values*/            
            if(config->Probes == 0)
                config->Probes = 2;
        }
    }
    /*Close the file*/
    file.close();

    return 0;
}