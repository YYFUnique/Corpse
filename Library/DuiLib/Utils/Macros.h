#ifndef __MACROS__H__
#define __MACROS__H__

#pragma once

#define SAFE_DEL(p) if ((p) != NULL){ delete (p);(p)=NULL; }

#endif //__MACROS__H__