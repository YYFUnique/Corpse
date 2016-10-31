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
* 库的版本
 */
#define LIBQR_VERSION "0.3.1"

/*
* code错误代码
 */
typedef enum {
	/* 通用code错误代码 */
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

	/* 参数用code错误代码 */
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

	/* 输入数据用code错误代码 */
	QR_ERR_EMPTY_SRC   = 0x10,
	QR_ERR_LARGE_SRC   = 0x11,
	QR_ERR_NOT_NUMERIC = 0x12,
	QR_ERR_NOT_ALNUM   = 0x13,
	QR_ERR_NOT_KANJI   = 0x14,

	/* 图像处理错误时的code代码 */
	QR_ERR_IMAGE_TOO_LARGE  = 0x30,
	QR_ERR_WIDTH_TOO_LARGE  = 0x31,
	QR_ERR_HEIGHT_TOO_LARGE = 0x32,
	QR_ERR_IMAGECREATE      = 0x33,
	QR_ERR_IMAGEFORMAT      = 0x34,
	QR_ERR_IMAGEFRAME       = 0x35,

	/* zlib用code错误代码 */
	QR_ERR_DEFLATE = 0x40
} qr_err_t;

/*
* 内部状态
 */
#define QR_STATE_BEGIN  0
#define QR_STATE_SET    1
#define QR_STATE_FINAL  2

/*
* 编码模式
 */
typedef enum {
	QR_EM_AUTO			= -1, /* 自动选择 */
	QR_EM_NUMERIC		=  0, /* 数字 */
	QR_EM_ALNUM			=  1, /* 英数字: 0-9 A-Z SP $%*+-./: */
	QR_EM_8BIT			=  2, /* 8比特字节 */
	QR_EM_KANJI			=  3  /* 汉字 */
} qr_em_t;

/* 模式总数 */
#define QR_EM_COUNT 4

/*
* 错误更正水平
 */
typedef enum {
	QR_ECL_L	= 0,		/* 低于正常 */
	QR_ECL_M	= 1,		/* レベルM */
	QR_ECL_Q	= 2,		/* レベルQ */
	QR_ECL_H	= 3			/* レベルH */
} qr_ecl_t;

/* 错误更正水平总数 */
#define QR_ECL_COUNT 4

/*
* 输出形式
 */
typedef enum {
	QR_FMT_PNG   =  0,		/* PNG */
	QR_FMT_BMP   =  1,		/* BMP */
	QR_FMT_TIFF  =  2,		/* TIFF */
	QR_FMT_PBM   =  3,		/* PBM */
	QR_FMT_SVG   =  4,		/* SVG */
	QR_FMT_JSON  =  5,		/* JSON */
	QR_FMT_DIGIT =  6,		/* 文字列 */
	QR_FMT_ASCII =  7,		/* ASCII码序列 */
	QR_FMT_UNAVAILABLE = -1 /* 不可使用 */
} QR_FROMAT_T;//qr_format_t;

/* 输出形式总数 */
#define QR_FMT_COUNT 8

/*
 * モジュール値のマスク
 */
#define QR_MM_DATA      0x01  /* 数据编码的黑色模块 */
#define QR_MM_BLACK     0x02  /* 印字される黒モジュール */
#define QR_MM_FUNC      0x04  /* 機能パターン領域(形式/型番情報を含む) */

/*
 * 機能パターンの定数
 */
#define QR_DIM_SEP      4  /* 分離パターンの幅 */
#define QR_DIM_FINDER   7  /* 位置検出パターンの1辺の長さ */
#define QR_DIM_ALIGN    5  /* 位置合わせパターンの1辺の長さ */
#define QR_DIM_TIMING   6  /* タイミングパターンのオフセット位置 */

/*
 * サイズ定数
 */
#define QR_SRC_MAX  7089  /* 入力データの最大長 */
#define QR_DIM_MAX   177  /* 1辺のモジュール数の最大値 */
#define QR_VER_MAX    40  /* 型番の最大値 */
#define QR_DWD_MAX  2956  /* データコード語の最大長(型番40/レベルL) */
#define QR_ECW_MAX  2430  /* 誤り訂正コード語の最大長(型番40/レベルH) */
#define QR_CWD_MAX  3706  /* コード語の最大長(型番40) */
#define QR_RSD_MAX   123  /* RSブロックデータコード語の最大長 */
#define QR_RSW_MAX    68  /* RSブロック誤り訂正コード語の最大長 */
#define QR_RSB_MAX     2  /* RSブロック種別の最大数 */
#define QR_MPT_MAX     8  /* マスクパターン種別総数 */
#define QR_APL_MAX     7  /* 位置合わせパターン座標の最大数 */
#define QR_FIN_MAX    15  /* 形式情報のビット数 */
#define QR_VIN_MAX    18  /* 型番情報のビット数 */
#define QR_MAG_MAX    16  /* ピクセル表示倍率の最大値 */
#define QR_SEP_MAX    16  /* 分離パターン幅の最大値 */
#define QR_ERR_MAX  1024  /* エラー情報の最大長 */
#define QR_STA_MAX    16  /* 構造的連接(分割/連結)の最大数 */
#define QR_STA_LEN    20  /* 構造的連接ヘッダのビット数 */

