// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Networking.h"
#include "FTcpReceiverThread.h"
#include "Sockets.h"
#include "Packet.h"
#include "Misc/FileHelper.h"
#include <JsonObjectConverter.h>
#include "SocketSubsystem.h"
//#include "JsonObjectConverter.h"
#include "GameFramework/Actor.h"
#include "TCPClient.generated.h"

UCLASS()
class TCP_TEST_API ATCPClient : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATCPClient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game end
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Tcp Client")
	bool SocketCreate(FString IPStr, int32 port);

	UFUNCTION(BlueprintCallable, Category = "Tcp Client")
	bool SocketSend(FString message);

	UFUNCTION(BlueprintCallable, Category = "Tcp Client")
	virtual bool SocketReceive();

public:
	// Replicated : 서버는 리플리케이트된 프로퍼티의 값이 변할 때마다 각 클라이언트에 업데이트를 전송 , 클라에서 변수값 변경X
	// 이 업데이트는 서버에서만 받으며, 클라이언트는 프로퍼티 업데이트를 서버나 다른 클라이언트로 절대 전송하지 않음.
	/** Update Rate (sec)  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = ShipParam, Meta = (DisplayName = "UpdateRate"))
	float Tcp_UpdateRate;

public:
	bool ThreadEnd();
	FString StringFromBinaryArray(TArray<uint8> BinaryArray);
	FSocket* Host;
	FIPv4Address ip;
	FRunnableThread* m_RecvThread;
	FTcpReceiverThread* m_TcpRecvThread;

};
