#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#ifndef _WINDOWS
  #include "../../bin_support/Icons/wmv.xpm"
#endif

#include "modelviewer.h"

static wxString langNames[] =
{
	wxT("English"),
	wxT("Korean"),
	wxT("French"),
	wxT("German"),
	wxT("Simplified Chinese"),
	wxT("Traditional Chinese"),
	wxT("Spanish (EU)"),
	wxT("Spanish (Latin American)"),
	wxT("Russian"),
};

static const wxLanguage langIds[] =
{
	wxLANGUAGE_ENGLISH,
	wxLANGUAGE_KOREAN,
	wxLANGUAGE_FRENCH,
	wxLANGUAGE_GERMAN,
	wxLANGUAGE_CHINESE_SIMPLIFIED,
	wxLANGUAGE_CHINESE_TRADITIONAL,
	wxLANGUAGE_SPANISH,
	wxLANGUAGE_SPANISH,
	wxLANGUAGE_RUSSIAN,
};

class WowModelViewApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
	virtual void OnUnhandledException();
	virtual void OnFatalException();
	void setInterfaceLocale();

	//virtual bool OnExceptionInMainLoop();
	//virtual void HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const ; 

	void LoadSettings();
	void SaveSettings();

	ModelViewer* frame;

	wxLocale locale;
};

void searchMPQs(bool firstTime);
