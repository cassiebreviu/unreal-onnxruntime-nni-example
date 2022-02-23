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
    camera = NewObject<UCameraComponent>();
	// Create the UNeuralNetwork object
	Network = NewObject<UNeuralNetwork>((UObject*)GetTransientPackage(), UNeuralNetwork::StaticClass());
    //TODO: fix model loading
	const FString ONNXModelFilePath = TEXT("/Content/Models/mosaic-9.onnx");
	if (Network->Load(ONNXModelFilePath))
	{
		// Pick between option a or b
		// Option a) Set to GPU
		if (Network->IsGPUSupported())
			Network->SetDeviceType(ENeuralDeviceType::GPU);
		// Option b) Set to CPU
		Network->SetDeviceType(ENeuralDeviceType::CPU);
		UE_LOG(LogTemp, Log, TEXT("Neural Network loaded successfully."))
	}
	// Check that the network was successfully loaded
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UNeuralNetwork could not loaded from %s."), *ONNXModelFilePath);
	}

}
void UMyNeuralNetwork::URunModel(cv::Mat image)
{
    
    //Running inference
    // Fill input neural tensor
    TArray<float> InArray = UPreProcessImage(image); // Assumed initialized with data and that InArray.Num() == Network->Num()
    Network->SetInputFromVoidPointerCopy(InArray.GetData()); // Equivalent: Network->SetInputFromVoidPointerCopy(InArray.GetData());
    UE_LOG(LogTemp, Display, TEXT("Input tensor: %s."), *Network->GetInputTensor().ToString());
    // Run UNeuralNetwork
    Network->Run();
    //Read and print OutputTensor
    const FNeuralTensor& OutputTensor = Network->GetOutputTensor();
    UE_LOG(LogTemp, Display, TEXT("Output tensor: %s."), *OutputTensor.ToString());

    //TODO: apply output image to camera view post process
    // taking the output and creating a texture as an input to a custom post processing effect. 
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



