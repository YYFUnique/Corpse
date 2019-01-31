!define FindProcess `!insertmacro nsProcess::FindProcess`

!macro nsProcess::FindProcess _FILE _ERR
	nsProcess::_FindProcess `${_FILE}`
	Pop ${_ERR}
!macroend


!define KillProcess `!insertmacro nsProcess::KillProcess`

!macro nsProcess::KillProcess _FILE _ERR
	nsProcess::_KillProcess `${_FILE}`
	Pop ${_ERR}
!macroend

!define nsProcess::CloseProcess `!insertmacro nsProcess::CloseProcess`

!macro nsProcess::CloseProcess _FILE _ERR
	nsProcess::_CloseProcess `${_FILE}`
	Pop ${_ERR}
!macroend


!define nsProcess::Unload `!insertmacro nsProcess::Unload`

!macro nsProcess::Unload
	nsProcess::_Unload
!macroend
