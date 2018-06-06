#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "qrencode.h"

extern char qr_bmp_file[MAX_PATH];
int genernate_qrcode_ex(char *qrdata)
{
    unsigned int    unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
    unsigned char*  pRGBData, *pSourceData, *pDestData;
    QRcode*         pQRC;
    FILE*           f;

    if (pQRC = QRcode_encodeString(qrdata, 12, QR_ECLEVEL_H, QR_MODE_8, 1))
    {
        unWidth = pQRC->width;
        unWidthAdjusted = unWidth *  3  * 3;
        if (unWidthAdjusted % 4)
            unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
        unDataBytes = unWidthAdjusted * unWidth *  3;

        // Allocate pixels buffer

        if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
        {
            exit(-1);
        }

        // Preset to white

        memset(pRGBData, 0xff, unDataBytes);


        // Prepare bmp headers

        BITMAPFILEHEADER kFileHeader;
        kFileHeader.bfType = 0x4d42;  // "BM"
        kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) +
            sizeof(BITMAPINFOHEADER) +
            unDataBytes;
        kFileHeader.bfReserved1 = 0;
        kFileHeader.bfReserved2 = 0;
        kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) +
            sizeof(BITMAPINFOHEADER);

        BITMAPINFOHEADER kInfoHeader;
        kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
        kInfoHeader.biWidth = unWidth *  3;
        kInfoHeader.biHeight = -((int)unWidth *  3);
        kInfoHeader.biPlanes = 1;
        kInfoHeader.biBitCount = 24;
        kInfoHeader.biCompression = BI_RGB;
        kInfoHeader.biSizeImage = 0;
        kInfoHeader.biXPelsPerMeter = 0;
        kInfoHeader.biYPelsPerMeter = 0;
        kInfoHeader.biClrUsed = 0;
        kInfoHeader.biClrImportant = 0;


        // Convert QrCode bits to bmp pixels

        pSourceData = pQRC->data;
        for (y = 0; y < unWidth; y++)
        {
            pDestData = pRGBData + unWidthAdjusted * y *  3;
            for (x = 0; x < unWidth; x++)
            {
                if (*pSourceData & 1)
                {
                    for (l = 0; l <  3; l++)
                    {
                        for (n = 0; n <  3; n++)
                        {
                            *(pDestData + n * 3 + unWidthAdjusted * l) = 0xff;
                            *(pDestData + 1 + n * 3 + unWidthAdjusted * l) = 0;
                            *(pDestData + 2 + n * 3 + unWidthAdjusted * l) = 0;
                        }
                    }
                }
                pDestData += 3 *  3;
                pSourceData++;
            }
        }


        // Output the bmp file

        if (!(fopen_s(&f, qr_bmp_file, "wb")))
        {
            fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
            fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
            fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);

            fclose(f);
        }
        else
        {
            printf("Unable to open file");
            exit(-1);
        }

        // Free data

        free(pRGBData);
        QRcode_free(pQRC);
    }
    else
    {
        printf("NULL returned");
        exit(-1);
    }

	return 0;
}