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

#include "compiler_defines.h"
#if _CPP_11_ENABLE_
#   include <mutex>
#else
#   include "platform_defines.h"
#endif

namespace TM
{
namespace Detail
{

template <class MutexT>
class LockGuard
{
public:
    typedef MutexT mutex_type;

    explicit LockGuard(mutex_type &_m) : m_(_m) {
        m_.lock();
    }
    ~LockGuard() {
        m_.unlock();
    }

private:
    LockGuard(const LockGuard &);
    LockGuard& operator= (const LockGuard &);

    mutex_type& m_;
};

} // TM::Detail

#if _CPP_11_ENABLE_
    template <class T>
    using LockGuard = std::lock_guard<T>;
#else
    using TM::Detail::LockGuard;
#endif

} // TM
