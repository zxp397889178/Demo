#pragma once

#define GETBYTELEN   2
#define IMAGE_YES    1
#define IMAGE_NO     0

/*---JPEG/JPG(1)---*/
#define IMAGE_JPEG_TYPE  0x01
#define IMAGE_JPEG_JPG "JPEG/JPG"
#define JPEG_FIRSTBIT  0xFF
#define JPEG_SECONDBIT 0xD8
#define JPEG_PENULTIMATEBIT  0xFF		//倒数第二
#define JPEG_LASTBIT  0xD9		//倒数第一

/*---BMP(2)---*/
#define IMAGE_BMP_TYPE  0x02
#define IMAGE_BMP     "BMP"
#define BMP_FIRSTBIT  0x42
#define BMP_SECONDBIT 0x4D

/*---PNG(3)---*/
#define IMAGE_PNG_TYPE  0x04
#define IMAGE_PNG     "PNG"
#define PNG_FIRSTBIT  0x89
#define PNG_SECONDBIT 0x50
#define PNG_THIRDBIT  0x4E
#define PNG_FORTHBIT  0x47

/*---GIF(4)---*/
#define IMAGE_GIF_TYPE 0x08
#define IMAGE_GIF     "GIF"
#define GIF_FIRSTBIT  0x47
#define GIF_SECONDBIT 0x49
#define GIF_THIRDBIT  0x46
#define GIF_FORTHBIT  0x38
#define GIF_FIFTHBIT1 0x39   // 可能存在是37
#define GIF_FIFTHBIT2 0x37
#define GIF_SIXTHBIT  0x61

/*---TIFF(5)---*/
#define IMAGE_TIFF_TYPE  0x10
#define IMAGE_TIFF      "TIFF"
#define TIFF_FIRSTBIT   0x4D
#define TIFF_SECONDBIT  0x4D
#define TIFF_FIRSTBIT2  0x49  // 或者
#define TIFF_SECONDBIT2 0x49

/*---ICO(6)---*/
#define IMAGE_ICO    "ICO"
#define ICO_FIRSTBIT   0x0
#define ICO_SECONDBIT  0x0
#define ICO_THIRDBIT   0x1  // 01
#define ICO_FIFTHBIT   0x1  // 01
/*---TGA(7)---*/
#define IMAGE_TGA    "TGA"
#define TGA_THIRDBIT 0x2  // 02
#define TGA_FIFTHBIT 0x0  // 00
/*---CUR(8)---*/
#define IMAGE_CUR    "CUR"
#define CUR_THIRDBIT 0x2  // 02
#define CUR_FIFTHBIT 0x1  // 01
/*---PCX(9)---*/
#define IMAGE_PCX    "PCX"
#define PCX_FIRSTBIT 0xA  // 0A
/*---IFF(10)---*/
#define IMAGE_IFF     "IFF"
#define IFF_FIRSTBIT  0x46
#define IFF_SECONDBIT 0x4F
#define IFF_THIRDBIT  0x52
#define IFF_FORTHBIT  0x4D

/*---ANI(11)---*/
#define IMAGE_ANI     "ANI"
#define ANI_FIRSTBIT  0x52
#define ANI_SECONDBIT 0x49
#define ANI_THIRDBIT  0x46
#define ANI_FORTHBIT  0x46
/* 根据图片文件头部数据判断*/
int CheckImageHeader(unsigned char* szFileData)
{
	if (NULL == szFileData){
		return IMAGE_NO;
	}

	//JPEG
	if (szFileData[0] == JPEG_FIRSTBIT && szFileData[1] == JPEG_SECONDBIT) //&& szFileData[6] == JPEG_PENULTIMATEBIT && szFileData[7] == JPEG_LASTBIT)
	{
		return IMAGE_YES;
	}

	//BMP
	if (szFileData[0] == BMP_FIRSTBIT && szFileData[1] == BMP_SECONDBIT)
	{
		return IMAGE_YES;
	}

	//PNG
	if (szFileData[0] == PNG_FIRSTBIT && szFileData[1] == PNG_SECONDBIT &&
		szFileData[2] == PNG_THIRDBIT && szFileData[3] == PNG_FORTHBIT)
	{
		return IMAGE_YES;
	}

	//GIF
	if (szFileData[0] == GIF_FIRSTBIT && szFileData[1] == GIF_SECONDBIT &&
		szFileData[2] == GIF_THIRDBIT && szFileData[3] == GIF_FORTHBIT &&
		(szFileData[4] == GIF_FIFTHBIT1 || szFileData[4] == GIF_FIFTHBIT2) &&
		szFileData[5] == GIF_SIXTHBIT )
	{
		return IMAGE_YES;
	}
	
	//TIFF
	if ((szFileData[0] == TIFF_FIRSTBIT && szFileData[1] == TIFF_SECONDBIT) &&
		(szFileData[0] == TIFF_FIRSTBIT2 && szFileData[1] == TIFF_SECONDBIT2))
	{
		return IMAGE_YES;
	}

	//ICO
	if (szFileData[2] == ICO_THIRDBIT && szFileData[4] == ICO_FIFTHBIT)
	{
		return IMAGE_YES;
	}

	//TGA
	if (szFileData[2] == TGA_THIRDBIT && szFileData[4] == TGA_FIFTHBIT)
	{
		return IMAGE_YES;
	}

	//CUR
	if (szFileData[2] == CUR_THIRDBIT && szFileData[4] == CUR_FIFTHBIT)
	{
		return IMAGE_YES;
	}

	//PCX
	if (szFileData[0] == PCX_FIRSTBIT )
	{
		return IMAGE_YES;
	}

	//IFF
	if (szFileData[0] == IFF_FIRSTBIT && szFileData[1] == IFF_SECONDBIT &&
		szFileData[2] == IFF_THIRDBIT && szFileData[3] == IFF_FORTHBIT)
	{
		return IMAGE_YES;
	}

	//ANI
	if (szFileData[0] == ANI_FIRSTBIT && szFileData[1] == ANI_SECONDBIT &&
		szFileData[2] == ANI_THIRDBIT && szFileData[3] == ANI_FORTHBIT)
	{
		return IMAGE_YES;
	}

	return IMAGE_NO;
}

bool IsImage(LPCTSTR lpStrPath)
{
	unsigned char szFileData[8] = { 0 };		//头6字节 尾2字节
	FILE *fd = NULL;

	int nHeadLen = 6;
	int nEndLen = 2;

	if ((fd = _tfopen(lpStrPath, _T("rb"))) == NULL)
	{
		return false;
	}

	//去掉JPG格式的结尾符判断  -- 就不需要这部分逻辑
	//fseek(fd, 0, SEEK_END);

	//if (ftell(fd) < nHeadLen + nEndLen)
	//{
	//	fclose(fd);
	//	return false;
	//}

	//fseek(fd, 0, SEEK_SET);

	if (fread(szFileData, sizeof(unsigned char), nHeadLen, fd) != nHeadLen)
	{
		fclose(fd);
		return false;
	}

	//去掉JPG格式的结尾符判断
	//fseek(fd, -nEndLen, SEEK_END);

	//if (fread(szFileData + nHeadLen, sizeof(unsigned char), nEndLen, fd) != nEndLen)
	//{
	//	fclose(fd);
	//	return false;
	//}

	fclose(fd);

	return (IMAGE_YES == CheckImageHeader(szFileData));
}