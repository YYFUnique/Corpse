/*
 * QR Code Generator Library: Symbol Converters for Windows Bitmap
 *
 * Core routines were originally written by Junn Ohta.
 * Based on qr.c Version 0.1: 2004/4/3 (Public Domain)
 *
 * @package     libqr
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2006-2013 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "qrcnv.h"
#if defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__)
#include <stdint.h>
#endif
/* {{{ size constants */

#define QRCNV_BMP_BISIZE 40  /* Windows Bitmap */
#define QRCNV_BMP_OFFBITS 62 /* 14(bf) + 40(bi) + 4(rgbq) + 4(rgbq) */
#define QRCNV_BMP_PPM 3780   /* 96 dpi ~= 3780 ppm */
                             /* 72 dpi ~= 2835 ppm */

/* }}} */
/* {{{ utility macro */

#if defined(__BIG_ENDIAN__)
#define qrBmpWriteShort(ptr, n) { \
	uint16_t us = (uint16_t)((n) & 0xffffU); \
	qr_byte_t *tmp = (qr_byte_t *)&us; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[0]; \
}
#elif defined(__LITTLE_ENDIAN__)
#define qrBmpWriteShort(ptr, n) { \
	uint16_t us = (uint16_t)((n) & 0xffffU); \
	qr_byte_t *tmp = (qr_byte_t *)&us; \
	*(ptr)++ = tmp[0]; \
	*(ptr)++ = tmp[1]; \
}
#else
#define qrBmpWriteShort(ptr, n) { \
	*(ptr)++ = (n) & 0xffU; \
	*(ptr)++ = ((n) >> 8) & 0xffU; \
}
#endif

#if defined(__BIG_ENDIAN__)
#define qrBmpWriteLong(ptr, n) { \
	uint32_t ul = (uint32_t)(n); \
	qr_byte_t *tmp = (qr_byte_t *)&ul; \
	*(ptr)++ = tmp[3]; \
	*(ptr)++ = tmp[2]; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[0]; \
}
#elif defined(__LITTLE_ENDIAN__)
#define qrBmpWriteLong(ptr, n) { \
	uint32_t ul = (uint32_t)(n); \
	qr_byte_t *tmp = (qr_byte_t *)&ul; \
	*(ptr)++ = tmp[0]; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[2]; \
	*(ptr)++ = tmp[3]; \
}
#else
#define qrBmpWriteLong(ptr, n) { \
	*(ptr)++ = (n) & 0xffU; \
	*(ptr)++ = ((n) >> 8) & 0xffU; \
	*(ptr)++ = ((n) >> 16) & 0xffU; \
	*(ptr)++ = ((n) >> 24) & 0xffU; \
}
#endif

#define qrBmpNextPixel() { \
	if (pxshift == 0) { \
		rptr++; \
		pxshift = 7; \
	} else { \
		pxshift--; \
	} \
}

#define QRCNV_NEW(rsize, ssize) { \
	rbuf = (qr_byte_t *)new BYTE[rsize]; \
	if (rbuf == NULL) { \
	QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION); \
	} \
	sbuf = (qr_byte_t *)new BYTE[ssize]; \
	if (sbuf == NULL) { \
	free(rbuf); \
	QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION); \
	} \
}

/* }}} */
/* {{{ function prototypes */

static qr_byte_t *
qrBmpWriteHeader(qr_byte_t *bof, int size, int width, int height, int imagesize);

/* }}} */
/* {{{ qrSymbolToBMP() */

/*
* 条形码生成的代码的符号,单色2值
* Windows Bitmap(BMP)转换成
 */
