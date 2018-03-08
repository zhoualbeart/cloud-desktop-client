#include <stdio.h>
#include <fstream>
#include "qr.h"

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

        string strData = "http://www.baidu.com����";
        qrAddData(p, (const qr_byte_t* )strData.data(), strData.length());
        //ע����Ҫ����qrFinalize����
        if (!qrFinalize(p))
        {
                printf("finalize error\n");
                return -1;
        }
        int size = 0;
        //����5�ֱ��ʾ������֮��ľ���Ͷ�ά��ͼƬ�ķŴ�������Χ����1-16
		qr_byte_t * buffer = qrSymbolToBMP(p, 5, 5, &size);
        if (buffer == NULL)
        {
                printf("error %s", qrGetErrorInfo(p));
                return -1;
        }
        ofstream f("a.png");
        f.write((const char *)buffer, size);
        f.close();
        return 0;
}