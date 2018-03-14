//Task-4
//Server
#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wchar.h>
#include <Windows.h>
#include <map>

using namespace std;
map<string, SOCKET> SocketsList;

//const char IP[] = "192.168.0.104";
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
//DWORD WINAPI PerformConnection(LPVOID lpar)
//{
//	PARAMS* par = (PARAMS*)lpar;
//	SOCKET newsock = par->newsock;
//	sockaddr_in addr = par->addr;
//	printf("Connection to %S:%d is open \n", inet_ntoa(addr.sin_addr), htons(addr.sin_port));
//	//Обмен данными с клиентом
//
//}
/*DWORD WINAPI Reading(LPVOID pinfo)
{
	printf("Reading...\n");
	SOCKET sock = *(SOCKET*)pinfo;
	for(;;)
	{
		wchar_t Mess[MessSize];
		wmemset(Mess, 0, MessSize);
		if(recv(sock,(char*)Mess, MessSize*sizeof(wchar_t), 0) == -1)
			ExitThread(1);
		printf("Message: %s\n", Mess);
		if(wcscmp(Mess, L"всего") == 0)
			ExitThread(1);
	}
}
DWORD WINAPI Writing(LPVOID pinfo)
{
	printf("Writing...\n");
	SOCKET sock = *(SOCKET*)pinfo;
	for(;;)
	{
		wchar_t Mess[MessSize];
		wmemset(Mess, 0, MessSize);
		wprintf(L">");
		wscanf(L"%100ls", Mess);
		send(sock, (char*)Mess, wcslen(Mess)*sizeof(wchar_t), 0);
		if(wcscmp(Mess, L"всего") == 0)
			ExitThread(1);
	}
}*/
void showUsers(SOCKET &sock)
{
	bool flag = false;
	cout <<"Показываю список пользователей..."<<endl;
	//sock тот, который запрашивает, его не нужно выводить
	for(auto it = SocketsList.begin(); it != SocketsList.end(); ++it)
		if ( it->second != sock )
		{	
			flag = true;
			string s;
			s = it->first.c_str();
			s = "$Пользователь " + s +" сейчас online.";
			//cout<< s <<endl;
			send(sock, s.c_str() ,s.size(), 0);// it->first.c_str(), it->first.size(), 0);
		}
	if(!flag)
	{
		string s;
		s = "$Кроме вас в сети нет пользователей на данный момент.";
		send(sock, s.c_str() ,s.size(), 0);
	}
}
void showErrorMessage(){
	wchar_t ErrorMessage[1000];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                          WSAGetLastError(),
                          MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                          ErrorMessage, sizeof(ErrorMessage),
                          NULL);
     wprintf(L"Ошибка: %s\n", ErrorMessage);
}
void showConnectMes(string &Nickname)
{
	string s;
	s += "!Пользователь " + Nickname + " подключился к чату.";
	for (auto it = SocketsList.begin(); it != SocketsList.end(); ++it )
	if (it->first != Nickname )
		send(it->second, s.c_str(), s.size(), 0);
}
void showDisconnectMes(string &Nickname)
{
	string s;
	s += "!Пользователь " + Nickname + " вышел из чата.";
	for ( auto it = SocketsList.begin(); it != SocketsList.end(); ++it )
	if ( it->first != Nickname )
		send(it->second, s.c_str(), s.size(), 0);
}
DWORD WINAPI Reading_and_Writing(LPVOID lpar)
{
	cout<<"Ожидаю действий от пользователей..."<<endl;
	SOCKET &sock = ((Params*)lpar)->newsock;
	char nm[10];
	string Nickname;
	int rc = recv(sock, nm, 10, 0);
	Nickname.append(nm, rc);	 //Nickname пустой, добавляем туда имя начиная с нулевого символа(=rc)
	//cout<< Nickname <<endl;
	if ( rc > 0 )
	{
		cout << "Пользоваетель " << Nickname << " подключился к чату." << endl;
		showConnectMes(string(Nickname));
	}
	else 
		if ( rc == SOCKET_ERROR )
		{
			showErrorMessage();
			WSACleanup();
			closesocket(sock);
			return 0;
		}
	SocketsList.insert(make_pair(Nickname, sock));
	char* buffer = new char[64];
	string msg;
	while ((rc = recv(sock, buffer, 64, 0)) > 0 )
	{
		msg.append(buffer, rc);
		cout << Nickname << " : " << msg << endl;
		string to(msg, 0, msg.find(' '));
		string text(msg, msg.find(' ') + 1, msg.size());
		text = Nickname + " : " + text;
		/*if(to == "/quit")
			rc = 0;
		else*/
			if (to == "?")
				cout<<"help"<< endl;
			else
				if (to == "/users")
					showUsers(sock);
				else 
					if ( to == "/all" )
					{
						for ( auto it = SocketsList.begin(); it != SocketsList.end(); ++it )
						if ( it->second != sock )
							send(it->second, text.c_str(), text.size(), 0);
					}
					else 
						if (to[0] == '/')
							if ( SocketsList.count(to.substr(1, to.size())) == 1 )
								send(SocketsList[to.substr(1, to.size())], text.c_str(), text.size(), 0);
							else
								send(sock, "!Такого пользователя нет!", 25, 0);
			msg.erase();
	}
	if (rc == SOCKET_ERROR || rc == 0 )
	{
		cout << "Пользователь " << Nickname << " вышел из чата." << endl;
		showDisconnectMes(string(Nickname));
		SocketsList.erase(Nickname);				//удаляем пользователя из map
	}
	closesocket(sock);
	return 0;
}
int main (int argc, char * argv[])
{
	setlocale(LC_ALL,"Russian");//"Russian_Russia.866");

	WSADATA ws;
	WSAStartup(MAKEWORD(2, 0), &ws);
	//Обработка входных данных
	if (argc != 3)
	{
		wprintf(L" \n Ошибка: Неверное количество параметров! \n");
		return 1;
	}
	
	const char * IPAddr = argv[1];
	int NPort = atoi(argv[2]);

	//Создание сокета
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//Заполнение структуры
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(NPort);
	serv_addr.sin_addr.s_addr = inet_addr(IPAddr);     
	
	//Установка соединения
	if(bind(sock, (sockaddr*) &serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
	{
		wprintf(L" Ошибка в bind\n");	
		showErrorMessage();
		WSACleanup();
		return 1;
	}
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR )
	{
		showErrorMessage();
		WSACleanup();
		return 1;
	}
	SOCKET newsocket;
	/*for(;;)
	{
		SOCKET newsocket = accept(sock, NULL, NULL);
		if(newsocket != INVALID_SOCKET)
		{*/
	while (newsocket = accept(sock, NULL, NULL))
	{
			
			HANDLE hThread = CreateThread(NULL, 0, Reading_and_Writing, (LPVOID)(new Params(newsocket, serv_addr)), 0, NULL);
			if(hThread == NULL)
			{
				showErrorMessage();
				WSACleanup();
			}
			//ResumeThread(hThread);
			/*while ( int rc = recv(newsocket, buffer, 1024, 0) )
					input_file.write(buffer, rc);*/
			/*HANDLE handles[2];
			handles[0] = CreateThread(NULL,0, Reading, (LPVOID)&clientsock, 0, NULL);
			handles[1] = CreateThread(NULL,0, Writing, (LPVOID)&clientsock, 0, NULL);
			WaitForMultipleObjects(2, handles, FALSE, INFINITE);*/

			//CloseHandle(handles[0]);


			/*CloseHandle(hThread);
			shutdown(newsocket, 0);
			closesocket(newsocket);*/
		//}
		/*else
		{
			showErrorMessage();
			return 1;
		}*/
	}
	shutdown(sock, 0);
	closesocket(sock);
	WSACleanup();
	return 0;
}