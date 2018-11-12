#pragma once

class LUALIB_API CLua
{
public:
	CLua();
	~CLua();

public:
	void Init();
	void Close();
	BOOL LoadFile(LPCTSTR lpszFileName);
	BOOL DoFn(LPCTSTR lpszFunName, const CParamGroup& ParamIn, CParamGroup& ParamOut);
public:
	static BOOL PushLuaData(lua_State*, CParamData* pParam);
	static BOOL PopLuaData(lua_State* pState, CParamData* pParam, int nIndex);
protected:
	lua_State* m_pLua;
};