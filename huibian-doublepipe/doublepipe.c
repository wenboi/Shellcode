#include <windows.h>
#include <stdio.h>
#include <winbase.h>
typedef void (*MYPROC)(LPTSTR);

int main()
{
	HINSTANCE LibHandle;
	MYPROC ProcAdd;

	LibHandle = LoadLibrary("kernel32");
	printf("kernel32 LibHandle = //0x%x\n",LibHandle);

	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"CreatePipe");
	printf("createpipe LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"CreateProcessA");
	printf("CreateProcessA LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"PeekNamedPipe");
	printf("PeekNamedPipe LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"WriteFile");
	printf("WriteFile LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"ReadFile");
	printf("read LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"ExitProcess");
	printf("Proc LibHandle = //0x%x\n",ProcAdd);

	LibHandle = LoadLibrary("ws2_32");
	printf("ws2_32 LibHandle = //0x%x\n",LibHandle);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"WSAStartup");
	printf("WSAStartup LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"socket");
	printf("socket LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"bind");
	printf("bind LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"listen");
	printf("listen LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"accept");
	printf("accept LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"send");
	printf("send LibHandle = //0x%x\n",ProcAdd);
	ProcAdd=(MYPROC)GetProcAddress(LibHandle,"recv");
	printf("recv LibHandle = //0x%x\n",ProcAdd);
	return 0;
}