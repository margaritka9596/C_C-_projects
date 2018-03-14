//Task-4, multi-threaded chat
//Client
//need arguments: IP , number of port 
#include <winsock2.h>
#include <wchar.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

using namespace std;

//const char IPAddr[] = "192.168.0.104";
//const long NPort = 12007;
//const int MessSize = 1000;

struct Params
{
	SOCKET newsock;
	sockaddr_in addr;
	Params(SOCKET newsock, sockaddr_in addr)
	{
		this->newsock = newsock;
		this->addr = addr;
	}
};
void showErrorMessage(){
	wchar_t ErrorMessage[1000];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                          WSAGetLastError(),
                          MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                          ErrorMessage, sizeof(ErrorMessage),
                          NULL);
            wprintf(L"Ошибка: %s\n", ErrorMessage);
}
void showHelp(){
	cout << endl;
	wprintf(L"Команды чата: \n");
	wprintf(L">ваше сообщение_________________________отправка сообщения всем пользователям\n");
	wprintf(L">/*Никнейм получателя ваше сообщение____отправка личного сообщения пользователю\n");
	wprintf(L">/users_________________________________показать всех пользователей\n\n");
	//wprintf(L">/quit__________________________________выход\n\n");
}
DWORD WINAPI Reading(LPVOID pinfo)
{
	//printf("Reading...\n");
	SOCKET sock = *(SOCKET*)pinfo;
	/*int rc;
	char* buffer = new char[64];
	string msg;
	while ( (rc = recv(sock, buffer, 64, 0)) > 0 )
	{
		msg.append(buffer, rc);
		cout << msg << endl;
		msg.erase();
	}*/
	for(;;)
	{
		wchar_t Mess[64];
		wmemset(Mess, 0, 64);
		string s;
		int rc = recv(sock, (char*)Mess, 512/**sizeof(wchar_t)*/, 0);
		if( rc == -1)
			ExitThread(1);
		s.append((char*)Mess, rc);
		if(s.front() == '!')
		{
			s.erase(0, 1);
			//wprintf(L"%s\n>", s);
			cout << s << endl;
			cout<< ">";
		}
		else
			if(s.front() == '$')
			{
				s.erase(0, 1);
				//wprintf(L"%s\n>", s);
				cout << s << endl;
				cout<< ">";
			}
			else
			{
				wprintf(L"Сообщение от  "); 
				printf("%s\n>", Mess);
			}
		/*if(wcscmp(Mess, L"/quit") == 0)
			ExitThread(1);*/
	}
}
DWORD WINAPI Writing(LPVOID pinfo)
{
	//printf("Writing...\n");
	SOCKET sock = *(SOCKET*)pinfo;
	for(;;)
	{
		//wchar_t Mess[64];
		//wmemset(Mess, 0, 64);
		//wscanf_s(L"%100ls", Mess);
		string Mess;
		cout <<">";
		getline(cin, Mess);
		if (Mess.front() == '?')
			showHelp();
		else
			if ( Mess.front() != '/' )
				Mess = "/all " + Mess;
			else 
				if ( Mess.find("/users") )
					Mess += ' ';						//чтоб отделить команду

				//else
				//	if (Mess.find("/quit"))
				//	{
				//		//exitinput = 1;
				//		//(*(threadParams*)input).exitflag = 1;
				//		string s = "/quit";
				//		send(sock , s.c_str() , s.size(), 0);
				//		ExitThread(1);
				//	} 
		send(sock, Mess.c_str(), Mess.size(), 0);
	}
	return 0;
}
int main (int argc, char * argv[])
{
	WSADATA ws;
	WSAStartup(MAKEWORD(2, 0), &ws);

	setlocale( LC_ALL, "");//"Russian_Russia.866" );

	//Обработка входных данных
	if (argc != 3)
	{
		wprintf(L" \n Ошибка: Неверное количество параметров! \n");
		WSACleanup();
		return 1;
	}
	const char * IPAddr = argv[1];
	int NPort = atoi(argv[2]);
	
	//Создание сокета
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	//Заполнение структуры
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(NPort);
	addr.sin_addr.s_addr = inet_addr(IPAddr);
			
	//Установка соединения
	if(connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		showErrorMessage();
		return 1;
	}

	//Запрос имени пользователя
	string Nickname;
	wprintf(L"			Добро пожаловать!\nВведите свой никнейм: ");
	getline(cin, Nickname);
	
	send(sock, Nickname.c_str(), Nickname.size(), 0);
	wprintf(L"\n                 Для получения справки введите '?'     \n");
	HANDLE handles[2];

	handles[0] = CreateThread(NULL, 0, Reading, (LPVOID)(new Params(sock,addr)), 0, NULL);
	handles[1] = CreateThread(NULL, 0, Writing, (LPVOID)(new Params(sock,addr)), 0, NULL);
	WaitForMultipleObjects(2, handles, TRUE, INFINITE);
	
	CloseHandle(handles[0]);
	CloseHandle(handles[1]);
	
	shutdown(sock, 2);
	closesocket(sock);
	system("pause");
	WSACleanup();
	return 0;
}
