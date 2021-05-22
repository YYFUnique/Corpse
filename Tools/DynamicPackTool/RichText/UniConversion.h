#pragma once

const int UTF8MaxBytes = 4;
extern int UTF8BytesOfLead[256];
void UTF8BytesOfLeadInitialise();
BOOL UTF8IsTrailByte(int nch);

inline bool UTF8IsAscii(int ch) {
	return ch < 0x80;
}
inline bool IsSpaceOrTab(int ch) {
	return ch == ' ' || ch == '\t';
}

enum { UTF8MaskWidth=0x7, UTF8MaskInvalid=0x8 };
int UTF8Classify(const BYTE *us, int len);