#pragma once

#include <map>
#include <BaseTsd.h>



#define  RDB_HEAD_SIZE 16
#define  TGF_HEAD_SIZE 4
/// �ṹ��ı߽����Ϊ1���ֽڣ����������������ڴ����������洢�ģ��Է�������ļ�����
#pragma pack(1)
/// 
typedef struct
{
	BYTE  Head[RDB_HEAD_SIZE];		//�ļ�ͷ
	DWORD IndexNum;				//������Ŀ
	INT64 Offset;				//ƫ�Ƶ�ַ
	INT64 BaseAddr;				//��ַ
}FILEHEAD,*LPFILEHEAD;

typedef struct
{
	LPWSTR lwpName;		//�ļ�ͷ
	PBYTE SartAddr;	    //��ʼ��ַ=��ַ+ƫ�Ƶ�ַ 
	INT64  nSize;		//��Դ��С
}RESHEAD,*LPRESHEAD;

typedef struct
{
	CHAR  TGF[TGF_HEAD_SIZE];	//TGF�ļ�ͷ
	DWORD WZ1;	    
	DWORD WZ2;		
	BYTE  Info[4];  //None,Key����Ϣ
	DWORD Offset;   //ͼƬ�ļ�ƫ��
}TGFHEAD,*LPTGFHEAD;

#pragma pack()

const char RDB_HEADER[] = "531E98204F8542F0";
const char RDB_HEADER_ENCRYPT[] = "631E98204F8542F0";
const char TGF_HEADER[] = "TGF";

const BYTE PNG_HEADER[] = {0x89,0x50,0x4E,0x47};
const BYTE BMP_HEADER[] = {0x42,0x4D};
const BYTE GIF_HEADER[] = {0x47,0x49,0x46};
const BYTE JPG_HEADER[] = {0xFF,0xD8,0xFF,0xE0,0x00,0x10,0x4A,0x46,0x49,0x46};
const BYTE ICO_HEADER[] = {0x00,0x00,0x01,0x00,0x01,0x00}; //ͼ��
const BYTE CUR_HEADER[] = {0x00,0x00,0x02,0x00,0x01,0x00}; //���