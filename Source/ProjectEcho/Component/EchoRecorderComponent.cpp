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
    StartTimeOffset = 0.0f;
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

void UEchoRecorderComponent::UpdatePlaybackTime(float MasterTime)
{
    if (!bIsPlaybackMode || PlaybackData.RecordedTransforms.Num() == 0)
    {
        return;
    }

    // 이 메아리의 상대 시간 계산
    float RelativeTime = MasterTime - StartTimeOffset;

    // 녹화 시작 전이면 대기
    if (RelativeTime < 0.0f)
    {
        return;
    }

    // 녹화가 끝났으면 마지막 위치에 고정
    if (RelativeTime > PlaybackData.TotalDuration)
    {
        if (PlaybackData.RecordedTransforms.Num() > 0)
        {
            int32 LastIndex = PlaybackData.RecordedTransforms.Num() - 1;
            GetOwner()->SetActorTransform(PlaybackData.RecordedTransforms[LastIndex]);
        }
        return;
    }

    // RelativeTime에 해당하는 프레임 찾기
    int32 FrameIndex = 0;
    for (int32 i = 0; i < PlaybackData.RecordedTimestamps.Num() - 1; i++)
    {
        if (PlaybackData.RecordedTimestamps[i] <= RelativeTime &&
            RelativeTime < PlaybackData.RecordedTimestamps[i + 1])
        {
            FrameIndex = i;
            break;
        }
    }

    // 두 프레임 사이 보간
    if (FrameIndex < PlaybackData.RecordedTransforms.Num() - 1)
    {
        float T1 = PlaybackData.RecordedTimestamps[FrameIndex];
        float T2 = PlaybackData.RecordedTimestamps[FrameIndex + 1];
        float TimeDiff = T2 - T1;

        if (TimeDiff > 0.0f)
        {
            float Alpha = (RelativeTime - T1) / TimeDiff;
            Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

            FTransform Transform1 = PlaybackData.RecordedTransforms[FrameIndex];
            FTransform Transform2 = PlaybackData.RecordedTransforms[FrameIndex + 1];

            FTransform Interpolated;
            Interpolated.SetLocation(FMath::Lerp(Transform1.GetLocation(), Transform2.GetLocation(), Alpha));
            Interpolated.SetRotation(FQuat::Slerp(Transform1.GetRotation(), Transform2.GetRotation(), Alpha));
            Interpolated.SetScale3D(Transform1.GetScale3D());

            GetOwner()->SetActorTransform(Interpolated);
        }
    }
}
