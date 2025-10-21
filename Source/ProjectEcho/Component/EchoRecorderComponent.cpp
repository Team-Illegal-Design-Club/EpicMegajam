#include "Component/EchoRecorderComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UEchoRecorderComponent::UEchoRecorderComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    bIsRecordingMode = false;
    bIsPlaybackMode = false;
    RecordingInterval = 0.033f; // ~30fps ��ȭ
    TimeSinceLastRecord = 0.0f;
    CurrentPlaybackIndex = 0;
}

void UEchoRecorderComponent::BeginPlay()
{
    Super::BeginPlay();

    GameInstance = Cast<UEchoGameInstance>(GetWorld()->GetGameInstance());
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("EchoGameInstance not found!"));
    }
}

void UEchoRecorderComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsRecordingMode)
    {
        HandleRecording(DeltaTime);
    }
    else if (bIsPlaybackMode)
    {
        HandlePlayback(DeltaTime);
    }
}

void UEchoRecorderComponent::HandleRecording(float DeltaTime)
{
    if (!GameInstance || !GameInstance->bIsRecording) return;

    TimeSinceLastRecord += DeltaTime;

    // ���� ���ݸ��ٸ� ��� (����ȭ)
    if (TimeSinceLastRecord >= RecordingInterval)
    {
        FTransform CurrentTransform = GetOwner()->GetActorTransform();
        GameInstance->RecordTransform(CurrentTransform);

        TimeSinceLastRecord = 0.0f;
    }
}

void UEchoRecorderComponent::HandlePlayback(float DeltaTime)
{
    if (PlaybackData.RecordedTransforms.Num() == 0) return;

    float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
    float PlaybackTime = CurrentTime - PlaybackStartTime;

    // ���� �ð��� �´� ������ ã��
    while (CurrentPlaybackIndex < PlaybackData.RecordedTimestamps.Num() - 1 &&
        PlaybackData.RecordedTimestamps[CurrentPlaybackIndex + 1] <= PlaybackTime)
    {
        CurrentPlaybackIndex++;
    }

    // ��ȭ ���� �����ϸ� ����
    if (CurrentPlaybackIndex >= PlaybackData.RecordedTransforms.Num() - 1)
    {
        CurrentPlaybackIndex = 0;
        PlaybackStartTime = CurrentTime;
        return;
    }

    // �� ������ ���� ����
    if (CurrentPlaybackIndex < PlaybackData.RecordedTransforms.Num() - 1)
    {
        int32 NextIndex = CurrentPlaybackIndex + 1;

        float CurrentTimestamp = PlaybackData.RecordedTimestamps[CurrentPlaybackIndex];
        float NextTimestamp = PlaybackData.RecordedTimestamps[NextIndex];
        float TimeDiff = NextTimestamp - CurrentTimestamp;

        if (TimeDiff > 0.0f)
        {
            float Alpha = (PlaybackTime - CurrentTimestamp) / TimeDiff;
            Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

            // Transform ����
            FTransform Current = PlaybackData.RecordedTransforms[CurrentPlaybackIndex];
            FTransform Next = PlaybackData.RecordedTransforms[NextIndex];

            FTransform Interpolated;
            Interpolated.SetLocation(FMath::Lerp(Current.GetLocation(), Next.GetLocation(), Alpha));
            Interpolated.SetRotation(FQuat::Slerp(Current.GetRotation(), Next.GetRotation(), Alpha));
            Interpolated.SetScale3D(FMath::Lerp(Current.GetScale3D(), Next.GetScale3D(), Alpha));

            GetOwner()->SetActorTransform(Interpolated);
        }
    }
}

void UEchoRecorderComponent::StartRecordingMode()
{
    bIsRecordingMode = true;
    bIsPlaybackMode = false;
    TimeSinceLastRecord = 0.0f;

    UE_LOG(LogTemp, Log, TEXT("Started recording mode on %s"), *GetOwner()->GetName());
}

void UEchoRecorderComponent::StartPlaybackMode(FEchoRecordingData Data)
{
    bIsRecordingMode = false;
    bIsPlaybackMode = true;

    PlaybackData = Data;
    CurrentPlaybackIndex = 0;
    PlaybackStartTime = UGameplayStatics::GetTimeSeconds(this);

    UE_LOG(LogTemp, Log, TEXT("Started playback mode on %s with %d frames"),
        *GetOwner()->GetName(), PlaybackData.RecordedTransforms.Num());
}

void UEchoRecorderComponent::StopRecordingMode()
{
    bIsRecordingMode = false;
    UE_LOG(LogTemp, Log, TEXT("Stopped recording mode"));
}

void UEchoRecorderComponent::StopPlaybackMode()
{
    bIsPlaybackMode = false;
    UE_LOG(LogTemp, Log, TEXT("Stopped playback mode"));
}

void UEchoRecorderComponent::RecordActionEvent(FString ActionName)
{
    if (bIsRecordingMode && GameInstance)
    {
        GameInstance->RecordAction(ActionName);
    }
}