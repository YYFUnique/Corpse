/*
 * QR Code Generator Library: Basic Header
 *
 * Core routines were originally written by Junn Ohta.
 * Based on qr.c Version 0.1: 2004/4/3 (Public Domain)
 *
 * @package     libqr
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2006-2013 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#ifndef _QR_H_
#define _QR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdio.h>

#ifdef LS_STATIC_LIB_CALL
#define QR_API 
#elif defined  QR_EXPORTS
#define QR_API __declspec(dllexport)
#else
#define QR_API __declspec(dllimport)
#endif

/*
* ��İ汾
 */
#define LIBQR_VERSION "0.3.1"

/*
* code�������
 */
typedef enum {
	/* ͨ��code������� */
	QR_ERR_NONE             = 0,
	QR_ERR_USAGE            = 0x68,
	QR_ERR_NOT_IMPL         = 0x69,
	QR_ERR_SEE_ERRNO        = 0x6e,
	QR_ERR_FOPEN            = 0x6f,
	QR_ERR_FREAD            = 0x72,
	QR_ERR_STATE            = 0x73,
	QR_ERR_UNKNOWN          = 0x75,
	QR_ERR_FWRITE           = 0x77,
	QR_ERR_MEMORY_EXHAUSTED = 0x78,

	/* ������code������� */
	QR_ERR_INVALID_ARG     = 0x01,
	QR_ERR_INVALID_VERSION = 0x02,
	QR_ERR_INVALID_MODE    = 0x03,
	QR_ERR_INVALID_ECL     = 0x04,
	QR_ERR_INVALID_MPT     = 0x05,
	QR_ERR_INVALID_MAG     = 0x06,
	QR_ERR_INVALID_SEP     = 0x07,
	QR_ERR_INVALID_SIZE    = 0x08,
	QR_ERR_INVALID_FMT     = 0x09,
	QR_ERR_INVALID_OUT     = 0x0a,
	QR_ERR_INVALID_MAXNUM  = 0x0b,
	QR_ERR_UNSUPPORTED_FMT = 0x0c,
	QR_ERR_EMPTY_PARAM     = 0x0f,

	/* ����������code������� */
	QR_ERR_EMPTY_SRC   = 0x10,
	QR_ERR_LARGE_SRC   = 0x11,
	QR_ERR_NOT_NUMERIC = 0x12,
	QR_ERR_NOT_ALNUM   = 0x13,
	QR_ERR_NOT_KANJI   = 0x14,

	/* ͼ�������ʱ��code���� */
	QR_ERR_IMAGE_TOO_LARGE  = 0x30,
	QR_ERR_WIDTH_TOO_LARGE  = 0x31,
	QR_ERR_HEIGHT_TOO_LARGE = 0x32,
	QR_ERR_IMAGECREATE      = 0x33,
	QR_ERR_IMAGEFORMAT      = 0x34,
	QR_ERR_IMAGEFRAME       = 0x35,

	/* zlib��code������� */
	QR_ERR_DEFLATE = 0x40
} qr_err_t;

/*
* �ڲ�״̬
 */
#define QR_STATE_BEGIN  0
#define QR_STATE_SET    1
#define QR_STATE_FINAL  2

/*
* ����ģʽ
 */
typedef enum {
	QR_EM_AUTO			= -1, /* �Զ�ѡ�� */
	QR_EM_NUMERIC		=  0, /* ���� */
	QR_EM_ALNUM			=  1, /* Ӣ����: 0-9 A-Z SP $%*+-./: */
	QR_EM_8BIT			=  2, /* 8�����ֽ� */
	QR_EM_KANJI			=  3  /* ���� */
} qr_em_t;

/* ģʽ���� */
#define QR_EM_COUNT 4

/*
* �������ˮƽ
 */
typedef enum {
	QR_ECL_L	= 0,		/* �������� */
	QR_ECL_M	= 1,		/* ��٥�M */
	QR_ECL_Q	= 2,		/* ��٥�Q */
	QR_ECL_H	= 3			/* ��٥�H */
} qr_ecl_t;

