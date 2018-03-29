#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LIB_LUA_API 
#elif defined  LUA_EXPORTS
#define LIB_LUA_API __declspec(dllexport)
#else
#define LIB_LUA_API __declspec(dllimport)
#endif

class Lualib
{
public:
	Lualib();
	~Lualib();

public:
protected:
	lua_State* m_pLua;
};