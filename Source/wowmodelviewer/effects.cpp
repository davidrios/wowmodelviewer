#include "effects.h"
#include "Attachment.h"
#include "enums.h"
#include "Game.h"
#include "itemselection.h"

BEGIN_EVENT_TABLE(EnchantsDialog, wxDialog)
	EVT_BUTTON(ID_ENCHANTSOK, EnchantsDialog::OnClick)
	EVT_BUTTON(ID_ENCHANTSCANCEL, EnchantsDialog::OnClick)
END_EVENT_TABLE()

EnchantsDialog::EnchantsDialog(wxWindow* parent, CharControl* cc)
{
	charControl = cc;
	Initiated = false;
	EnchantsInitiated = false;

	LHandEnchant = -1;
	RHandEnchant = -1;

	slot = nullptr;
	effectsListbox = nullptr;

	Create(parent, -1, wxT("Weapon Enchants"), wxDefaultPosition, wxSize(200, 350), wxDEFAULT_DIALOG_STYLE);
	wxDialog::Show(false);
}

void EnchantsDialog::OnClick(wxCommandEvent& event)
{
	if (event.GetId() == ID_ENCHANTSOK)
	{
		const std::wstring sel = effectsListbox->GetStringSelection().c_str();

		if (sel == L"")
		{
			Show(false);
			return;
		}

		if (sel == wxT("NONE") || sel == wxT("None"))
		{
			if (slot->GetSelection() == 0)
				RHandEnchant = -1;
			else
				LHandEnchant = -1;
			Show(false);
			return;
		}

		for (auto& it : enchants)
		{
			if (it.second.name == sel)
			{
				const EnchantsRec enchant = it.second;
				int s = slot->GetSelection();
				s += 10;

				if (!charControl->model)
					return;

				if (!slotHasModel(s))
					return;

				if (slot->GetSelection() == 0)
					RHandEnchant = it.first;
				else
					LHandEnchant = it.first;

				// children:
				for (const auto& i : charControl->charAtt->children)
				{
					if (i->slot == s)
					{
						Attachment* att = i;
						if (att->children.size() > 0)
							att->delChildren();

						const WoWModel* m = static_cast<WoWModel*>(att->model());
						if (!m)
							return;

						for (ssize_t k = 0; k < 5; k++)
						{
							if ((enchant.models[k] != "") && (m->attLookup[k] >= 0))
							{
								att->addChild(enchant.models[k].c_str(), k, -1);
							}
						}
						break;
					}
				}

				Show(false);
				return;
			}
		}
	}
	else if (event.GetId() == ID_ENCHANTSCANCEL)
	{
		this->Show(false);
	}
}

void EnchantsDialog::InitObjects()
{
	const wxString handSlots[2] = {wxT("Right Hand"), wxT("Left Hand")};

	slot = new wxRadioBox(this, -1, wxT("Apply effects to:"), wxPoint(10, 10), wxSize(180, 80), 2, handSlots, 4,
	                      wxRA_SPECIFY_ROWS, wxDefaultValidator, wxT("radioBox"));

	text1 = new wxStaticText(this, -1, wxT("Enchantments:"), wxPoint(10, 110), wxDefaultSize);
	effectsListbox = new wxListBox(this, -1, wxPoint(10, 130), wxSize(180, 160), choices, wxLB_SINGLE);

	btnOK = new wxButton(this, ID_ENCHANTSOK, wxT("OK"), wxPoint(90, 295), wxSize(50, 22));
	btnCancel = new wxButton(this, ID_ENCHANTSCANCEL, wxT("Cancel"), wxPoint(140, 295), wxSize(50, 22));

	Initiated = true;
}

void EnchantsDialog::InitEnchants()
{
	const QString query =
		"SELECT SpellItemEnchantment.ID, Name, IVE1.path AS enchant1, IVE2.path AS enchant2, IVE3.path AS enchant3, IVE4.path AS enchant4, IVE5.path AS enchant5 \
                   FROM SpellItemEnchantment \
                   LEFT JOIN ItemVisuals ON VisualID = ItemVisuals .ID \
                   LEFT JOIN ItemVisualEffects IVE1 ON ItemVisuals .itemVisualEffects1 = IVE1.ID \
                   LEFT JOIN ItemVisualEffects IVE2 ON ItemVisuals .itemVisualEffects1 = IVE2.ID \
                   LEFT JOIN ItemVisualEffects IVE3 ON ItemVisuals .itemVisualEffects3 = IVE3.ID \
                   LEFT JOIN ItemVisualEffects IVE4 ON ItemVisuals .itemVisualEffects4 = IVE4.ID \
                   LEFT JOIN ItemVisualEffects IVE5 ON ItemVisuals .itemVisualEffects5 = IVE5.ID \
                   WHERE VisualID != 0";

	const sqlResult enchantsInfos = GAMEDATABASE.sqlQuery(query);

	if (!enchantsInfos.valid || enchantsInfos.values.empty())
		return;

	for (auto& value : enchantsInfos.values)
	{
		EnchantsRec rec;
		rec.name = wxConvLocal.cWC2WX(
			wxConvUTF8.cMB2WC(wxString(value[1].toStdString().c_str(), wxConvUTF8).mb_str()));;
		rec.models[0] = value[2].toStdString();
		rec.models[1] = value[3].toStdString();
		rec.models[2] = value[4].toStdString();
		rec.models[3] = value[5].toStdString();
		rec.models[4] = value[6].toStdString();
		enchants[value[0].toInt()] = rec;
	}

	choices.Clear();
	for (auto& enchant : enchants)
		choices.Add(enchant.second.name.c_str());

	EnchantsInitiated = true;
}

void EnchantsDialog::Display()
{
	if (!EnchantsInitiated)
		InitEnchants();

	if (!Initiated)
		InitObjects();

	if (Initiated)
	{
		Center();
		Show(true);
	}
}
