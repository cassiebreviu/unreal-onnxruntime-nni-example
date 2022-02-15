#include "MyNeuralNetwork.h"

#include <fstream>
#include <iostream>
#include <array>
#include <string>
#include <filesystem>

#include "OpenCVHelper.h"
#include "PreOpenCVHeaders.h"
#include <ThirdParty/OpenCV/include/opencv2/imgproc.hpp>
#include <ThirdParty/OpenCV/include/opencv2/highgui/highgui.hpp>
#include <ThirdParty/OpenCV/include/opencv2/core.hpp>
#include "PostOpenCVHeaders.h"

using namespace std;
using namespace cv;
UNeuralNetwork* Network; 


UMyNeuralNetwork::UMyNeuralNetwork()
{
	// Create the UNeuralNetwork object
	Network = NewObject<UNeuralNetwork>((UObject*)GetTransientPackage(), UNeuralNetwork::StaticClass());
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
void UMyNeuralNetwork::URunModel()
{
    
    //Running inference
    // Fill input neural tensor
    TArray<float> InArray = UGetInArray(); // Assumed initialized with data and that InArray.Num() == Network->Num()
    Network->SetInputFromArrayCopy(InArray); // Equivalent: Network->SetInputFromVoidPointerCopy(InArray.GetData());
    UE_LOG(LogTemp, Display, TEXT("Input tensor: %s."), *Network->GetInputTensor().ToString());
    // Run UNeuralNetwork
    Network->Run();
    //Read and print OutputTensor
    const FNeuralTensor& OutputTensor = Network->GetOutputTensor();
    UE_LOG(LogTemp, Display, TEXT("Output tensor: %s."), *OutputTensor.ToString());
}

TArray<float> UMyNeuralNetwork::UGetInArray()
{
    Mat image = imread("/Content/Imgs/sample.png");
    //cvtColor(image, image, cv::COLOR_BGR2RGB);


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