/* �������ˮƽ���� */
#define QR_ECL_COUNT 4

/*
* �����ʽ
 */
typedef enum {
	QR_FMT_PNG   =  0,		/* PNG */
	QR_FMT_BMP   =  1,		/* BMP */
	QR_FMT_TIFF  =  2,		/* TIFF */
	QR_FMT_PBM   =  3,		/* PBM */
	QR_FMT_SVG   =  4,		/* SVG */
	QR_FMT_JSON  =  5,		/* JSON */
	QR_FMT_DIGIT =  6,		/* ������ */
	QR_FMT_ASCII =  7,		/* ASCII������ */
	QR_FMT_UNAVAILABLE = -1 /* ����ʹ�� */
} QR_FROMAT_T;//qr_format_t;

/* �����ʽ���� */
#define QR_FMT_COUNT 8

/*
 * �⥸��`�낎�Υޥ���
 */
#define QR_MM_DATA      0x01  /* ���ݱ���ĺ�ɫģ�� */
#define QR_MM_BLACK     0x02  /* ӡ�֤�����\�⥸��`�� */
#define QR_MM_FUNC      0x04  /* �C�ܥѥ��`���I��(��ʽ/�ͷ����򺬤�) */

/*
 * �C�ܥѥ��`��ζ���
 */
#define QR_DIM_SEP      4  /* ���x�ѥ��`��η� */
#define QR_DIM_FINDER   7  /* λ�×ʳ��ѥ��`���1�x���L�� */
#define QR_DIM_ALIGN    5  /* λ�úϤ碌�ѥ��`���1�x���L�� */
#define QR_DIM_TIMING   6  /* �����ߥ󥰥ѥ��`��Υ��ե��å�λ�� */

/*
 * ����������
 */
#define QR_SRC_MAX  7089  /* �����ǩ`��������L */
#define QR_DIM_MAX   177  /* 1�x�Υ⥸��`��������� */
#define QR_VER_MAX    40  /* �ͷ������ */
#define QR_DWD_MAX  2956  /* �ǩ`�����`���Z������L(�ͷ�40/��٥�L) */
#define QR_ECW_MAX  2430  /* �`��ӆ�����`���Z������L(�ͷ�40/��٥�H) */
#define QR_CWD_MAX  3706  /* ���`���Z������L(�ͷ�40) */
#define QR_RSD_MAX   123  /* RS�֥�å��ǩ`�����`���Z������L */
#define QR_RSW_MAX    68  /* RS�֥�å��`��ӆ�����`���Z������L */
#define QR_RSB_MAX     2  /* RS�֥�å��N�e������� */
#define QR_MPT_MAX     8  /* �ޥ����ѥ��`��N�e�t�� */
#define QR_APL_MAX     7  /* λ�úϤ碌�ѥ��`�����ˤ������ */
#define QR_FIN_MAX    15  /* ��ʽ���Υӥå��� */
#define QR_VIN_MAX    18  /* �ͷ����Υӥå��� */
#define QR_MAG_MAX    16  /* �ԥ������ʾ���ʤ���� */
#define QR_SEP_MAX    16  /* ���x�ѥ��`�������� */
#define QR_ERR_MAX  1024  /* ����`��������L */
#define QR_STA_MAX    16  /* ������B��(�ָ�/�B�Y)������� */
#define QR_STA_LEN    20  /* ������B�ӥإå��Υӥå��� */

/*
 * �������ζ���
 */
#define NAV            0  /* ��ʹ��(not available) */
#define PADWORD1    0xec  /* ���ݥ��`���Z1: 11101100 */
#define PADWORD2    0x11  /* ���ݥ��`���Z2: 00010001 */
#define VERPOINT1      9  /* ������ָʾ�ӤΥӥå���������ֱǰ���ͷ�1 */
#define VERPOINT2     26  /* ������ָʾ�ӤΥӥå���������ֱǰ���ͷ�2 */

