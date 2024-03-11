import FileHandler
from autoencoder import Autoencoder
from sklearn.model_selection import train_test_split

#Get arguments from command line
dataset,query,output_dataset,output_query = FileHandler.HandleArguments()

#Get dataframes from the input and query files
train_array,indexes = FileHandler.ReadInput(dataset)
query_array,query_indexes = FileHandler.ReadInput(query,True)


#Split the data in training and validation
X_train,X_Val = train_test_split(train_array,test_size=0.2)

model = Autoencoder(latent_dim=30)

model.train(X_train,X_Val)

train_images = []
query_images = []
latent_dim_images = model.encode(train_array)
for i in range(len(latent_dim_images)):
    train_images.append((latent_dim_images[i] * 255.0).flatten())

FileHandler.WriteOutput(output_dataset,train_images)

latent_dim_images = model.encode(query_array)
for i in range(len(latent_dim_images)):
    query_images.append((query_indexes[i],(latent_dim_images[i] * 255.0).flatten()))

FileHandler.WriteOutput(output_query,query_images)
    





