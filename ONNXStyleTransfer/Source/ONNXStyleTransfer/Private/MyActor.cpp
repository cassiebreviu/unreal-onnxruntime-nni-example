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
        TArray<FColor> SrcData = myNetwork->URunModel(wrappedImage);
        //create texture for new int array
        TextureFromImage_Internal(224, 224, SrcData, false);

	}
}

void AMyActor::TextureFromImage_Internal(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor>& SrcData, const bool UseAlpha)
{
    // Create the texture
    auto MyNeuralTexture = UTexture2D::CreateTransient(
        SrcWidth,
        SrcHeight,
        //PF_B8G8R8A8
        PF_R8G8B8A8
    );

    // Lock the texture so it can be modified
    uint8* MipData = static_cast<uint8*>(MyNeuralTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

    // Create base mip.
    uint8* DestPtr = NULL;
    const FColor* SrcPtr = NULL;
    for (int32 y = 0; y < SrcHeight; y++)
    {
        DestPtr = &MipData[(SrcHeight - 1 - y) * SrcWidth * sizeof(FColor)];
        SrcPtr = const_cast<FColor*>(&SrcData[(SrcHeight - 1 - y) * SrcWidth]);
        for (int32 x = 0; x < SrcWidth; x++)
        {
            *DestPtr++ = SrcPtr->B;
            *DestPtr++ = SrcPtr->G;
            *DestPtr++ = SrcPtr->R;
            if (UseAlpha)
            {
                *DestPtr++ = SrcPtr->A;
            }
            else
            {
                *DestPtr++ = 0xFF;
            }
            SrcPtr++;
        }
    }

    // Unlock the texture
    MyNeuralTexture->PlatformData->Mips[0].BulkData.Unlock();
    MyNeuralTexture->UpdateResource();

    //creating a texture as an input to a custom post processing effect
}