/*
 * 8bit�Х��ʥ�ǩ`����
 */
typedef unsigned char qr_byte_t;

/*
 * RS�֥�å����Ȥ����
 */
typedef struct qr_rsblock_t {
  int rsbnum;     /* RS�֥�å��� */
  int totalwords; /* RS�֥�å��t���`���Z�� */
  int datawords;  /* RS�֥�å��ǩ`�����`���Z�� */
  int ecnum;      /* RS�֥�å��`��ӆ����(��ʹ��) */
} qr_rsblock_t;

/*
 * �`��ӆ����٥뤴�Ȥ����
 */
typedef struct qr_eclevel_t {
  int datawords;                /* �ǩ`�����`���Z��(ȫRS�֥�å�) */
  int capacity[QR_EM_COUNT];    /* ���Ż���`�ɤ��ȤΥǩ`������ */
  int nrsb;                     /* RS�֥�å��ηN�(1�ޤ���2) */
  qr_rsblock_t rsb[QR_RSB_MAX]; /* RS�֥�å����Ȥ���� */
} qr_eclevel_t;

/*
 * �ͷ����Ȥ����
 */
typedef struct qr_vertable_t {
  int          version;           /* �ͷ� */
  int          dimension;         /* 1�x�Υ⥸��`���� */
  int          totalwords;        /* �t���`���Z�� */
  int          remainedbits;      /* ����ӥå��� */
  int          nlen[QR_EM_COUNT]; /* ������ָʾ�ӤΥӥå��� */
  qr_eclevel_t ecl[QR_ECL_COUNT]; /* �`��ӆ����٥뤴�Ȥ���� */
  int          aplnum;            /* λ�úϤ碌�ѥ��`������������ */
  int          aploc[QR_APL_MAX]; /* λ�úϤ碌�ѥ��`���������� */
} qr_vertable_t;

/*
 * ���˥ǩ`����
 */
typedef struct qr_coord_t { int ypos, xpos; } qr_coord_t;

/*
 * �ѥ��`��������
 */
typedef struct qr_param_t {
  int version;              /* �ͷ� */
  int mode;                 /* ���Ż���`�� */
  int eclevel;              /* �`��ӆ����٥� */
  int masktype;             /* �ޥ����ѥ��`��N�e */
} qr_param_t;

/*
 * QR���`�ɥ��֥�������
 */
typedef struct qrcode_t {
  qr_byte_t *dataword;      /* �ǩ`�����`���Z�I��Υ��ɥ쥹 */
  qr_byte_t *ecword;        /* �`��ӆ�����`���Z�I��Υ��ɥ쥹 */
  qr_byte_t *codeword;      /* ����ܥ������å��`���Z�I��Υ��ɥ쥹 */
  qr_byte_t *_symbol;       /* ����ܥ�ǩ`���I��Υ��ɥ쥹 */
  qr_byte_t **symbol;       /* ����ܥ�ǩ`���θ����^�Υ��ɥ쥹�Υݥ��� */
  qr_byte_t *source;        /* �����ǩ`���I��Υ��ɥ쥹 */
  size_t srcmax;            /* �����ǩ`���I���������� */
  size_t srclen;            /* �����ǩ`���I���ʹ������ */
  int enclen;               /* �ǩ`�����`���Z�ξt�ӥå��L */
  int delta1, delta2;       /* �ͷ��Ԅ��x�k���a����ʹ����ӥå��L��� */
  int dwpos;                /* �ǩ`�����`���Z��׷�ӥХ���λ�� */
  int dwbit;                /* �ǩ`�����`���Z��׷�ӥӥå�λ�� */
  int xpos, ypos;           /* �⥸��`������ä�������λ�� */
  int xdir, ydir;           /* �⥸��`�����ä��Ƅӷ��� */
  int state;                /* �I����M��״�r */
  int errcode;              /* ������𤳤ä�����`�η��� */
  char errinfo[QR_ERR_MAX]; /* ������𤳤ä�����`��Ԕ�� */
  qr_param_t param;         /* �����ѥ��`�� */
} QRCode;

