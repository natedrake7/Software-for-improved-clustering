from keras.layers import Input, Conv2D, MaxPooling2D, UpSampling2D,Dense,Reshape,Flatten
from keras.models import Model
from keras.callbacks import History
import matplotlib.pyplot as plt
import os
import re

class Autoencoder:
    def __init__(self, input_shape=(28, 28, 1), latent_dim=10):
        self.input_shape = input_shape
        self.latent_dim = latent_dim
        self.autoencoder_model = self.__build_autoencoder()
        self.encoder_model = self.__build_encoder_model()

    def __build_autoencoder(self):
        input_img = Input(shape=self.input_shape)

        # Encoder
        conv1 = Conv2D(32, (3, 3), activation='gelu', padding='same')(input_img)
        pool1 = MaxPooling2D(pool_size=(2, 2))(conv1)
        conv2 = Conv2D(64, (3, 3), activation='gelu', padding='same')(pool1)
        pool2 = MaxPooling2D(pool_size=(2, 2))(conv2)
        conv3 = Conv2D(128, (3, 3), activation='gelu', padding='same')(pool2)
        flatten = Flatten()(conv3)
        dense_latent = Dense(self.latent_dim, activation='gelu')(flatten)
        #10 pixels

        # Decoder
        dense_upsample = Dense(128 * 7 * 7, activation='gelu')(dense_latent)
        reshape = Reshape((7, 7, 128))(dense_upsample)
        conv4 = Conv2D(64, (3, 3), activation='gelu', padding='same')(reshape)
        up1 = UpSampling2D((2, 2))(conv4)
        conv5 = Conv2D(32, (3, 3), activation='gelu', padding='same')(up1)
        up2 = UpSampling2D((2, 2))(conv5)
        decoded = Conv2D(1, (3, 3), activation='sigmoid', padding='same')(up2)

        # Autoencoder model
        autoencoder_model = Model(input_img, decoded)
        autoencoder_model.compile(optimizer='adam', loss='binary_crossentropy',metrics=['accuracy'])

        return autoencoder_model

    def train(self, x_train,x_val, epochs=40, batch_size=64):
        history = History()

        self.autoencoder_model.fit(x_train,x_train, epochs=epochs, batch_size=batch_size,validation_data=[x_val,x_val],callbacks=[history], shuffle=True,verbose=2)

        training_loss = history.history['loss']
        validation_loss = history.history['val_loss']

        epochs_range = range(1, epochs + 1)

        self.PlotLearningCurve(training_loss,validation_loss,epochs_range)

    #Returns image in latent space
    def __build_encoder_model(self):
        input_img = Input(shape=self.input_shape)

        conv1 = Conv2D(32, (3, 3), activation='relu', padding='same')(input_img)
        pool1 = MaxPooling2D(pool_size=(2, 2))(conv1)
        conv2 = Conv2D(64, (3, 3), activation='relu', padding='same')(pool1)
        pool2 = MaxPooling2D(pool_size=(2, 2))(conv2)
        conv3 = Conv2D(128, (3, 3), activation='relu', padding='same')(pool2)
        flatten = Flatten()(conv3)
        dense_latent = Dense(self.latent_dim, activation='relu')(flatten)

        encoder_model = Model(input_img, dense_latent)
        return encoder_model
    
    def encode(self, x):
        return self.encoder_model.predict(x)

    def PlotLearningCurve(self, training_data, validation_data, epochs_range, labels=['Training Loss', 'Validation Loss'], title='Training and Validation Loss', ylabel='Loss'):
        
        image_folder = './Images'
        latest_number = 0
        
        existing_files = [file for file in os.listdir(image_folder) if re.match(r'loss-fig(\d+)\.png', file)] #get all the files in dir that match the 'loss-fig<number?.png' name

        if existing_files:
            for file in existing_files:
                numeric_part = re.search(r'loss-fig(\d+)\.png', file).group(1) #check the number of each file
                numeric_value = int(numeric_part)
                if numeric_value > latest_number:   #get the latest numeric value which is also the max one
                    latest_number = numeric_value
        else:
            latest_number = 0 #if no existing files, start the counter from 0
            
        plot_counter = latest_number + 1

        output_file = f'{image_folder}/loss-fig{plot_counter}.png' #create the filename for the new image file
        
        plt.figure(figsize=(8, 4))

        plt.plot(epochs_range,training_data,label=labels[0])
        plt.plot(epochs_range,validation_data,label=labels[1])

        plt.title(title)

        plt.xlabel('Epochs')
        plt.ylabel(ylabel)

        plt.legend()
        plt.savefig(output_file)

