#pragma once
#include <atlstr.h>
#include <vector>
#ifdef __cplusplus

extern "C" {
#include "Lua/lua.hpp"
#include "Lua/lualib.h"
#include "Lua/lauxlib.h"
};
#endif

#define LOAD_LUA_FILE(Lua,File) (luaL_loadfile((Lua),(File)) || lua_pcall((L), 0, LUA_MULTRET, 0))

class CArgLua
{
public:
	CArgLua(){}
	virtual void PushValue(lua_State* Lua) const = 0;
};

typedef std::vector<CArgLua*> CVectorList;

class TIntLua : public CArgLua
{
	int m_nVal;
public:
	explicit TIntLua(int v):m_nVal(v){}
	virtual void PushValue(lua_State* Lua) const {lua_pushinteger(Lua, m_nVal);}
};
class TStrLua:public CArgLua
{
	CString m_strVal;
public:
	explicit TStrLua(LPCTSTR lpszLuaString):m_strVal(lpszLuaString){}
	virtual void PushValue(lua_State* Lua) const {lua_pushstring(Lua, m_strVal);}
};

class TBoolLua:public CArgLua
{
	bool m_bVal;
public:
	explicit TBoolLua(bool v):m_bVal(v){}
	virtual void PushValue(lua_State* Lua) const {lua_pushboolean(Lua, m_bVal);}
};

class CLuaPool
{
public:  
	CLuaPool();
	~CLuaPool(); 
public:
	void AddArg(int dVal);  
	void AddArg(LPCTSTR lpszVal);  
	void AddArg(bool bVal);
	int Push(lua_State* L) const;  	
	BOOL CallLua(lua_State* L, LPCTSTR lpszFuncName);
protected:
	CVectorList m_ArgList;
};