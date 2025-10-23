#include "Component/StageTimerComponent.h"

UStageTimerComponent::UStageTimerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    StageTimeLimit = 60.0f; // 기본 60초
    TimeRemaining = 0.0f;
    bTimerActive = false;
    bIsPaused = false;
    WarningThreshold = 10.0f;
}

void UStageTimerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UStageTimerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bTimerActive || bIsPaused) return;

    TimeRemaining -= DeltaTime;

    // UI 업데이트 브로드캐스트
    OnTimerUpdated.Broadcast(TimeRemaining);

    // 시간 종료
    if (TimeRemaining <= 0.0f)
    {
        TimeRemaining = 0.0f;
        bTimerActive = false;
        OnTimerExpired.Broadcast();

        UE_LOG(LogTemp, Warning, TEXT("Timer expired!"));
    }
}

void UStageTimerComponent::StartTimer(float TimeInSeconds)
{
    StageTimeLimit = TimeInSeconds;
    TimeRemaining = TimeInSeconds;
    bTimerActive = true;
    bIsPaused = false;

    UE_LOG(LogTemp, Log, TEXT("Timer started: %.1f seconds"), TimeInSeconds);
}

void UStageTimerComponent::StopTimer()
{
    bTimerActive = false;
    TimeRemaining = 0.0f;

    UE_LOG(LogTemp, Log, TEXT("Timer stopped"));
}

void UStageTimerComponent::PauseTimer()
{
    bIsPaused = true;
    UE_LOG(LogTemp, Log, TEXT("Timer paused"));
}

void UStageTimerComponent::ResumeTimer()
{
    bIsPaused = false;
    UE_LOG(LogTemp, Log, TEXT("Timer resumed"));
}

void UStageTimerComponent::AddTime(float SecondsToAdd)
{
    TimeRemaining += SecondsToAdd;
    UE_LOG(LogTemp, Log, TEXT("Added %.1f seconds. New time: %.1f"),
        SecondsToAdd, TimeRemaining);
}

FString UStageTimerComponent::GetFormattedTime() const
{
    int32 Minutes = FMath::FloorToInt(TimeRemaining / 60.0f);
    int32 Seconds = FMath::FloorToInt(FMath::Fmod(TimeRemaining, 60.0f));

    return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

bool UStageTimerComponent::IsInWarningPhase() const
{
    return bTimerActive && TimeRemaining <= WarningThreshold && TimeRemaining > 0.0f;
}