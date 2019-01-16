!define BindControl `!insertmacro _FindControl`

!macro _FindControl _CtrlName _FuncName
	nsTBCIASkinEngine::FindControl /NOUNLOAD `${_CtrlName}`
	Pop $0
	${if} $0 == 0
		GetFunctionAddress $0 `${_FuncName}`
		nsTBCIASkinEngine::OnControlBindNSISScript /NOUNLOAD `${_CtrlName}` $0
	${endif}
!macroend