#pragma once

#include <map>
#include <BaseTsd.h>



#define  RDB_HEAD_SIZE 16
#define  TGF_HEAD_SIZE 4
/// 结构体的边界对齐为1个字节，令其所有数据在内存中是连续存储的，以方便进行文件解析
#pragma pack(1)
/// 
typedef struct
{
	BYTE  Head[RDB_HEAD_SIZE];		//文件头
	DWORD IndexNum;				//索引数目
	INT64 Offset;				//偏移地址
	INT64 BaseAddr;				//基址
}FILEHEAD,*LPFILEHEAD;

typedef struct
{
	LPWSTR lwpName;		//文件头
	PBYTE SartAddr;	    //开始地址=基址+偏移地址 
	INT64  nSize;		//资源大小
}RESHEAD,*LPRESHEAD;

typedef struct
{
	CHAR  TGF[TGF_HEAD_SIZE];	//TGF文件头
	DWORD WZ1;	    
	DWORD WZ2;		
	BYTE  Info[4];  //None,Key等信息
	DWORD Offset;   //图片文件偏移
}TGFHEAD,*LPTGFHEAD;

#pragma pack()

const char RDB_HEADER[] = "531E98204F8542F0";
const char RDB_HEADER_ENCRYPT[] = "631E98204F8542F0";
const char TGF_HEADER[] = "TGF";

const BYTE PNG_HEADER[] = {0x89,0x50,0x4E,0x47};
const BYTE BMP_HEADER[] = {0x42,0x4D};
const BYTE GIF_HEADER[] = {0x47,0x49,0x46};
const BYTE JPG_HEADER[] = {0xFF,0xD8,0xFF,0xE0,0x00,0x10,0x4A,0x46,0x49,0x46};
const BYTE ICO_HEADER[] = {0x00,0x00,0x01,0x00,0x01,0x00}; //图标
const BYTE CUR_HEADER[] = {0x00,0x00,0x02,0x00,0x01,0x00}; //光标