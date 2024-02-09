// dllmain.h : Declaration of module class.

class CATLProject1Module : public ATL::CAtlDllModuleT< CATLProject1Module >
{
public :
	DECLARE_LIBID(LIBID_ATLProject1Lib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLPROJECT1, "{c86c36c2-30f5-48cc-9f66-141b1619a3da}")
};

extern class CATLProject1Module _AtlModule;
