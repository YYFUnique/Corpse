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
#include "platform_defines.h"
#if _OS_WIN_

#include "mutex_win.h"

#include "win_target_2000.h"
#include <windows.h>

namespace TM {
namespace Detail {
namespace Win {

class Mutex::Impl
{
public:
    Impl() {
        ::InitializeCriticalSection(&cs_);
    }
    ~Impl() {
        ::DeleteCriticalSection(&cs_);
    }

    void lock() {
        ::EnterCriticalSection(&cs_);
    }
    bool try_lock() {
        return (::TryEnterCriticalSection(&cs_) == TRUE);
    }
    void unlock() {
        ::LeaveCriticalSection(&cs_);
    }

    Mutex::native_handle_type native_handle() {
        return &cs_;
    }

private:
    ::CRITICAL_SECTION cs_;
};

Mutex::Mutex() : imp_(new Mutex::Impl) {}
Mutex::~Mutex() { delete imp_; imp_ = NULL; }
void Mutex::lock() { imp_->lock(); }
bool Mutex::try_lock() { return imp_->try_lock(); }
void Mutex::unlock() { imp_->unlock(); }
Mutex::native_handle_type Mutex::native_handle() { return imp_->native_handle(); }

}}} // TM::Detail::Win

#endif // _OS_WIN_
