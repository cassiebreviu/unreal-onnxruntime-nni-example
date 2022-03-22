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

//#include "MediaBundle.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Texture2D.h"
using namespace std;
using namespace cv;


UMyNeuralNetwork::UMyNeuralNetwork()
{
	Network = nullptr;
}
TArray<FColor> UMyNeuralNetwork::URunModel(cv::Mat image)
{
	// Create result array
	TArray<FColor> results;

	// Create Network object if null
	if (Network == nullptr) {
		//Running inference
		Network = NewObject<UNeuralNetwork>((UObject*)GetTransientPackage(), UNeuralNetwork::StaticClass());
		//create array of the correct pixel values from results
		

		// Load model from file.
		const FString& ONNXModelFilePath = TEXT("C:/code/unreal-onnxruntime-styletransfer/Models/mosaic-9.onnx");
		// Set Device
		Network->SetDeviceType(ENeuralDeviceType::CPU);
		// Check that the network was successfully loaded
		if (Network->Load(ONNXModelFilePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Neural Network loaded successfully."))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UNeuralNetwork could not loaded from %s."), *ONNXModelFilePath);
			return results;
		}
	}

	// Fill input neural tensor
	const TArray<float> InArray = UPreProcessImage(image); // Assumed initialized with data and that InArray.Num() == Network->Num()
	Network->SetInputFromArrayCopy(InArray); // Equivalent: Network->SetInputFromVoidPointerCopy(InArray.GetData());
	UE_LOG(LogTemp, Display, TEXT("Input tensor: %s."), *Network->GetInputTensor().ToString());
	// Run UNeuralNetwork
	Network->Run();
	UE_LOG(LogTemp, Log, TEXT("Neural Network Inference complete."))
	//Read and print OutputTensor
	//const FNeuralTensor& OutputTensor = Network->GetOutputTensor();
	TArray<float> OutputTensor = Network->GetOutputTensor().GetArrayCopy<float>();


	for (int i = 0; i < OutputTensor.Num(); i += 3) {

		//change all neg num positive int
		int R = floor(abs(OutputTensor[i]));
		int G = floor(abs(OutputTensor[i + 1]));
		int B = floor(abs(OutputTensor[i + 2]));
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

	UE_LOG(LogTemp, Log, TEXT("Results created successfully."))
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