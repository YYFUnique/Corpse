#include "stdafx.h"
#include "TLua.h"

BOOL PushArgToLua(lua_State* Lua, LPCTSTR lpszLuaFileName, const CVectorList& argvalue)
{
	lua_getglobal(Lua, lpszLuaFileName);
	for (size_t i = 0; i < argvalue.size(); i++)
		argvalue[i]->PushValue(Lua);

	return TRUE;
}

CLuaPool::CLuaPool()
{

}

void CLuaPool::AddArg(int nVal)
{
	TIntLua* pObj = new TIntLua(nVal);
	m_ArgList.push_back(pObj);
}

void CLuaPool::AddArg(LPCTSTR lpszVal)
{
	TStrLua* pObj = new TStrLua(lpszVal);
	m_ArgList.push_back(pObj);
}

void CLuaPool::AddArg(bool bVal)
{
	TBoolLua* pObj = new TBoolLua(bVal);
	m_ArgList.push_back(pObj);
}

CLuaPool::~CLuaPool()
{
	for (size_t i = 0; i < m_ArgList.size(); i++)
		delete m_ArgList[i];

	m_ArgList.clear();
}

int CLuaPool::Push(lua_State* L) const
{
	for (size_t i = 0; i < m_ArgList.size(); i++)  
		m_ArgList[i]->PushValue(L);  

     return m_ArgList.size();  
}

BOOL CLuaPool::CallLua(lua_State* L, LPCTSTR lpszFileName)
{
	CStringA strFileNameA(lpszFileName);
	lua_getglobal(L, strFileNameA);
	if (lua_isfunction(L, -1) )
	{
		if (0 == lua_pcall(L, Push(L), LUA_MULTRET, 0))
			return FALSE;

		fprintf(stderr, "%s:%d call function failed:%s/n", __FILE__, __LINE__, luaL_checkstring(L, -1));
	}

	return FALSE;
}