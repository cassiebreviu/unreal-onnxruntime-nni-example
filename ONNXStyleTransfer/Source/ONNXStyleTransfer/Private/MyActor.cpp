// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"
#include "MyNeuralNetwork.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

#include "PreOpenCVHeaders.h"
#include "OpenCVHelper.h"
#include <ThirdParty/OpenCV/include/opencv2/imgproc.hpp>
#include <ThirdParty/OpenCV/include/opencv2/highgui/highgui.hpp>
#include <ThirdParty/OpenCV/include/opencv2/core.hpp>
#include "PostOpenCVHeaders.h"

// Sets default values
AMyActor::AMyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	auto SceneCaptureComponent = NewObject<USceneCaptureComponent2D>();
	auto TextureRenderTarget = NewObject<UTextureRenderTarget2D>();

	TextureRenderTarget->InitCustomFormat(512, 512, EPixelFormat::PF_B8G8R8A8, true);
	TextureRenderTarget->UpdateResourceImmediate();
	SceneCaptureComponent->TextureTarget = TextureRenderTarget;
	SceneCaptureComponent->CaptureScene();

	auto RenderTargetResource = TextureRenderTarget->GameThread_GetRenderTargetResource();

	if (RenderTargetResource) {
		TArray<FColor> buffer;
		RenderTargetResource->ReadPixels(buffer);

		cv::Mat wrappedImage(TextureRenderTarget->GetSurfaceHeight(), TextureRenderTarget->GetSurfaceWidth(), CV_8UC4, buffer.GetData());
	
		//std::string OutputFile(TCHAR_TO_UTF8(*OutputVideoFile));
		//cv::imwrite(OutputFile, wrappedImage);

		//create network and run model
		UMyNeuralNetwork* myNetwork = NewObject<UMyNeuralNetwork>();
		myNetwork->URunModel(wrappedImage);

	}
}

