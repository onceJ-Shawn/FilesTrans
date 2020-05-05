#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define CHUNK_SIZE (64*1024) 
#ifdef  _WIN32
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include<unistd.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#define SOCKET int
#endif
#include<stdlib.h>
#include <stdio.h>
#define BUFSIZE 262144 
/*int init_socket()
{
#ifdef _WIN32
	//1、请求版本
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (HIBYTE(wsaData.wVersion) != 2 || LOBYTE(wsaData.wVersion) != 2)
	{
		printf("请求版本失败！\n");
		return -1;
	}
	else
		printf("请求版成功！\n");
#endif
	return 0;
}
SOCKET socket_create(char *&ip,char *&cfilename,int port)
{
	if (init_socket() == -1)return 0;
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("创建socket失败！\n");
		return 0;
	}
	else printf("创建socket成功！\n");
	int num = 0;
	sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	sockAddr.sin_port = htons(port);
	int r = bind(serverSocket, (SOCKADDR*)&sockAddr, sizeof sockAddr);
	if (r == -1)
	{
		printf("绑定失败！\n");
		return 0;
	}
	else printf("绑定成功！\n");
	listen(serverSocket, 10);
	if (r == -1)
	{
		printf("监听失败！\n");
		return -1;
	}
	else printf("监听成功！\n");
	return serverSocket;
}
SOCKET socket_accept(SOCKET serverSocket)
{
#ifdef _WIN32
	int nSize = 0;
#else
	int nSize = 1;
#endif
	nSize = sizeof(SOCKADDR_IN);
	SOCKADDR_IN clientAddr;
	SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr,&nSize);//阻塞  相当于scanf
	if (clientSocket == SOCKET_ERROR)
	{
		printf("客户端出错！\n");
		return 0;
	}
	else {
		printf("客户端连接成功！\n");
		return clientSocket;
	}
}
*/
int tcpip(char* ip, char* cfilename, int port)
{
#ifdef _WIN32


	//1、请求版本
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (HIBYTE(wsaData.wVersion) != 2 || LOBYTE(wsaData.wVersion) != 2)
	{
		printf("请求版本失败！\n");
		return -1;
	}
	else printf("请求版成功！\n");
	//2、创建socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("创建socket失败！\n");
		return -1;
	}
	else printf("创建socket成功！\n");
	//3、创建协议地址族
	char inputip[20] = { 0 };
	//	findIP(ip, sizeof(ip));
	int num = 0;
	strcpy(inputip, ip);
	while (1)
	{
		if (strcmp(ip, inputip) == 0)
			break;
		else
			printf("IP地址与本机不符合，若不知道本机地址，请按1\n");
		int i = 0;
		scanf("%d", &i);
		if (i == 1)
			printf("%s", ip);
		printf("请输入本机IP地址");
	}

	sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	sockAddr.sin_port = htons(port);
	//4、绑定
	int r = bind(serverSocket, (SOCKADDR*)&sockAddr, sizeof sockAddr);
	if (r == -1)
	{
		printf("绑定失败！\n");
		return -1;
	}
	else printf("绑定成功！\n");
	//5、监听
	listen(serverSocket, 10);
	if (r == -1)
	{
		printf("监听失败！\n");
		return -1;
	}
	else printf("监听成功！\n");
	//6、连接
	SOCKADDR clientAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &nSize);//阻塞  相当于scanf
	if (clientSocket == SOCKET_ERROR)
	{
		printf("客户端出错！\n");
		return -2;
	}
	printf("客户端连接成功！\n");
	//7、传输
	char fileName[256] = { 0 };
	r = recv(clientSocket, fileName, 255, NULL);
	if (r > 0)
	{
		printf("接收到文件名：%s\n", fileName);
	}
	int s = strlen(fileName);
	int j = 0;
	for (int i = 0; i < s; i++)
	{
		if (fileName[i] == '//')
			j++;
	}//统计\的个数
	j /= 2;
	int nj = 0;
	char changer[256] = { 0 };
	changer[0] = '//';
	for (int i = 0; i < s; i++)
	{
		if (nj == j)
		{
			strcat(changer, fileName + i);
			break;
		}
		else if (fileName[i] == '//')
			nj++;
	}
	strcat(cfilename, changer);
	printf("%s\n", cfilename);
	int FileLen = 0;
	int nCurrentPos = 0; //断点位置              
	FILE *PosFile= fopen("PosFile.temp", "rb");
	if(PosFile)   
	{                
		fread((char*)&nCurrentPos,sizeof(nCurrentPos),1,PosFile);        //读取断点位置                
		printf("The File Pos is %d\n",nCurrentPos);                
		nCurrentPos=nCurrentPos+1;                                //从断点的下一块开始                
		fclose(PosFile);
		send(clientSocket,(char*)&nCurrentPos,sizeof(nCurrentPos),0);        //发送断点值                   
	}        
	else        
	{                
		send(clientSocket,(char*)&nCurrentPos,sizeof(nCurrentPos),0);        //无断点文件nCurrentPos为0                        
	}        
	if(recv(clientSocket,(char*)&FileLen,sizeof(FileLen),0)!=0)//接受文件长度        
	{                
		int nChunkCount;                
		FILE* file=fopen(cfilename,"rb");                
		nChunkCount=FileLen/CHUNK_SIZE;                //计算文件块数                
		if(FileLen%nChunkCount!=0)                
		{                        
			nChunkCount++;                
		}                
		if(file)                
		{                        
			fseek(file,nCurrentPos*CHUNK_SIZE,SEEK_SET);        //文件指针移至断点处                        
			char *date = new char[CHUNK_SIZE];                        
			for(int i=nCurrentPos;i<nChunkCount;i++)        //从断点处开始写入文件                        
			{                                
				printf("Recv The Chunk is %d\n",i);                                
				int nLeft;                                
				if(i+1==nChunkCount)//最后一块 
				nLeft=FileLen-CHUNK_SIZE*(nChunkCount-1);
				else                                        
					nLeft=CHUNK_SIZE;                                
				int idx=0;                                
				while(nLeft>0)                                
				{                                        
					int ret=recv(clientSocket,&date[idx],nLeft,0);                                        
					if(ret==SOCKET_ERROR)                                        
					{                                                
						printf("Recv Date Error");                                                
						return 0;                                        
					}                                        
					idx+=ret;                                        
					nLeft-=ret;                                
				}                                
				fwrite(date,CHUNK_SIZE,1,file);                                
				FILE *PosFile=fopen("PosFile.temp","wb");                //将断点写入PosFile.temp文件                                
				int seekpos=i+1;                                
				if(PosFile);   
				{                                        
					fwrite((char*)&seekpos,sizeof(seekpos),1,PosFile);                                        
					fclose(PosFile);                                
				}                        
			}                        
			fclose(file);                        
			delete[] date;                
		}                        
		if(DeleteFile("PosFile.temp")!=0)                
		{                        
			printf("文件传输完成");                
		}        
	}
	//int size = 0;
	//r = recv(clientSocket, (char*)&size, 4, NULL);
	//if (r > 0)
	//{
	//	printf("接收到文件大小：%d\n", size);
	//}
	//FILE *fp = fopen(cfilename, "wb");
	//int count = 0;
	//char buff[1024];
	//while (1)
	//{
	//	memset(buff, 0, 1024);
	//	r = recv(clientSocket, buff, 1024, NULL);
	//	if (r > 0)
	//	{
	//		count += r;
	//		fwrite(buff, 1, r, fp);
	//		if (count > size)
	//		{
	//			printf("文件接收完毕！\n");
	//			break;
	//		}
	//	}
	//}
	//fclose(fp);
	return 1;
