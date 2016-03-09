// dllmain.h : 模块类的声明。

class CImageOleModule : public CAtlDllModuleT< CImageOleModule >
{
public :
	DECLARE_LIBID(LIBID_ImageOleLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_IMAGEOLE, "{C06BB180-B27A-4823-99A8-6BB26099A084}")
};

extern class CImageOleModule _AtlModule;
