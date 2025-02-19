#pragma once

#include <wx/dialog.h>

class wxButton;
class wxStaticText;
class wxTextCtrl;

#include "database.h"

class ItemImporterDialog : public wxDialog
{
public:
	ItemImporterDialog(wxWindow* parent = nullptr, wxWindowID id = -1, const wxString& title = _T("Import Item from URL"),
	                   const wxPoint& position = wxDefaultPosition, const wxSize& size = wxSize(300, 300));

	ItemRecord& getImportedItem();

private:
	wxTextCtrl* m_URLname;
	wxButton* m_importBtn;
	wxButton* m_displayBtn;
	wxStaticText* m_nameResult;
	wxStaticText* m_typeResult;
	wxStaticText* m_idResult;
	wxStaticText* m_displayIdResult;

	ItemRecord* m_importedItem;

	static const int ID_BTN_IMPORT;

	void OnImportButtonClicked(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();
};
