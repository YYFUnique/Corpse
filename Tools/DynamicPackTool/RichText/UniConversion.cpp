#include "StdAfx.h"
#include "UniConversion.h"

int UTF8BytesOfLead[256];
static BOOL bInitBytesOfLead = FALSE;

static int BytesFromLead(int leadByte) {
	if (leadByte < 0xC2) {
		// Single byte or invalid
		return 1;
	} else if (leadByte < 0xE0) {
		return 2;
	} else if (leadByte < 0xF0) {
		return 3;
	} else if (leadByte < 0xF5) {
		return 4;
	} else {
		// Characters longer than 4 bytes not possible in current UTF-8
		return 1;
	}
}

void UTF8BytesOfLeadInitialise() {
	if (!bInitBytesOfLead) {
		for (int n=0; n<256; ++n) {
			UTF8BytesOfLead[n] = BytesFromLead(n);
		}
		bInitBytesOfLead = true;
	}
}

int UTF8Classify(const BYTE *us, int len)
{
	// For the rules: http://www.cl.cam.ac.uk/~mgk25/unicode.html#utf-8
	if (*us < 0x80) {
		// Single bytes easy
		return 1;
	} else if (*us > 0xf4) {
		// Characters longer than 4 bytes not possible in current UTF-8
		return UTF8MaskInvalid | 1;
	} else if (*us >= 0xf0) {
		// 4 bytes
		if (len < 4)
			return UTF8MaskInvalid | 1;
		if (UTF8IsTrailByte(us[1]) && UTF8IsTrailByte(us[2]) && UTF8IsTrailByte(us[3])) {
			if (((us[1] & 0xf) == 0xf) && (us[2] == 0xbf) && ((us[3] == 0xbe) || (us[3] == 0xbf))) {
				// *FFFE or *FFFF non-character
				return UTF8MaskInvalid | 4;
			}
			if (*us == 0xf4) {
				// Check if encoding a value beyond the last Unicode character 10FFFF
				if (us[1] > 0x8f) {
					return UTF8MaskInvalid | 1;
				} else if (us[1] == 0x8f) {
					if (us[2] > 0xbf) {
						return UTF8MaskInvalid | 1;
					} else if (us[2] == 0xbf) {
						if (us[3] > 0xbf) {
							return UTF8MaskInvalid | 1;
						}
					}
				}
			} else if ((*us == 0xf0) && ((us[1] & 0xf0) == 0x80)) {
				// Overlong
				return UTF8MaskInvalid | 1;
			}
			return 4;
		} else {
			return UTF8MaskInvalid | 1;
		}
	} else if (*us >= 0xe0) {
		// 3 bytes
		if (len < 3)
			return UTF8MaskInvalid | 1;
		if (UTF8IsTrailByte(us[1]) && UTF8IsTrailByte(us[2])) {
			if ((*us == 0xe0) && ((us[1] & 0xe0) == 0x80)) {
				// Overlong
				return UTF8MaskInvalid | 1;
			}
			if ((*us == 0xed) && ((us[1] & 0xe0) == 0xa0)) {
				// Surrogate
				return UTF8MaskInvalid | 1;
			}
			if ((*us == 0xef) && (us[1] == 0xbf) && (us[2] == 0xbe)) {
				// U+FFFE non-character - 3 bytes long
				return UTF8MaskInvalid | 3;
			}
			if ((*us == 0xef) && (us[1] == 0xbf) && (us[2] == 0xbf)) {
				// U+FFFF non-character - 3 bytes long
				return UTF8MaskInvalid | 3;
			}
			if ((*us == 0xef) && (us[1] == 0xb7) && (((us[2] & 0xf0) == 0x90) || ((us[2] & 0xf0) == 0xa0))) {
				// U+FDD0 .. U+FDEF
				return UTF8MaskInvalid | 3;
			}
			return 3;
		} else {
			return UTF8MaskInvalid | 1;
		}
	} else if (*us >= 0xc2) {
		// 2 bytes
		if (len < 2)
			return UTF8MaskInvalid | 1;
		if (UTF8IsTrailByte(us[1])) {
			return 2;
		} else {
			return UTF8MaskInvalid | 1;
		}
	} else {
		// 0xc0 .. 0xc1 is overlong encoding
		// 0x80 .. 0xbf is trail byte
		return UTF8MaskInvalid | 1;
	}
}

BOOL UTF8IsTrailByte(int ch) 
{
	return (ch >= 0x80) && (ch < 0xc0);
}