/*
 * ������B��QR���`�ɥ��֥�������
 */
typedef struct qrcode_sa_t {
  QRCode *qrs[QR_STA_MAX];  /* QR���`�ɥ��֥������ȤΥݥ������� */
  QRCode *cur;              /* �����������댝���QR���`�ɥ��֥������� */
  int num;                  /* ����ܥ��� */
  int max;                  /* ��󥷥�ܥ��� */
  int parity;               /* �ѥ�ƥ� */
  int state;                /* �I����M��״�r */
  qr_param_t param;         /* �����ѥ��`�� */
} QRStructured;

/*
 * QR���`�ɳ����v����
 */
typedef qr_byte_t *(*QRConverter)(QRCode *, int, int, int *);
typedef qr_byte_t *(*QRsConverter)(QRStructured *, int, int, int, int *);

/*
 * �����v���Υץ�ȥ�����
 */
QR_API QRCode *qrInit(int version, int mode, int eclevel, int masktype, int *errcode);
QR_API void qrDestroy(QRCode *qr);
QR_API int qrGetErrorCode(QRCode *qr);
QR_API char *qrGetErrorInfo(QRCode *qr);
QR_API int qrAddData(QRCode *qr, const qr_byte_t *source, int size);
QR_API int qrAddData2(QRCode *qr, const qr_byte_t *source, int size, int mode);
QR_API int qrFinalize(QRCode *qr);
QR_API int qrIsFinalized(const QRCode *qr);
QR_API int qrHasData(const QRCode *qr);
QR_API QRCode *qrClone(const QRCode *qr, int *errcode);

/*
 * ������B�Ӳ������v���Υץ�ȥ�����
 */
QR_API QRStructured *qrsInit(int version, int mode, int eclevel, int masktype, int maxnum, int *errcode);
QR_API void qrsDestroy(QRStructured *st);
QR_API int qrsGetErrorCode(QRStructured *st);
QR_API char *qrsGetErrorInfo(QRStructured *st);
QR_API int qrsAddData(QRStructured *st, const qr_byte_t *source, int size);
QR_API int qrsAddData2(QRStructured *st, const qr_byte_t *source, int size, int mode);
QR_API int qrsFinalize(QRStructured *st);
QR_API int qrsIsFinalized(const QRStructured *st);
QR_API int qrsHasData(const QRStructured *st);
QR_API QRStructured *qrsClone(const QRStructured *st, int *errcode);

QR_API void qrFreeBMP(qr_byte_t* qrData);

/*
 * �������v���Υץ�ȥ�����
 */
QR_API int qrOutputSymbol(QRCode *qr, FILE *fp, int fmt, int sep, int mag);
QR_API int qrOutputSymbol2(QRCode *qr, const char *pathname, int fmt, int sep, int mag);
QR_API qr_byte_t *qrGetSymbol(QRCode *qr, int fmt, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToDigit(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToASCII(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToJSON(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToPBM(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToBMP(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToBMP2(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToSVG(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToTIFF(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToPNG(QRCode *qr, int sep, int mag, int *size);

/*
 * ������B�ӳ������v���Υץ�ȥ�����
 */
QR_API int qrsOutputSymbols(QRStructured *st, FILE *fp, int fmt, int sep, int mag, int order);
QR_API int qrsOutputSymbols2(QRStructured *st, const char *pathname, int fmt, int sep, int mag, int order);
QR_API qr_byte_t *qrsGetSymbols(QRStructured *st, int fmt, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToDigit(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToASCII(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToJSON(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToPBM(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToBMP(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToSVG(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToTIFF(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToPNG(QRStructured *st, int sep, int mag, int order, int *size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _QR_H_ */
