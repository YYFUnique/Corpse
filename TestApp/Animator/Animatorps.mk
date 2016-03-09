
Animatorps.dll: dlldata.obj Animator_p.obj Animator_i.obj
	link /dll /out:Animatorps.dll /def:Animatorps.def /entry:DllMain dlldata.obj Animator_p.obj Animator_i.obj \
		kernel32.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del Animatorps.dll
	@del Animatorps.lib
	@del Animatorps.exp
	@del dlldata.obj
	@del Animator_p.obj
	@del Animator_i.obj