/*
 * その他の定数
 */
#define NAV            0  /* 不使用(not available) */
#define PADWORD1    0xec  /* 埋め草コード語1: 11101100 */
#define PADWORD2    0x11  /* 埋め草コード語2: 00010001 */
#define VERPOINT1      9  /* 文字数指示子のビット数が変わる直前の型番1 */
#define VERPOINT2     26  /* 文字数指示子のビット数が変わる直前の型番2 */

/*
 * 8bitバイナリデータ型
 */
typedef unsigned char qr_byte_t;

/*
 * RSブロックごとの情報
 */
typedef struct qr_rsblock_t {
  int rsbnum;     /* RSブロック数 */
  int totalwords; /* RSブロック総コード語数 */
  int datawords;  /* RSブロックデータコード語数 */
  int ecnum;      /* RSブロック誤り訂正数(不使用) */
} qr_rsblock_t;

/*
 * 誤り訂正レベルごとの情報
 */
typedef struct qr_eclevel_t {
  int datawords;                /* データコード語数(全RSブロック) */
  int capacity[QR_EM_COUNT];    /* 符号化モードごとのデータ容量 */
  int nrsb;                     /* RSブロックの種類(1または2) */
  qr_rsblock_t rsb[QR_RSB_MAX]; /* RSブロックごとの情報 */
} qr_eclevel_t;

/*
 * 型番ごとの情報
 */
typedef struct qr_vertable_t {
  int          version;           /* 型番 */
  int          dimension;         /* 1辺のモジュール数 */
  int          totalwords;        /* 総コード語数 */
  int          remainedbits;      /* 剰余ビット数 */
  int          nlen[QR_EM_COUNT]; /* 文字数指示子のビット数 */
  qr_eclevel_t ecl[QR_ECL_COUNT]; /* 誤り訂正レベルごとの情報 */
  int          aplnum;            /* 位置合わせパターン中心座標数 */
  int          aploc[QR_APL_MAX]; /* 位置合わせパターン中心座標 */
} qr_vertable_t;

/*
 * 座標データ型
 */
typedef struct qr_coord_t { int ypos, xpos; } qr_coord_t;

/*
 * パラメータ構造体
 */
typedef struct qr_param_t {
  int version;              /* 型番 */
  int mode;                 /* 符号化モード */
  int eclevel;              /* 誤り訂正レベル */
  int masktype;             /* マスクパターン種別 */
} qr_param_t;

/*
 * QRコードオブジェクト
 */
typedef struct qrcode_t {
  qr_byte_t *dataword;      /* データコード語領域のアドレス */
  qr_byte_t *ecword;        /* 誤り訂正コード語領域のアドレス */
  qr_byte_t *codeword;      /* シンボル配置用コード語領域のアドレス */
  qr_byte_t *_symbol;       /* シンボルデータ領域のアドレス */
  qr_byte_t **symbol;       /* シンボルデータの各行頭のアドレスのポインタ */
  qr_byte_t *source;        /* 入力データ領域のアドレス */
  size_t srcmax;            /* 入力データ領域の最大容量 */
  size_t srclen;            /* 入力データ領域の使用容量 */
  int enclen;               /* データコード語の総ビット長 */
  int delta1, delta2;       /* 型番自動選択の補助に使われるビット長差分 */
  int dwpos;                /* データコード語の追加バイト位置 */
  int dwbit;                /* データコード語の追加ビット位置 */
  int xpos, ypos;           /* モジュールを配置する座標位置 */
  int xdir, ydir;           /* モジュール配置の移動方向 */
  int state;                /* 処理の進行状況 */
  int errcode;              /* 最後に起こったエラーの番号 */
  char errinfo[QR_ERR_MAX]; /* 最後に起こったエラーの詳細 */
  qr_param_t param;         /* 出力パラメータ */
} QRCode;

/*
 * 構造的連接QRコードオブジェクト
 */
typedef struct qrcode_sa_t {
  QRCode *qrs[QR_STA_MAX];  /* QRコードオブジェクトのポインタ配列 */
  QRCode *cur;              /* 値を入力する対象のQRコードオブジェクト */
  int num;                  /* シンボル数 */
  int max;                  /* 最大シンボル数 */
  int parity;               /* パリティ */
  int state;                /* 処理の進行状況 */
  qr_param_t param;         /* 出力パラメータ */
} QRStructured;

/*
 * QRコード出力関数型
 */
typedef qr_byte_t *(*QRConverter)(QRCode *, int, int, int *);
typedef qr_byte_t *(*QRsConverter)(QRStructured *, int, int, int, int *);

/*
 * 基本関数のプロトタイプ
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
 * 構造的連接操作用関数のプロトタイプ
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
 * 出力用関数のプロトタイプ
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
 * 構造的連接出力用関数のプロトタイプ
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
