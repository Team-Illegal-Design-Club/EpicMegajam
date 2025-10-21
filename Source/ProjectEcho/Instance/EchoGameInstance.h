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

    // Transform ��� �迭
    UPROPERTY(BlueprintReadWrite)
    TArray<FTransform> RecordedTransforms;

    // �� Transform�� Ÿ�ӽ�����
    UPROPERTY(BlueprintReadWrite)
    TArray<float> RecordedTimestamps;

    // ���ͷ��� �̺�Ʈ (��ư ���� ��)
    UPROPERTY(BlueprintReadWrite)
    TArray<FString> RecordedActions;

    // �׼� Ÿ�ӽ�����
    UPROPERTY(BlueprintReadWrite)
    TArray<float> ActionTimestamps;

    // ��Ÿ������
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

    // ���� �������� ����
    UPROPERTY(BlueprintReadWrite, Category = "Echo|Stage")
    FString CurrentStageID;

    UPROPERTY(BlueprintReadWrite, Category = "Echo|Stage")
    int32 CurrentDeathCount;

    UPROPERTY(BlueprintReadWrite, Category = "Echo|Stage")
    int32 MaxDeathsPerStage;

    // ���� ���������� ��� ��ȭ ������
    UPROPERTY(BlueprintReadWrite, Category = "Echo|Recording")
    TArray<FEchoRecordingData> StageRecordings;

    // ���� ��ȭ ���� ������ (�ӽ�)
    UPROPERTY(BlueprintReadWrite, Category = "Echo|Recording")
    FEchoRecordingData CurrentRecording;

    UPROPERTY(BlueprintReadWrite, Category = "Echo|Recording")
    bool bIsRecording;

    UPROPERTY(BlueprintReadWrite, Category = "Echo|Recording")
    float RecordingStartTime;

    // �������Ʈ ȣ�� ������ �Լ���
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
