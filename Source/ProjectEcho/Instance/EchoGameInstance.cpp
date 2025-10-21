// Fill out your copyright notice in the Description page of Project Settings.


#include "Instance/EchoGameInstance.h"
#include "Kismet/GameplayStatics.h"


UEchoGameInstance::UEchoGameInstance()
{
    CurrentStageID = TEXT("Tutorial");
    CurrentDeathCount = 0;
    MaxDeathsPerStage = 5;
    bIsRecording = false;
    RecordingStartTime = 0.0f;
}

void UEchoGameInstance::StartRecording()
{
    if (bIsRecording)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already recording!"));
        return;
    }

    // 새 녹화 초기화
    CurrentRecording = FEchoRecordingData();
    CurrentRecording.DeathNumber = CurrentDeathCount;

    // 랜덤 색상 생성 (구분용)
    CurrentRecording.EchoColor = FLinearColor::MakeRandomColor();

    bIsRecording = true;
    RecordingStartTime = UGameplayStatics::GetTimeSeconds(this);

    UE_LOG(LogTemp, Log, TEXT("Started recording for death #%d"), CurrentDeathCount);
}

void UEchoGameInstance::StopRecording()
{
    if (!bIsRecording)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not currently recording!"));
        return;
    }

    bIsRecording = false;

    // 총 녹화 시간 계산
    float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
    CurrentRecording.TotalDuration = CurrentTime - RecordingStartTime;

    // 녹화 데이터를 배열에 추가
    StageRecordings.Add(CurrentRecording);

    UE_LOG(LogTemp, Log, TEXT("Stopped recording. Duration: %.2f seconds, Frames: %d"),
        CurrentRecording.TotalDuration,
        CurrentRecording.RecordedTransforms.Num());
}

void UEchoGameInstance::RecordTransform(FTransform Transform)
{
    if (!bIsRecording) return;

    float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
    float RelativeTime = CurrentTime - RecordingStartTime;

    CurrentRecording.RecordedTransforms.Add(Transform);
    CurrentRecording.RecordedTimestamps.Add(RelativeTime);
}

void UEchoGameInstance::RecordAction(FString ActionName)
{
    if (!bIsRecording) return;

    float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
    float RelativeTime = CurrentTime - RecordingStartTime;

    CurrentRecording.RecordedActions.Add(ActionName);
    CurrentRecording.ActionTimestamps.Add(RelativeTime);

    UE_LOG(LogTemp, Log, TEXT("Recorded action: %s at time %.2f"), *ActionName, RelativeTime);
}

void UEchoGameInstance::IncrementDeathCount()
{
    CurrentDeathCount++;
    UE_LOG(LogTemp, Warning, TEXT("Player died. Death count: %d/%d"),
        CurrentDeathCount, MaxDeathsPerStage);
}

bool UEchoGameInstance::ShouldHardReset()
{
    return CurrentDeathCount > MaxDeathsPerStage;
}

void UEchoGameInstance::ResetStageData()
{
    CurrentDeathCount = 0;
    ClearAllRecordings();
    UE_LOG(LogTemp, Log, TEXT("Stage data reset"));
}

void UEchoGameInstance::ClearAllRecordings()
{
    StageRecordings.Empty();
    UE_LOG(LogTemp, Log, TEXT("All recordings cleared"));
}

int32 UEchoGameInstance::GetRemainingDeaths() const
{
    return MaxDeathsPerStage - CurrentDeathCount;
}