#include "stdafx.h"
#include "qrcode.h"
#include <string.h>
#include "qr.h"

int genernate_qrcode(const char *qrdata, const char *bmp_filepath)
{
	int			errcode;
	QRCode		*p;
	int			size;
	FILE		*fp;
	qr_byte_t	*buffer;

	errcode = QR_ERR_NONE;
    /* qrInit��5�������ֱ���version,mode,����ȼ�������ʹ����� */
	p = qrInit(13, QR_EM_8BIT, QR_ECL_L, -1, &errcode);

	if (NULL == qrdata)
	{
		printf("file path cannot can not be null.\n");
		return -1;
	}

    if (p == NULL)
    {
        printf("error\n");
        return -1;
    }

    qrAddData(p, (const qr_byte_t* )qrdata, strlen(qrdata));
    /* ע����Ҫ����qrFinalize���� */
    if (!qrFinalize(p))
    {
        printf("finalize error\n");
        return -1;
    }
	
	size = 0;
    /* ����5�ֱ��ʾ������֮��ľ���Ͷ�ά��ͼƬ�ķŴ�������Χ����1-16 */
	buffer = qrSymbolToBMP(p, 0, 3, &size);
    if (buffer == NULL)
    {
        printf("error %s", qrGetErrorInfo(p));
        return -1;
    }

    if (NULL == (fp = fopen(bmp_filepath,"wb+")))
	{
		printf("open bmp file error\n");
		return -1;
	}

	if ( 1 != fwrite((const char *)buffer, size, 1, fp))
	{
		printf("write file error\n");
		fclose(fp);
		return -1;
	}

    fclose(fp);
    return 0;
}