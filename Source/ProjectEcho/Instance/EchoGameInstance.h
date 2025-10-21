// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EchoGameInstance.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FEchoRecordingData
{
    GENERATED_BODY()

    // Transform 기록 배열
    UPROPERTY(BlueprintReadWrite)
    TArray<FTransform> RecordedTransforms;

    // 각 Transform의 타임스탬프
    UPROPERTY(BlueprintReadWrite)
    TArray<float> RecordedTimestamps;

    // 인터랙션 이벤트 (버튼 누름 등)
    UPROPERTY(BlueprintReadWrite)
    TArray<FString> RecordedActions;

    // 액션 타임스탬프
    UPROPERTY(BlueprintReadWrite)
    TArray<float> ActionTimestamps;

    // 메타데이터
    UPROPERTY(BlueprintReadWrite)
    int32 DeathNumber;

    UPROPERTY(BlueprintReadWrite)
    FLinearColor EchoColor;

    UPROPERTY(BlueprintReadWrite)
    float TotalDuration;

    FEchoRecordingData()
        : DeathNumber(0)
        , EchoColor(FLinearColor::White)
        , TotalDuration(0.0f)
    {
    }
};

UCLASS()
class PROJECTECHO_API UEchoGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UEchoGameInstance();

    // 현재 스테이지 정보
    UPROPERTY(BlueprintReadWrite, Category = "Echo|Stage")
    FString CurrentStageID;

    UPROPERTY(BlueprintReadWrite, Category = "Echo|Stage")
    int32 CurrentDeathCount;

    UPROPERTY(BlueprintReadWrite, Category = "Echo|Stage")
    int32 MaxDeathsPerStage;

    // 현재 스테이지의 모든 녹화 데이터
    UPROPERTY(BlueprintReadWrite, Category = "Echo|Recording")
    TArray<FEchoRecordingData> StageRecordings;

    // 현재 녹화 중인 데이터 (임시)
    UPROPERTY(BlueprintReadWrite, Category = "Echo|Recording")
    FEchoRecordingData CurrentRecording;

    UPROPERTY(BlueprintReadWrite, Category = "Echo|Recording")
    bool bIsRecording;

    UPROPERTY(BlueprintReadWrite, Category = "Echo|Recording")
    float RecordingStartTime;

    // 블루프린트 호출 가능한 함수들
    UFUNCTION(BlueprintCallable, Category = "Echo|Recording")
    void StartRecording();

    UFUNCTION(BlueprintCallable, Category = "Echo|Recording")
    void StopRecording();

    UFUNCTION(BlueprintCallable, Category = "Echo|Recording")
    void RecordTransform(FTransform Transform);

    UFUNCTION(BlueprintCallable, Category = "Echo|Recording")
    void RecordAction(FString ActionName);

    UFUNCTION(BlueprintCallable, Category = "Echo|Stage")
    void IncrementDeathCount();

    UFUNCTION(BlueprintCallable, Category = "Echo|Stage")
    bool ShouldHardReset();

    UFUNCTION(BlueprintCallable, Category = "Echo|Stage")
    void ResetStageData();

    UFUNCTION(BlueprintCallable, Category = "Echo|Stage")
    void ClearAllRecordings();

    UFUNCTION(BlueprintPure, Category = "Echo|Stage")
    int32 GetRemainingDeaths() const;
};
