// TestLua.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "Tlua.h"

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	LOAD_LUA_FILE(L,_T("C:\\luacall.lua")); //luacall.lua中有一个test函数

	CLuaPool LuaPool;
	LuaPool.AddArg(5);
	LuaPool.AddArg(_T("ok"));
	LuaPool.AddArg(_T(true));
	LuaPool.CallLua(L,_T("test"));
	//PushArgToLua(L,_T("test"),argvalue); //调用test函数，参数为 3, "abc", true
	return 0;
}