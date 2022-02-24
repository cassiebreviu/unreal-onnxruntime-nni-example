#include "MyNeuralNetwork.h"

#include <fstream>
#include <iostream>
#include <array>
#include <string>
#include <filesystem>


#include "PreOpenCVHeaders.h"
#include "OpenCVHelper.h"
#include <ThirdParty/OpenCV/include/opencv2/imgproc.hpp>
#include <ThirdParty/OpenCV/include/opencv2/highgui/highgui.hpp>
#include <ThirdParty/OpenCV/include/opencv2/core.hpp>
#include "PostOpenCVHeaders.h"

#include "MediaBundle.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Texture2D.h"

using namespace std;
using namespace cv;
UNeuralNetwork* Network; 
UCameraComponent* camera;


UMyNeuralNetwork::UMyNeuralNetwork()
{
	// Create the UNeuralNetwork object
	Network = NewObject<UNeuralNetwork>((UObject*)GetTransientPackage(), UNeuralNetwork::StaticClass());
    // Load the UNeuralNetwork object from a UNeuralNetwork UAsset
    //const FString NetworkUAssetFilePath = TEXT("'/Game/Models/mosaic-9.mosaic-9'");
    //Network = LoadObject<UNeuralNetwork>((UObject*)GetTransientPackage(), *NetworkUAssetFilePath);
    // Check that the network was successfully loaded
    //ensureMsgf(Network->IsLoaded(), TEXT("UNeuralNetwork could not loaded from %s."), *NetworkUAssetFilePath);
    // Optionally set to CPU/GPU mode. This step is optional, if not called, it will use the device type that was saved on the loaded UAsset
    // Network->SetDeviceType(ENeuralDeviceType::CPU);


    //TODO: fix model loading
	const FString &ONNXModelFilePath = TEXT("C:/code/Unreal Projects/ONNXStyleTransfer/Content/Models/mosaic-9.onnx");
    Network->SetDeviceType(ENeuralDeviceType::CPU);
	if (Network->Load(ONNXModelFilePath))
	{
    	UE_LOG(LogTemp, Log, TEXT("Neural Network loaded successfully."))
	}
	// Check that the network was successfully loaded
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UNeuralNetwork could not loaded from %s."), *ONNXModelFilePath);
	}

}
TArray<FColor> UMyNeuralNetwork::URunModel(cv::Mat image)
{
    
    //Running inference
    // Fill input neural tensor
    const TArray<float> InArray = UPreProcessImage(image); // Assumed initialized with data and that InArray.Num() == Network->Num()
    Network->SetInputFromArrayCopy(InArray); // Equivalent: Network->SetInputFromVoidPointerCopy(InArray.GetData());
    UE_LOG(LogTemp, Display, TEXT("Input tensor: %s."), *Network->GetInputTensor().ToString());
    // Run UNeuralNetwork
    Network->Run();
    //Read and print OutputTensor
    //const FNeuralTensor& OutputTensor = Network->GetOutputTensor();
    TArray<float> OutputTensor = Network->GetOutputTensor().GetArrayCopy<float>();
    //UE_LOG(LogTemp, Display, TEXT("Output tensor: %s."), OutputTensor.ToString());

    //create array of the correct pixel values from results
    TArray<FColor> results;
    for (int i = 0; i < OutputTensor.Num(); i+=3) {
       
       //change all neg num positive int
       int R =  floor(abs(OutputTensor[i]));
       int G =  floor(abs(OutputTensor[i+1]));
       int B =  floor(abs(OutputTensor[i+2]));
       int A = 0;

       //if greater than 255 set to 255
       if (R > 255) {
           R = 255;
       }
       if (G > 255) {
           G = 255;
       }
       if (B > 255) {
           B = 255;
       }
       auto color = FColor(R, G, B, A);
       //add to result
       results.Add(color);
    }
    return results;
}



TArray<float> UMyNeuralNetwork::UPreProcessImage(cv::Mat image)
{   

    if (image.empty()) {
        return {};
    }
    // resize
    resize(image, image, Size(224, 224));

    // reshape to 1D
    image = image.reshape(1, 1);

    // uint_8, [0, 255] -> float, [0, 1]
    vector<float> vec;
    image.convertTo(vec, CV_32FC1, 1. / 255);

    // HWC -> CHW
    TArray<float> output;
    for (size_t ch = 0; ch < 3; ++ch) {
        for (size_t i = ch; i < vec.size(); i += 3) {
            output.Emplace(vec[i]);
        }
    }
    return output;
}