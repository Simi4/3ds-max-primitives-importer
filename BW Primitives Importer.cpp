#include "BW Primitives Importer.h"
#include "utils/bw_model.h"


#define BW_Primitives_Importer_CLASS_ID Class_ID(0x6b959364, 0xa63b11c7)


class BW_Primitives_Importer : public SceneImport {
public:
    static HWND hParams;

    int           ExtCount();
    const TCHAR * Ext(int n);
    const TCHAR * LongDesc();
    const TCHAR * ShortDesc();
    const TCHAR * AuthorName();
    const TCHAR * CopyrightMessage();
    const TCHAR * OtherMessage1();
    const TCHAR * OtherMessage2();
    unsigned int  Version();
    void          ShowAbout(HWND hWnd);
    int           DoImport(const TCHAR *name,ImpInterface *i,Interface *gi, BOOL suppressPrompts=FALSE);

    BW_Primitives_Importer();
    ~BW_Primitives_Importer();
};


class BW_Primitives_ImporterClassDesc : public ClassDesc2 {
public:
    int IsPublic() override {
        return TRUE;
    }

    void* Create(BOOL /*loading = FALSE*/) override {
        return new BW_Primitives_Importer();
    }

    const TCHAR * ClassName() override {
        return GetString(IDS_CLASS_NAME);
    }

    SClass_ID SuperClassID() override {
        return SCENE_IMPORT_CLASS_ID;
    }

    Class_ID ClassID() override {
        return BW_Primitives_Importer_CLASS_ID;
    }

    const TCHAR* Category() override {
        return GetString(IDS_CATEGORY);
    }

    const TCHAR* InternalName() override {
        return _T("BW_Primitives_Importer");
    }

    HINSTANCE HInstance() override {
        return hInstance;
    }
};


ClassDesc2* GetBW_Primitives_ImporterDesc() {
    static BW_Primitives_ImporterClassDesc BW_Primitives_ImporterDesc;
    return &BW_Primitives_ImporterDesc;
}


INT_PTR CALLBACK BW_Primitives_ImporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
    static BW_Primitives_Importer *imp = NULL;

    switch(message) {
        case WM_INITDIALOG:
            imp = (BW_Primitives_Importer *)lParam;
            CenterWindow(hWnd,GetParent(hWnd));
            return TRUE;

        case WM_CLOSE:
            EndDialog(hWnd, 0);
            return 1;
    }
    return 0;
}


BW_Primitives_Importer::BW_Primitives_Importer()
{ }

BW_Primitives_Importer::~BW_Primitives_Importer() 
{ }

int BW_Primitives_Importer::ExtCount()
{
    return 2;
}

const TCHAR *BW_Primitives_Importer::Ext(int n)
{
    switch (n) {
        case 0:
            return _T("PRIMITIVES");
        case 1:
            return _T("PRIMITIVES_PROCESSED");
    };
    return _T("");
}

const TCHAR *BW_Primitives_Importer::LongDesc()
{
    return _T("BW Primitives (WoT, WoWS, WoWP)");
}

const TCHAR *BW_Primitives_Importer::ShortDesc()
{
    return _T("BW Primitives (WoT, WoWS, WoWP)");
}

const TCHAR *BW_Primitives_Importer::AuthorName()
{
    return _T("Simi4");
}

const TCHAR *BW_Primitives_Importer::CopyrightMessage() 
{
    return _T("Copyight (c) 2019 by Simi4");
}

const TCHAR *BW_Primitives_Importer::OtherMessage1() 
{
    return _T("");
}

const TCHAR *BW_Primitives_Importer::OtherMessage2()
{
    return _T("");
}

unsigned int BW_Primitives_Importer::Version()
{
    return 100;
}

void BW_Primitives_Importer::ShowAbout(HWND hWnd)
{ }

int BW_Primitives_Importer::DoImport(const TCHAR *filename, ImpInterface *i,
                                     Interface *gi, BOOL suppressPrompts)
{
    BWModel model;

    if (model.load(filename, i)) {
        return FALSE;
    }

    /*
    if (!suppressPrompts) {
        DialogBoxParam(hInstance,
            MAKEINTRESOURCE(IDD_PANEL),
            GetActiveWindow(),
            BW_Primitives_ImporterOptionsDlgProc, (LPARAM)this);
    }
    */

    return TRUE;
}
