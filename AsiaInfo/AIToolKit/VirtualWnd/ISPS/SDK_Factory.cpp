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
#include "StdAfx.h"
#include "PhisonSDK_FF01/FirmwareResult.h"
#include "PhisonSDK_FF01/PhisonSDK_FF01.h"

/*#include "libLog/DebugLog.h"*/
#include "PhisonSDK_FF01/mutex.h"

namespace TMPS { namespace Firmware {

struct InitFailExplainer : public Result::Explainer
{
	virtual const char* GetDesc(const int) { return "Not initialized"; }
	virtual bool GetFailJudge(const int) { return true; }
};
static InitFailExplainer s_initFailExplainer;


struct InvalidSDK : public SDK_Interface
{
	virtual Result InitAPI() { return MakeResult(0); }
	virtual Result CloseAPI() { return MakeResult(0); }

	virtual Result GetSDKVersion(uint32_t *) { return MakeResult(0); }

	virtual Result GetDriveLetterList(DriveLettersList_t *) { return MakeResult(0); }
	virtual Result GetDriveLetters(const char, DriveLetters_t *) { return MakeResult(0); }

	virtual Result GetDeviceInfo(const char, DeviceInfo_t *) { return MakeResult(0); }
	virtual Result GetUnitSize(const char, uint32_t *) { return MakeResult(0); }

	virtual Result GetWriteProtectStatus(const char, bool *) { return MakeResult(0); }
	virtual Result SetWriteProtect(const char, const bool, const bool ) { return MakeResult(0); }

	virtual Result ReadSector(
		const PartitionID_t, const char,
		const uint32_t, const uint32_t,
		const unsigned char*
	) { return MakeResult(0); }

	virtual Result WriteSector(
		const PartitionID_t, const char,
		const uint32_t, const uint32_t,
		const unsigned char*
	) { return MakeResult(0); }

	virtual Result FormatPartition(const char, const char*) { return MakeResult(0); }

	virtual Result ResizePublicPartitions(
		const char, const uint32_t,
		const char*, const char*
	) { return MakeResult(0); }

	virtual Result SetVID(const char _driveLetter, const char* _vid) { return MakeResult(0); }
	virtual Result SetPID(const char _driveLetter, const char* _pid) { return MakeResult(0); }
	virtual Result SetVendorName(const char _driveLetter, const std::string &_name) { return MakeResult(0); }
	virtual Result SetProductName(const char _driveLetter, const std::string &_name) { return MakeResult(0); }

	virtual Result GetLEDStatus(const char, LED_Status_t [3]) { return MakeResult(0); }
	virtual Result SetLEDStatus(const char, const LED_Index_t, const LED_Status_t) { return MakeResult(0); }

	virtual Result MakeResult(int _code) { return Result(_code, s_initFailExplainer); }
	virtual Result DumpDeviceInfo(const char, std::string *) { return MakeResult(0); }
};
static InvalidSDK s_invalidSDK;


class SDK_Factory
{
public:
	static SDK_Interface* CreateSDK() {
		SDK_Interface* p_sdk = CreateParticularSDK<PhisonSDK_FF01>();
		if (p_sdk != NULL) {
			/*LOGINFO("using firmware SDK: PhisonSDK_FF01");*/
			return p_sdk;
		}
		// PhisonSDK_FF01 failed, try next choice
		return NULL;
	}
	static void ReleaseSDK(SDK_Interface* _sdk) {
		Result res = _sdk->CloseAPI();
		if (res.IsFailCode()) {
			/*LOGERROR("Failed to close firmware SDK: %s(%d)", res.Desc(), res.Code());*/
		}
		delete _sdk;
	}

private:
	SDK_Factory();

	template <class SDK_Type>
	static SDK_Interface* CreateParticularSDK() {
		SDK_Interface* p_sdk = NULL;
		try {
			p_sdk = new SDK_Type();
			Result res = p_sdk->InitAPI();
			if (res.IsFailCode())
				throw res;
			return p_sdk;
		} catch (std::bad_alloc) {
			/*LOGERROR("memory insufficient");*/
		} catch (Result res) {
			//
			if (res.IsFailCode()) {
				/*LOGERROR("Failed to close firmware SDK: %s(%d)", res.Desc(), res.Code());*/
			}
		}
		delete p_sdk;
		return NULL;
	}
};


class SDK_Sentry
{
public:
	SDK_Sentry(SDK_Interface* _sdk = NULL) : sdk_(_sdk) {}
	~SDK_Sentry() { FreeSDK(); }

	SDK_Interface* GetSDK() { return sdk_; }

	void SetSDK(SDK_Interface* _sdk) {
		if (sdk_ == _sdk)
			return;
		FreeSDK();
		sdk_ = _sdk;
	}

	void FreeSDK() {
		if (sdk_ == NULL)
			return;

		SDK_Factory::ReleaseSDK(sdk_);
		sdk_ = NULL;
	}

private:
	SDK_Sentry(const SDK_Sentry &);
	SDK_Sentry& operator= (const SDK_Sentry &);

	SDK_Interface *sdk_;
};


SDK_Interface& SDK()
{
	static SDK_Interface* volatile s_selected = NULL;
	static SDK_Sentry s_sentry;
	static TM::Mutex s_mutex;

	if (s_selected == NULL)
	{
		TM::LockGuard<TM::Mutex> lock(s_mutex);

		if (s_selected == NULL)
		{
			s_sentry.SetSDK(SDK_Factory::CreateSDK());
			if (s_sentry.GetSDK() != NULL)
			{
				s_selected = s_sentry.GetSDK();
			}
			else
			{
				return s_invalidSDK;
			}
		}
	}

	return *s_selected;
}

}}