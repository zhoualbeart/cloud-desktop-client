#include <stdio.h>
#include <fstream>
#include "qr.h"


#if 0

using namespace std;
int main()
{
	int errcode = QR_ERR_NONE;
	//qrInit的5个参数分别是version,mode,纠错等级和掩码，使用过程中使用除了QR_EM_8BIT以外的mode会生成不正常的二维码，暂时不知道原因。
	QRCode* p = qrInit(10, QR_EM_8BIT, 2, -1, &errcode);
	if (p == NULL)
	{
		printf("error\n");
		return -1;
	}

	string strData = "http://www.baidu.com";
	qrAddData(p, (const qr_byte_t* )strData.data(), strData.length());
	//注意需要调用qrFinalize函数
	if (!qrFinalize(p))
	{
		printf("finalize error\n");
		return -1;
	}
	int size = 0;
	//两个5分别表示：像素之间的距离和二维码图片的放大倍数，范围都是1-16
	qr_byte_t * buffer = qrSymbolToBMP(p, 0, 5, &size);
	if (buffer == NULL)
	{
		printf("error %s", qrGetErrorInfo(p));
		return -1;
	}
	ofstream f("a.bmp");
	f.write((const char *)buffer, size);
	f.close();
	return 0;
}

#endif



int main()
{
	int errcode = QR_ERR_NONE;
	FILE *fp;
	//qrInit的5个参数分别是version,mode,纠错等级和掩码，使用过程中使用除了QR_EM_8BIT以外的mode会生成不正常的二维码，暂时不知道原因。
	QRCode* p = qrInit(10, QR_EM_8BIT, 2, -1, &errcode);
	if (p == NULL)
	{
		printf("error\n");
		return -1;
	}

	char *strData = "D:\\lf\\cloud-desktop-client\\libqrtest\\小智.txt";
	qrAddData(p, (const qr_byte_t* )strData, strlen(strData));
	//注意需要调用qrFinalize函数
	if (!qrFinalize(p))
	{
		printf("finalize error\n");
		return -1;
	}
	int size = 0;
	//两个5分别表示：像素之间的距离和二维码图片的放大倍数，范围都是1-16
	qr_byte_t * buffer = qrSymbolToBMP(p, 0, 5, &size);
	if (buffer == NULL)
	{
		printf("error %s", qrGetErrorInfo(p));
		return -1;
	}
	if (NULL == (fp = fopen("a.bmp", "w+")))
	{
		printf("open %s file error\n", strData);
		return -1;
	}
	fwrite((const char *)buffer, size, 1, fp);
	fclose(fp);

	return 0;
}
