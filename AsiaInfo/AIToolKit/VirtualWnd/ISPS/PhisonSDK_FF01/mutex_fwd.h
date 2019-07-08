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
    namespace TM { using Mutex = std::mutex; }
#else
#   include "platform_defines.h"
#   if _OS_WIN_
    namespace TM
    {
        namespace Detail { namespace Win { class Mutex; } }
        using TM::Detail::Win::Mutex;
    }
#   else
    namespace TM
    {
        namespace Detail { namespace Pthread { class Mutex; } }
        using TM::Detail::Pthread::Mutex;
    }
#   endif
#endif
