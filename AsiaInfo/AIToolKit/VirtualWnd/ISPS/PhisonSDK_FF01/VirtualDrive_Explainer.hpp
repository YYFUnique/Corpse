/*-----------------------------------------------------------------------------
 *
 *   Copyright (C) 2002-2015, Asiainfo Inc.  All Rights Reserved.
 *
 *   This program is an unpublished copyrighted work which is proprietary
 *   to Asiainfo, Inc. and contains confidential information that is not
 *   to be reproduced or disclosed to any other person or entity without
 *   prior written consent from Asiainfo, Inc. in each and every instance.
 *
 *   WARNING:  Unauthorized reproduction of this program as well as
 *   unauthorized preparation of derivative works based upon the
 *   program or distribution of copies by sale, rental, lease or
 *   lending are violations of federal copyright laws and state trade
 *   secret laws, punishable by civil and criminal penalties.
 *
 *-----------------------------------------------------------------------------*/
#pragma once

#include "VirtualDrive.h"


namespace TMPS { namespace Firmware {


class VirtualDriveExplainer : public Result::Explainer
{
public:
	virtual bool GetFailJudge(const int _code) { return _code != VirtualDrive::Code_OK; }
	virtual const char* GetDesc(const int _code) {
		switch (_code) {
			case VirtualDrive::Code_OK: return "OK";
			case VirtualDrive::Code_AlreadyMounted: return "already mounted";
			case VirtualDrive::Code_WrongDrive: return "wrong drive";
			default: return "general failed";
		}
	}
};

extern VirtualDriveExplainer s_VD_explainer;

inline Result _MakeError(const int _code=VirtualDrive::Code_GeneralFail) { return Result(_code, s_VD_explainer); }
inline Result _MakeSuccess(const int _code=VirtualDrive::Code_OK) { return Result(_code, s_VD_explainer); }


}} // namespace TMPS::Firmware
