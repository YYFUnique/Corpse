// TestLua.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "Tlua.h"

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	LOAD_LUA_FILE(L,_T("C:\\luacall.lua")); //luacall.lua����һ��test����

	CLuaPool LuaPool;
	LuaPool.AddArg(5);
	LuaPool.AddArg(_T("ok"));
	LuaPool.AddArg(_T(true));
	LuaPool.CallLua(L,_T("test"));
	//PushArgToLua(L,_T("test"),argvalue); //����test����������Ϊ 3, "abc", true
	return 0;
}