QR_API qr_byte_t *
qrSymbolToBMP2(QRCode *qr, int sep, int mag, int *size)
{
	qr_byte_t *rbuf, *rptr;
	qr_byte_t *sbuf, *sptr;
	int rsize, imgsize;
	int sepskips;
	int i, j, ix, dim, imgdim, sepdim;

	QRCNV_CHECK_STATE();
	QRCNV_GET_SIZE();

	//bmp位图在存储时，图片数据部分的字节数一定是4的倍数，如果不够四的倍数，在每一行要补齐字节。 
	rsize = (imgdim*3/4+1)*4;
	//bmp文件大小，包括文件头和数据部分
	imgsize = rsize * imgdim *3;
	*size = QRCNV_BMP_OFFBITS + imgsize;

	QRCNV_MALLOC(rsize, *size);

	sptr = qrBmpWriteHeader(sbuf, *size, imgdim, imgdim, imgsize);

	sepskips = rsize * sepdim;
	/* 在bmp图片下部绘制白边框 */
	if (sepskips) {
		memset(sptr, 0xFF, (size_t)sepskips);
		sptr += sepskips;
	}	

	for (i=0;i<dim;++i)
	{
		memset(rbuf, 0, (size_t)rsize);
		rptr = rbuf;

		/* 在bmp左边绘制白边框 */
		if (sepdim)
		{
			memset(rptr, 0xFF, 3*sepdim);
			rptr += 3*sepdim;
		}

		for (j=0;j<dim;++j)
		{
			int ix = qrIsBlack(qr,i,j) ? 0: 0xFF;
			memset(rptr, ix, 3*mag);
			rptr += 3*mag;
		}

		/* 在bmp右侧绘制白边框 */
		if (sepdim)
		{
			memset(rptr, 0xFF, 3*sepdim);
			rptr += 3*sepdim;
		}

		/* 存在放大模式情况多次拷贝 */	
		for (ix = 0; ix < mag; ix++) {
			memcpy(sptr, rbuf, (size_t)rsize);
			sptr += rsize;
		}
	}

	/* 在bmp上部绘制白边框 */

	if (sepskips) {
		memset(sptr, 0xFF, (size_t)sepskips);
		sptr += sepskips;
	}

	delete[] rbuf;
	return sbuf;
}

/* }}} */
/* {{{ qrBmpWriteHeader() */

static qr_byte_t *
qrBmpWriteHeader(qr_byte_t *bof, int size, int width, int height, int imagesize)
{
	qr_byte_t *ptr;

	ptr = bof;

	/*
	 * BITMAPFILEHEADER
	 */
	*ptr++ = 'B'; /* bfType */
	*ptr++ = 'M'; /* bfType */
	qrBmpWriteLong(ptr, size); /* bfSize */
	qrBmpWriteShort(ptr, 0); /* bfReserved1 */
	qrBmpWriteShort(ptr, 0); /* bfReserved2 */
	qrBmpWriteLong(ptr, QRCNV_BMP_OFFBITS); /* bfOffBits */

	/*
	 * BMPINFOHEADER
	 */
	qrBmpWriteLong(ptr, QRCNV_BMP_BISIZE); /* biSize */
	qrBmpWriteLong(ptr, width);  /* biWidth */
	qrBmpWriteLong(ptr, -height); /* biHeight */
	qrBmpWriteShort(ptr, 1); /* biPlanes */
	qrBmpWriteShort(ptr, 24); /* biBitCount - 1 bit per pixel */
	qrBmpWriteLong(ptr, 0); /* biCopmression - BI_RGB */
	qrBmpWriteLong(ptr, imagesize);     /* biSizeImage */
	qrBmpWriteLong(ptr, QRCNV_BMP_PPM); /* biXPixPerMeter */
	qrBmpWriteLong(ptr, QRCNV_BMP_PPM); /* biYPixPerMeter */
	qrBmpWriteLong(ptr, 2); /* biClrUsed */
	qrBmpWriteLong(ptr, 2); /* biCirImportant */

	/*
	 * RGBQUAD - white
	 */
	*ptr++ = 255; /* rgbBlue */
	*ptr++ = 255; /* rgbGreen */
	*ptr++ = 255; /* rgbRed */
	*ptr++ = 0;   /* rgbReserved */

	/*
	 *RGBQUAD - black
	 */
	*ptr++ = 0; /* rgbBlue */
	*ptr++ = 0; /* rgbGreen */
	*ptr++ = 0; /* rgbRed */
	*ptr++ = 0; /* rgbReserved */

	return ptr;
}


/* }}} */