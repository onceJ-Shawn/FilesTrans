#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define CHUNK_SIZE (64*1024) 
#include<stdio.h>
#include <stdlib.h>
#include<WinSock2.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")

int main() {

	//1、请求版本
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (HIBYTE(wsaData.wVersion) != 2 || LOBYTE(wsaData.wVersion) != 2)
	{
		printf("请求版本失败！\n");
		return 1;
	}
	else printf("请求版成功！\n");

	//2、创建socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("创建socket失败！\n");
		return -1;
	}
	else printf("创建socket成功！\n");

	//3、创建协议地址族
	char ip[20] = { 0 };
	char fileName[256] = { 0 };
	char port[6] = { 0 };//端口号
	int num = 0;
	printf("请输入:如（D://file//swift.bmp 192.168.184.1 9999）\n");
	scanf("%s", fileName);
	scanf("%s", ip);
	scanf("%s", port);
	num = atoi(port);
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = inet_addr(ip);//ip地址
	sockAddr.sin_port = htons(num);

	//4、连接
 	int r = connect(clientSocket, (SOCKADDR*)&sockAddr, sizeof sockAddr);
	if (r == SOCKET_ERROR)
	{
		printf("连接服务端出错！\n");
		return -2;
	}
	else printf("服务端连接成功！\n");

	//5、传输

	r = send(clientSocket, fileName, strlen(fileName), NULL);
	if (r > 0)
	{
		printf("文件名发送成功：%s\n", fileName);
	}
	FILE* fp = fopen(fileName, "rb");
	int size;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	r = send(clientSocket, (char*)&size, 4, NULL);
	if (r > 0)
	{
		printf("发送文件大小：%d\n", size);
	}
	char buff[1024];
	while (1)
	{              
		int nCurrentPos = 0;//接受断点值                        
		if(recv(clientSocket,(char*)&nCurrentPos,sizeof(nCurrentPos),0)==SOCKET_ERROR)                        
		{                                
			printf("The Clinet Socket is Closed\n");                                
			break;                        
		}
		else                        
		{                                
			printf("The Currentpos is The %d\n",nCurrentPos);                                      
			printf("Get File Proc is ok\n");
			FILE *file=fopen(fileName, "rb");
			int nChunkCount = 0;        //文件块数        
			if (file)
			{
				fseek(file, 0, SEEK_END);
				int FileLen = ftell(file);
				rewind(file);
				if (nCurrentPos != 0)
				{
					fseek(file,nCurrentPos*CHUNK_SIZE, SEEK_SET);        //文件指针移至断点处                        
						printf("file seek is %d\n",nCurrentPos * CHUNK_SIZE);
				}
				nChunkCount = FileLen / CHUNK_SIZE;                                //文件块数                
				if (FileLen%nChunkCount != 0)nChunkCount++;
				send(clientSocket, (char*)&FileLen, sizeof(FileLen), 0);                //发送文件长度                
				char *date = new char[CHUNK_SIZE];
				for (int i = nCurrentPos; i < nChunkCount; i++)                //从断点处分块发送                
				{
					printf("send the count %d\n",i );
					int nLeft;
					if (i + 1 == nChunkCount)                                //最后一块                                
						nLeft = FileLen - CHUNK_SIZE * (nChunkCount - 1);
					else
						nLeft = CHUNK_SIZE;
					int idx = 0;
					fread(date, CHUNK_SIZE,1,file);
					while (nLeft > 0)
					{
						int ret = send(clientSocket, &date[idx], nLeft, 0);
						if (ret == SOCKET_ERROR)
						{
							printf("Send The Date Error \n");
							break;
						}
						nLeft -= ret;
						idx += ret;
					}
				}
				fclose(file);
				delete[] date;
			}
			else
			{
				printf("open the file error\n");
			}
		}                
	}
	//关闭
	closesocket(clientSocket);
	WSACleanup();
	fclose(fp);
	system("pause");
	return 0;
}

