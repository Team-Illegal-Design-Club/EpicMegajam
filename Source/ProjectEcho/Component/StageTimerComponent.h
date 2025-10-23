

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StageTimerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, float, TimeRemaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerExpired);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTECHO_API UStageTimerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStageTimerComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    // 스테이지별 시간 제한 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
    float StageTimeLimit;

    // 남은 시간
    UPROPERTY(BlueprintReadOnly, Category = "Timer")
    float TimeRemaining;

    // 타이머 활성화 여부
    UPROPERTY(BlueprintReadWrite, Category = "Timer")
    bool bTimerActive;

    // 경고 시간 (빨간색 표시 시작)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
    float WarningThreshold;

    // 이벤트 디스패처
    UPROPERTY(BlueprintAssignable, Category = "Timer")
    FOnTimerUpdated OnTimerUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Timer")
    FOnTimerExpired OnTimerExpired;

    // 블루프린트 함수
    UFUNCTION(BlueprintCallable, Category = "Timer")
    void StartTimer(float TimeInSeconds);

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void StopTimer();

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void PauseTimer();

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void ResumeTimer();

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void AddTime(float SecondsToAdd);

    UFUNCTION(BlueprintPure, Category = "Timer")
    FString GetFormattedTime() const;

    UFUNCTION(BlueprintPure, Category = "Timer")
    bool IsInWarningPhase() const;

protected:
    virtual void BeginPlay() override;

private:
    bool bIsPaused;

		
	
};
