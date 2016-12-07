#include "stdafx.h"
#include "crc16.h"

CCrc16::CCrc16()
{
	MakeCRC16Table();
}

unsigned short CCrc16::CRC16Check(BYTE *FrameData,int FrameDataLength,unsigned short CheckCode)
{
	unsigned short DataLength;
	unsigned short CRCValue,i;
	
	CRCValue=CheckCode;
	DataLength=FrameDataLength;
	
	for (i=0;i<DataLength;i++)
		CRCValue=((CRCValue>>8)^CRC16Table[(CRCValue^(FrameData[i]+i))&0x00ff]);
	
	//CheckCode=CRCValue;
	return CRCValue;
}

void CCrc16::MakeCRC16Table()
{
	for (int i=0;i<256;i++)
		CRC16Table[i]=GenCRC16(i,0xa001,0);
}

unsigned short CCrc16::GenCRC16(unsigned short Data,unsigned short GenPoly,unsigned short Accum)
{
	for (int i=0;i<8;i++)
	{
		if ((Data^Accum)&0x0001)
			Accum=(unsigned short)(Accum>>1)^GenPoly;
		else
			Accum>>=1;
		Data>>=1;
	}
	return (Accum);
}