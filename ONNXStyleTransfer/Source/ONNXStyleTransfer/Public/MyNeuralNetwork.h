// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NeuralNetwork.h"
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
		void URunModel();
		TArray<float> UGetInArray();
	
};
