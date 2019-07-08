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

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>


#define SMART_ENUM(EnumName, ...)                                   \
class EnumName                                                      \
{                                                                   \
public:                                                             \
    EnumName() : value(0) {}                                        \
    EnumName(int x) : value(x) {}                                   \
                                                                    \
    enum {__VA_ARGS__};                                             \
                                                                    \
    operator int () const { return value; }                         \
                                                                    \
    const char* ToString(void) const {                              \
        return ToString(value);                                     \
    }                                                               \
                                                                    \
    static const char* ToString(int aInt) {                         \
        return NameMap()[aInt].c_str();                             \
    }                                                               \
                                                                    \
    static EnumName FromString(const std::string& s)                \
    {                                                               \
        for (NameMap_t::iterator it = NameMap().begin();            \
             it != NameMap().end();                                 \
             ++it)                                                  \
        {                                                           \
            if (it->second == s)                                    \
                return it->first;                                   \
        }                                                           \
		return -1;                                                  \
    }                                                               \
                                                                    \
    /*static std::vector<EnumName> allValues() {*/                  \
    /*  std::vector<EnumName> x{ __VA_ARGS__ }; */                  \
    /*  return x;                               */                  \
    /*}                                         */                  \
                                                                    \
    bool operator<(const EnumName a) const {                        \
        return (int)*this < (int)a;                                 \
    }                                                               \
                                                                    \
private:                                                            \
    int value;                                                      \
                                                                    \
	typedef std::map<int,std::string> NameMap_t;                    \
    static NameMap_t& NameMap() {                                   \
      static NameMap_t s_nameMap;                                   \
      if (s_nameMap.size() == 0)                                    \
          initMap(s_nameMap);                                       \
      return s_nameMap;                                             \
    }                                                               \
                                                                    \
    static void initMap(NameMap_t& tmp)                             \
    {                                                               \
        using namespace std;                                        \
                                                                    \
        int val = 0;                                                \
        string buf_1, buf_2, str = #__VA_ARGS__;                    \
        replace(str.begin(), str.end(), '=', ' ');                  \
        stringstream stream(str);                                   \
        vector<string> strings;                                     \
        while (getline(stream, buf_1, ','))                         \
            strings.push_back(buf_1);                               \
        for(vector<string>::iterator it = strings.begin();          \
            it != strings.end();                                    \
            ++it)                                                   \
        {                                                           \
            buf_1.clear(); buf_2.clear();                           \
            stringstream localStream(*it);                          \
            localStream>> buf_1 >> buf_2;                           \
            if(buf_2.size() > 0)                                    \
                val = atoi(buf_2.c_str());                          \
            tmp[val++] = buf_1;                                     \
        }                                                           \
    }                                                               \
};

