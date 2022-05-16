// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPClient.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME 사용위함

ATCPClient::ATCPClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	Tcp_UpdateRate = 0.1f;
}

// AActor에 이미 있음
// 네트워크 Replication에 사용되는 속성을 반환. Replicate 속성이 있는 모든 액터 클래스에서 이 속성을 재정의해야함
void ATCPClient::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// TCP_UpdateRate 멤버 변수는 ATCPClient 액터 유형 모든 사본에 대해 접속된 모든 클라이언트와 동기화된다.
	DOREPLIFETIME(ATCPClient, Tcp_UpdateRate);
}

// Called when the game starts or when spawned
void ATCPClient::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("ATCPClient::BeginPlay()"));	

	//SocketCreate("121.161.7.67", 1998);
	SocketCreate("192.168.219.102", 1998); // HYH

	SocketSend("HYH send Test");

}

void ATCPClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ThreadEnd();

	if (Host)
	{
		Host->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Host);
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ATCPClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_TcpRecvThread)
	{
		m_TcpRecvThread->Tcp_UpdateRate = Tcp_UpdateRate;
	}

	//if (SocketReceive())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Receive Succed!"));
	//}
}


bool ATCPClient::SocketCreate(FString IPstr, int32 port)
{
	// 들어오는 IPStr을 IPv4 주소로 전환
	FIPv4Address::Parse(IPstr, ip);
	
	// ip 주소와 포트를 생성해 addr에 저장 
	TSharedPtr<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	//bool bIsValid;
	//addr->SetIp(*IPstr, bIsValid);
	addr->SetIp(ip.Value);	
	addr->SetPort(port);

	// 클라이언트 socket 만들기
	Host = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	UE_LOG(LogTemp, Warning, TEXT("Trying to connect"));

	// 연결 성공시
	if (Host->Connect(*addr))
	{
		UE_LOG(LogTemp, Warning, TEXT("TCP Connect Succeed!"));
		return true;
	}

	// 연결 실패시
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TCP Connect Failed!"));
		return false;
	}

}

bool ATCPClient::SocketSend(FString message)
{
	// 패킷 구조체
	FStructTest packet;
	packet.Name = "HYH_TEST";
	packet.Data = 1234;
	
	// 구조체를 JSON 스트링으로
	FString JsonString;
	if (FJsonObjectConverter::UStructToJsonObjectString(packet, JsonString, 0, 0, 0, nullptr, false))
	{
		UE_LOG(LogTemp, Warning, TEXT("JSON : %s"), *JsonString);
	}	

	// TCHAR를 보냄
	//TCHAR* seriallizedChar = message.GetCharArray().GetData();
	//int32 size = FCString::Strlen(seriallizedChar) + 1;
	//int32 sent = 0;

	//if (Host->Send((uint8*)TCHAR_TO_UTF8(seriallizedChar), size, sent))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Send Succeed!"));
	//	return true;
	//}

	// JSON 문자열 보냄
	TCHAR* seriallizedChar = JsonString.GetCharArray().GetData();
	int32 size = FCString::Strlen(seriallizedChar) + 1;
	int32 sent = 0;

	if (Host->Send((uint8*)TCHAR_TO_UTF8(seriallizedChar), size, sent))
	{
		UE_LOG(LogTemp, Warning, TEXT("Send Succeed!"));
		return true;
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Send Failed!"));
		return false;
	}
}

bool ATCPClient::SocketReceive()
{
	if (Host->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		FThreadSafeCounter WorkerCounter;

		FString ThreadName(FString::Printf(TEXT("RecvThread%i"), WorkerCounter.Increment()));

		m_TcpRecvThread = new FTcpReceiverThread(Host);
		m_RecvThread = FRunnableThread::Create(m_TcpRecvThread, *ThreadName);

		return true;
	}

	return false;
}

bool ATCPClient::ThreadEnd()
{
	if (m_RecvThread != nullptr)
	{
		m_RecvThread->Kill(true);
		delete m_RecvThread;
	}

	return true;
}

FString ATCPClient::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}