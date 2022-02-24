// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NeuralNetwork.h"

#include "PreOpenCVHeaders.h"
#include "OpenCVHelper.h"
#include <ThirdParty/OpenCV/include/opencv2/imgproc.hpp>
#include <ThirdParty/OpenCV/include/opencv2/highgui/highgui.hpp>
#include <ThirdParty/OpenCV/include/opencv2/core.hpp>
#include "PostOpenCVHeaders.h"


#include "MyNeuralNetwork.generated.h"

/**
 * 
 */
UCLASS()
class ONNXSTYLETRANSFER_API UMyNeuralNetwork : public UNeuralNetwork
{
	GENERATED_BODY()

	public:
		// Sets default values for this actor's properties
		UMyNeuralNetwork();
		TArray<FColor> URunModel(cv::Mat image);
		TArray<float> UPreProcessImage(cv::Mat image);
	
};
