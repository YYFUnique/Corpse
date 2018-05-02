!define BindControl `!insertmacro nsTBCIASkinEngine::_FindControl`

!macro nsTBCIASkinEngine::_FindControl _CtrlName _FuncName
	nsTBCIASkinEngine::FindControl `${_CtrlName}`
	Pop $0
	${if} $0 == 0
		GetFunctionAddress $0 `${_FuncName}`
		nsTBCIASkinEngine::OnControlBindNSISScript `${_CtrlName}` $0
	${endif}
!macroend