// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Instance/EchoGameInstance.h"
#include "EchoRecorderComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTECHO_API UEchoRecorderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
    UEchoRecorderComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    // ��ȭ ���
    UPROPERTY(BlueprintReadWrite, Category = "Echo")
    bool bIsRecordingMode;

    // ��� ���
    UPROPERTY(BlueprintReadWrite, Category = "Echo")
    bool bIsPlaybackMode;

    // ����� ������ (�޾Ƹ���)
    UPROPERTY(BlueprintReadWrite, Category = "Echo")
    FEchoRecordingData PlaybackData;

    // ��� ����
    UPROPERTY(BlueprintReadWrite, Category = "Echo")
    float PlaybackStartTime;

    UPROPERTY(BlueprintReadWrite, Category = "Echo")
    int32 CurrentPlaybackIndex;

    // ��ȭ �ֱ� (�����Ӹ��ٴ� �ʹ� ����)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo")
    float RecordingInterval;

    float TimeSinceLastRecord;

    // �������Ʈ ȣ�� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Echo")
    void StartRecordingMode();

    UFUNCTION(BlueprintCallable, Category = "Echo")
    void StartPlaybackMode(FEchoRecordingData Data);

    UFUNCTION(BlueprintCallable, Category = "Echo")
    void StopRecordingMode();

    UFUNCTION(BlueprintCallable, Category = "Echo")
    void StopPlaybackMode();

    UFUNCTION(BlueprintCallable, Category = "Echo")
    void RecordActionEvent(FString ActionName);

    // 새로 추가 ↓↓↓
    UFUNCTION(BlueprintCallable, Category = "Echo")
    void UpdatePlaybackTime(float MasterTime);

    UPROPERTY(BlueprintReadWrite, Category = "Echo")
    float StartTimeOffset;
protected:
    virtual void BeginPlay() override;

private:
    void HandleRecording(float DeltaTime);
    void HandlePlayback(float DeltaTime);

    UEchoGameInstance* GameInstance;
};
