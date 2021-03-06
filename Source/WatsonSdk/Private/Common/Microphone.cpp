#pragma once

#include "Common/Microphone.h"

UMicrophone::UMicrophone()
{
    VoiceCapture = FVoiceModule::Get().CreateVoiceCapture();
    uint32 AvailableVoiceData;
    if (VoiceCapture->GetCaptureState(AvailableVoiceData) == EVoiceCaptureState::UnInitialized)
    {
        VoiceCapture->Init(16000, 1);
    }
    VoiceCaptureBuffer.SetNumUninitialized(10000);
	VoiceCaptureSize = 0;
}

void UMicrophone::StartRecording()
{
	VoiceCaptureBuffer.Empty();
	VoiceCaptureSize = 0;
	VoiceCapture->Start();
}

void UMicrophone::StopRecording()
{
	VoiceCapture->Stop();
}

TArray<uint8> UMicrophone::GetRecording()
{
	VoiceCaptureBuffer.Shrink();
	return VoiceCaptureBuffer;
}


void UMicrophone::Tick(float DeltaTime)
{
	uint32 AvailableVoiceDataInBytes = 0;
	if (VoiceCapture.IsValid() && VoiceCapture->GetCaptureState(AvailableVoiceDataInBytes) == EVoiceCaptureState::Type::Ok && AvailableVoiceDataInBytes > 0)
	{
		TArray<uint8> TempCaptureBuffer;
		TempCaptureBuffer.SetNumUninitialized(AvailableVoiceDataInBytes);

		uint32 ReadVoiceDataBytes = 0;
		VoiceCapture->GetVoiceData(TempCaptureBuffer.GetData(), AvailableVoiceDataInBytes, ReadVoiceDataBytes);

		VoiceCaptureBuffer.Append(TempCaptureBuffer.GetData(), ReadVoiceDataBytes);
		VoiceCaptureSize += ReadVoiceDataBytes;
	}
}

bool UMicrophone::IsTickableInEditor() const
{
	return false;
}

bool UMicrophone::IsTickableWhenPaused() const
{
	return false;
}

bool UMicrophone::IsTickable() const
{
	return true;
}

TStatId UMicrophone::GetStatId() const
{
	return TStatId();
}

void UMicrophone::BeginDestroy()
{
	Super::BeginDestroy();
	VoiceCapture->Shutdown();
	VoiceCaptureBuffer.Empty();
	VoiceCaptureSize = 0;
}
