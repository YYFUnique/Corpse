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
namespace TMPS { namespace Firmware {
class Result
{
public:
	class Explainer
	{
	public:
		virtual const char* GetDesc(const int) { return ""; }
		virtual bool GetFailJudge(const int) { return true; }
	};

	Result(const int _code) : code_(_code), explainer_(&s_dummyExplainer) {}
	Result(const int _code, Explainer &_explainer) : code_(_code), explainer_(&_explainer) {}

	operator int () const { return code_; }

	int Code() const { return code_; }
	const char* Desc() const { return explainer_->GetDesc(code_); }
	bool IsFailCode() const { return explainer_->GetFailJudge(code_); }

private:
	int code_;
	Explainer* explainer_;

	static Explainer s_dummyExplainer;
};
}}