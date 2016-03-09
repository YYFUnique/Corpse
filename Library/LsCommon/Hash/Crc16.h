#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

class LS_COMMON_API CCrc16
{
public:
	CCrc16();

	unsigned short CRC16Check(BYTE *FrameData,int FrameDataLength,unsigned short CheckCode);
private:
	unsigned short CRC16Table[256];
	void MakeCRC16Table();
	unsigned short GenCRC16(unsigned short Data,unsigned short GenPoly,unsigned short Accum);
};