#include "StdAfx.h"
#include "Lua.h"

Lualib::Lualib()
{
	 m_pLua = luaL_newstate();
}

Lualib::~Lualib()
{
	if (m_pLua != NULL)
	{
		lua_close(m_pLua);
		m_pLua = NULL;
	}
}