// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HAL/Runnable.h"
#include "Networking.h"

class TCP_TEST_API FTcpReceiverThread : public FRunnable
{
public:
	float Tcp_UpdateRate;

public:
	FTcpReceiverThread(FSocket* client = NULL) : m_Client(client)
	{}
	~FTcpReceiverThread()
	{
		stopping = true;
	}

	virtual bool Init() override
	{
		stopping = false;
		return true;
	}

	virtual uint32 Run() override
	{
		if (!m_Client)
		{
			return 0;
		}

		// Init
		Tcp_UpdateRate = 0.1f;

		TArray<uint8> ReceiveData;
		uint32 size = 1024u;

		// 패킷 수신부
		while (!stopping)
		{
			// HasPendingData : 대기열에 보류 중인 데이터가 있는지 확인하기 위해 소켓을 쿼리합니다
			// 소켓에 데이터가 있으면 true, 그렇지 않으면 false
			while (m_Client->HasPendingData(size))
			{
				ReceiveData.Init(0, FMath::Min(size, 65507u));
				int32 read = 0;

				m_Client->Recv(ReceiveData.GetData(), ReceiveData.Num(), read);
				UE_LOG(LogTemp, Warning, TEXT("Data Bytes Read : %s Byte."), *FString::FromInt(read));

				FString ReceivedUE4String = "Data : " + FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceiveData.GetData()))); // byte to char
				UE_LOG(LogTemp, Warning, TEXT("%s"), *ReceivedUE4String);
			}

			FPlatformProcess::Sleep(Tcp_UpdateRate);
		}

		return 1;
	}

	virtual void Stop() override
	{
		stopping = true;
	}

protected:
	bool stopping;
	FThreadSafeCounter m_StopTaskCounter;
private:
	FSocket* m_Client;
};