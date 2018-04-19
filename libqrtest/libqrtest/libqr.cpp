#include <stdio.h>
#include <fstream>
#include "qr.h"


#if 0

using namespace std;
int main()
{
	int errcode = QR_ERR_NONE;
	//qrInit��5�������ֱ���version,mode,����ȼ������룬ʹ�ù�����ʹ�ó���QR_EM_8BIT�����mode�����ɲ������Ķ�ά�룬��ʱ��֪��ԭ��
	QRCode* p = qrInit(10, QR_EM_8BIT, 2, -1, &errcode);
	if (p == NULL)
	{
		printf("error\n");
		return -1;
	}

	string strData = "http://www.baidu.com";
	qrAddData(p, (const qr_byte_t* )strData.data(), strData.length());
	//ע����Ҫ����qrFinalize����
	if (!qrFinalize(p))
	{
		printf("finalize error\n");
		return -1;
	}
	int size = 0;
	//����5�ֱ��ʾ������֮��ľ���Ͷ�ά��ͼƬ�ķŴ�������Χ����1-16
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
	//qrInit��5�������ֱ���version,mode,����ȼ������룬ʹ�ù�����ʹ�ó���QR_EM_8BIT�����mode�����ɲ������Ķ�ά�룬��ʱ��֪��ԭ��
	QRCode* p = qrInit(10, QR_EM_8BIT, 2, -1, &errcode);
	if (p == NULL)
	{
		printf("error\n");
		return -1;
	}

	char *strData = "D:\\lf\\cloud-desktop-client\\libqrtest\\С��.txt";
	qrAddData(p, (const qr_byte_t* )strData, strlen(strData));
	//ע����Ҫ����qrFinalize����
	if (!qrFinalize(p))
	{
		printf("finalize error\n");
		return -1;
	}
	int size = 0;
	//����5�ֱ��ʾ������֮��ľ���Ͷ�ά��ͼƬ�ķŴ�������Χ����1-16
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