#else
	struct sockaddr_in client_addr;
	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htons(ip);
	client_addr.sin_port = htons(port);

	// 创建socket，若成功，返回socket描述符 
	int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket_fd < 0)
	{
		perror("Create Socket Failed:");
		exit(1);
	}

	// 绑定客户端的socket和客户端的socket地址结构 非必需 
	if (-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr))))
	{
		perror("Client Bind Failed:");
		exit(1);
	}

	// 声明一个服务器端的socket地址结构，并用服务器那边的IP地址及端口对其进行初始化，用于后面的连接 
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, ip, &server_addr.sin_addr) == 0)
	{
		perror("Server IP Address Error:");
		exit(1);
	}
	server_addr.sin_port = htons(SERVER_PORT);
	socklen_t server_addr_length = sizeof(server_addr);

	// 向服务器发起连接，连接成功后client_socket_fd代表了客户端和服务器的一个socket连接 
	if (connect(client_socket_fd, (struct sockaddr*)&server_addr, server_addr_length) < 0)
	{
		perror("Can Not Connect To Server IP:");
		exit(0);
	}

	// 输入文件名 并放到缓冲区buffer中等待发送 
	char file_name[FILE_NAME_MAX_SIZE + 1];
	bzero(file_name, FILE_NAME_MAX_SIZE + 1);
	char buffer[BUFFER_SIZE];
	bzero(buffer, BUFFER_SIZE);
	strncpy(buffer, cfilename, strlen(cfilename) > BUFFER_SIZE ? BUFFER_SIZE : strlen(cfilename));

	// 向服务器发送buffer中的数据 
	if (send(client_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
	{
		perror("Send File Name Failed:");
		exit(1);
	}

	// 打开文件，准备写入 
	FILE *fp = fopen(cfilename, "w");
	if (NULL == fp)
	{
		printf("File:\t%s Can Not Open To Write\n", cfilename);
		exit(1);
	}

	// 从服务器接收数据到buffer中 
	// 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止 
	bzero(buffer, BUFFER_SIZE);
	int length = 0;
	while ((length = recv(client_socket_fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		if (fwrite(buffer, sizeof(char), length, fp) < length)
		{
			printf("File:\t%s Write Failed\n", cfilename);
			break;
		}
		bzero(buffer, BUFFER_SIZE);
	}

	// 接收成功后，关闭文件，关闭socket 
	printf("Receive File:\t%s From Server IP Successful!\n", cfilename);
	close(fp);
	close(client_socket_fd);
	return 1;
#endif
}
/*int recv_work(char *ip,char *cfilename,int port)
{
	SOCKET clientSocket = socket_create(ip,cfilename,port);
	if (clientSocket == 0)return 0;
	SOCKET serverSocket = socket_accept(clientSocket);
	if (serverSocket == 0)return 0;
	char fileName[30] = { 0 };
	int r = recv(clientSocket, fileName, 29, NULL);
	if (r > 0)
	{
		printf("接收到文件名：%s\n", fileName);
	}
	strcat(cfilename, fileName);
	int size = 0;
	r = recv(clientSocket, (char*)&size, 4, NULL);
	if (r > 0)
	{
		printf("接收到文件大小：%d\n", size);
	}
	FILE *fp = fopen(cfilename, "wb");
	int count = 0;
	char buff[1024];
	while (1)
	{
		memset(buff, 0, 1024);
		r = recv(clientSocket, buff, 1024, NULL);
		if (r > 0)
		{
			count += r;
			fwrite(buff, 1, r, fp);
			if (count >=size)
			{
				printf("文件接收完毕！\n");
				break;
			}
		}
	}
	fclose(fp);
	free(buff);

#ifdef  _WIN32
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
#else
	closesocket(clientSocket);
	closesocket(serverSocket);
#endif
	return 1;

}*/
int main(int arg, char *args[])
{
	//if (arg !=4)// 如果参数个数不为4，main函数退出
	//{
	//	printf("usage:server port\n");
	//	return EXIT_FAILURE;
	//}

	char ip[20] = { 0 };
	char cfilename[256] = { 0 };
	char argv[6] = { 0 };
	printf("请输入:如（D://file// 192.168.184.1 9999）\n");
	scanf("%s", cfilename);
	scanf("%s", ip);
	scanf("%s", argv);
	//strcpy(cfilename, args[1]);
	//strcpy(ip, args[2]);
	//strcpy(argv, args[3]);
	if (tcpip(ip, cfilename, atoi(argv)) == 1)
		printf("接收成功\n");
	else printf("接收失败\n");
	return 0;
}