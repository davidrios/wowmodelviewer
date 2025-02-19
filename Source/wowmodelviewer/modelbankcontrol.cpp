#include "modelbankcontrol.h"
#include <wx/arrstr.h>
#include <wx/wfstream.h>
#include "Game.h"
#include "globalvars.h"
#include "logger/Logger.h"
#include "modelviewer.h"

IMPLEMENT_CLASS(ModelBankControl, wxWindow)

BEGIN_EVENT_TABLE(ModelBankControl, wxWindow)
	EVT_BUTTON(ID_MODELBANK_ADD, ModelBankControl::OnButton)
	EVT_BUTTON(ID_MODELBANK_REMOVE, ModelBankControl::OnButton)
	EVT_BUTTON(ID_MODELBANK_DISPLAY, ModelBankControl::OnButton)
END_EVENT_TABLE()

ModelBankControl::ModelBankControl(wxWindow* parent, wxWindowID id)
{
	LOG_INFO << "Creating Model Bank Control...";

	txtName = nullptr;
	lblName = nullptr;
	btnAdd = nullptr;
	btnRemove = nullptr;
	btnDisplay = nullptr;
	lstBank = nullptr;

	if (Create(parent, id, wxDefaultPosition, wxSize(270, 280), 0, wxT("ModelBankControlFrame")) == false)
	{
		LOG_ERROR << "Failed to create a window for our ModelBankControl.";
		return;
	}

	//wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")
	//wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")
	//wxButton(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "button")
	//wxTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr)
	//wxStaticText(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticText")

	lblName = new wxStaticText(this, wxID_ANY, wxT("Name:"), wxPoint(5, 15), wxDefaultSize, 0);
	txtName = new wxTextCtrl(this, ID_MODELBANK_NAME, wxEmptyString, wxPoint(40, 10), wxDefaultSize, 0,
	                         wxDefaultValidator);

	btnAdd = new wxButton(this, ID_MODELBANK_ADD, wxT("Add"), wxPoint(200, 10), wxDefaultSize);
	btnRemove = new wxButton(this, ID_MODELBANK_REMOVE, wxT("Remove"), wxPoint(200, 40), wxDefaultSize);
	btnDisplay = new wxButton(this, ID_MODELBANK_DISPLAY, wxT("Display"), wxPoint(200, 70), wxDefaultSize);

	lstBank = new wxListBox(this, wxID_ANY, wxPoint(5, 40), wxSize(190, 250), 0, nullptr, wxLB_SINGLE | wxLB_ALWAYS_SB);

	LoadList();
}

ModelBankControl::~ModelBankControl()
{
	SaveList();
	bankList.clear();
}

void ModelBankControl::OnButton(wxCommandEvent& event)
{
	const int id = event.GetId();

	if (id == ID_MODELBANK_ADD)
	{
		AddModel();
	}
	else if (id == ID_MODELBANK_REMOVE)
	{
		RemoveModel();
	}
	else if (id == ID_MODELBANK_DISPLAY)
	{
		LoadModel();
	}
}

void ModelBankControl::LoadModel()
{
	if (!g_canvas)
		return;

	const int val = lstBank->GetSelection();
	if (val == wxNOT_FOUND)
		return;

	const ModelBank cd = bankList[val];

	g_modelViewer->LoadModel(GAMEDIRECTORY.getFile(QString::fromWCharArray(cd.fileName.c_str())));

	if (g_canvas->model())
	{
		WoWModel* m = const_cast<WoWModel*>(g_canvas->model());
		m->pos_ = cd.pos;
		m->rot_ = cd.rot;
	}
	else
		return;

	if (cd.modelType == MT_CHAR && g_charControl)
	{
		g_charControl->model->cd.set(CharDetails::SKIN_COLOR, cd.skinColor);
		g_charControl->model->cd.set(CharDetails::FACE, cd.faceType);
		g_charControl->model->cd.set(CharDetails::FACIAL_CUSTOMIZATION_STYLE, cd.hairStyle);
		g_charControl->model->cd.set(CharDetails::FACIAL_CUSTOMIZATION_COLOR, cd.hairColor);
		g_charControl->model->cd.set(CharDetails::ADDITIONAL_FACIAL_CUSTOMIZATION, cd.facialHair);

		g_charControl->model->cd.showEars = cd.showEars;
		g_charControl->model->cd.showFacialHair = cd.showFacialHair;
		g_charControl->model->cd.showFeet = cd.showFeet;
		g_charControl->model->cd.showHair = cd.showHair;

		// @TODO : to repair
		//for (size_t i=0; i<NUM_CHAR_SLOTS; i++)
		//  g_charControl->model->cd.equipment[i] = cd.equipment[i];

		g_charControl->RefreshModel();
		g_charControl->RefreshEquipment();
	}
	else if (cd.modelType == MT_NORMAL)
	{
		TextureGroup grp;
		grp.count = static_cast<int>(cd.textures.size());
		grp.base = TEXTURE_GAMEOBJECT1;
		for (size_t i = 0; i < grp.count; i++)
			grp.tex[i] = cd.textures[i];

		const int Val = g_animControl->AddSkin(grp);
		g_animControl->SetSkin(Val);
	}
}

void ModelBankControl::AddModel()
{
	if (!g_canvas || !g_canvas->model())
		return;

	if (txtName->GetValue().IsEmpty() == true)
		return;

	ModelBank cd;

	cd.fileName = wxString(g_canvas->model()->name().toStdWString());
	cd.name = txtName->GetValue();

	cd.pos = g_canvas->model()->pos_;
	cd.rot = g_canvas->model()->rot_;

	cd.modelType = g_canvas->model()->modelType;

	// @TODO : to repair
	/*
	if (cd.modelType == MT_NORMAL) {
	  int val = g_animControl->skinList->GetSelection();
	  TextureGroup *grp = (TextureGroup*) g_animControl->skinList->GetClientData(val);
  
	  for (size_t i=0; i<grp->count; i++) {
	    if (g_canvas->model()->useReplaceTextures[grp->base+i]) {
	      cd.textures.push_back(grp->tex[i]);
	    }
	  }
  
	} 
	else if (cd.modelType == MT_CHAR) {
  
	  
	  //for (size_t i=0; i<NUM_CHAR_SLOTS; i++)
	  //  cd.equipment[i] = g_charControl->model->cd.equipment[i];
  
	  cd.faceType = g_charControl->model->cd.get(CharDetails::FACE);
	  cd.facialHair = g_charControl->model->cd.get(CharDetails::ADDITIONAL_FACIAL_CUSTOMIZATION);
	  cd.hairColor = g_charControl->model->cd.get(CharDetails::FACIAL_CUSTOMIZATION_COLOR);
	  cd.hairStyle = g_charControl->model->cd.get(CharDetails::FACIAL_CUSTOMIZATION_COLOR);
	  cd.skinColor = g_charControl->model->cd.get(CharDetails::SKIN_COLOR);
  
	  cd.showEars = g_charControl->model->cd.showEars;
	  cd.showFacialHair = g_charControl->model->cd.showFacialHair;
	  cd.showFeet = g_charControl->model->cd.showFeet;
	  cd.showHair = g_charControl->model->cd.showHair;
	  cd.showUnderwear = g_charControl->model->cd.showUnderwear;
  
	} else if (cd.modelType == MT_WMO) {
	
	}
	*/
	bankList.push_back(cd);

	txtName->Clear();
	UpdateList();
}

void ModelBankControl::RemoveModel()
{
	const int val = lstBank->GetSelection();
	if (val == wxNOT_FOUND)
		return;

	std::vector<ModelBank> temp;
	for (size_t i = 0; i < bankList.size(); i++)
	{
		if (static_cast<int>(i) != val)
		{
			temp.push_back(bankList[i]);
		}
	}

	bankList.clear();
	for (const auto& i : temp)
	{
		bankList.push_back(i);
	}

	UpdateList();
}

void ModelBankControl::UpdateList()
{
	lstBank->Clear();

	for (auto& i : bankList)
	{
		lstBank->Append(i.name);
	}
}

void ModelBankControl::SaveList()
{
	/*
	if (bankList.size() == 0)
	  return;
  
	wxFFileOutputStream file(wxT("modelbank.dat"), wxT("w+b"));
  
	if (!file.IsOk()) {
	  LOG_ERROR << "Was unable to save the ModelBank data to the HDD.";
	  return;
	}
  
	// File Tag
	char tag[5] = "MB02";
	file.Write(&tag, 4);
  
	size_t val = 0;
	int iVal = 0;
  
	for (size_t i=0; i<bankList.size(); i++) {
	  if (bankList[i].name.size() > 0) {
	    // name
	    val = bankList[i].name.size();
	    file.Write(&val, sizeof(val));
	    file.Write(bankList[i].name.c_str(), val);
  
	    // filename
	    val = bankList[i].fileName.size();
	    file.Write(&val, sizeof(val));
	    file.Write(bankList[i].fileName.c_str(), val);
  
	    file.Write(&bankList[i].pos, sizeof(glm::vec3));
	    file.Write(&bankList[i].rot, sizeof(glm::vec3));
  
  
	    // File Type
	    file.Write(&bankList[i].modelType, sizeof(unsigned int));
  
	    if (bankList[i].modelType == MT_NORMAL) {
  
	      // Texture count
	      iVal = (int)bankList[i].textures.Count();
	      file.Write(&iVal, sizeof(int));
  
	      for (size_t j=0; j<bankList[i].textures.Count(); j++) {
	        val = bankList[i].textures[j].size();
	        file.Write(&val, sizeof(val));
	        file.Write(bankList[i].textures[j].c_str(), val);
	      }
  
	    } else if (bankList[i].modelType == MT_CHAR) {
	      // Skin Colour
	      file.Write(&bankList[i].skinColor, sizeof(unsigned int));
	      // Face Type
	      file.Write(&bankList[i].faceType, sizeof(unsigned int));
	      // Hair Colour
	      file.Write(&bankList[i].hairColor, sizeof(unsigned int));
	      // Hair Style
	      file.Write(&bankList[i].hairStyle, sizeof(unsigned int));
	      // Facial Features style
	      file.Write(&bankList[i].facialHair, sizeof(unsigned int));
	      // Race
	      file.Write(&bankList[i].race, sizeof(unsigned int));
	      // Gender
	      file.Write(&bankList[i].gender, sizeof(unsigned int));
	      // use NPC?
	      file.Write(&bankList[i].useNPC, sizeof(unsigned int));
  
	      //bool showUnderwear, showEars, showHair, showFacialHair, showFeet;
	      file.Write(&bankList[i].showUnderwear, sizeof(bool));
	      file.Write(&bankList[i].showEars, sizeof(bool));
	      file.Write(&bankList[i].showHair, sizeof(bool));
	      file.Write(&bankList[i].showFacialHair, sizeof(bool));
	      file.Write(&bankList[i].showFeet, sizeof(bool));
  
	      for (ssize_t j=0; j<NUM_CHAR_SLOTS; j++) {
	        iVal = bankList[i].equipment[j];
	        file.Write(&iVal, sizeof(int));
	      }
	    }
	  }
	}
  
	file.Close();
	*/
}

void ModelBankControl::LoadList()
{
	/*
	if (!wxFile::Exists(wxT("modelbank.dat")))
	  return;
  
	wxFFileInputStream file(wxT("modelbank.dat"), wxT("rb"));
  
	if (!file.IsOk())
	  return;
  
	if (file.GetSize() < 4)
	  return;
  
	bankList.clear();
  
	size_t val = 0;
	char name[512];
	char filename[1024];
	char textures[1024];
	char tag[5] = {0,0,0,0,0};
  
	file.Read(&tag, 4);
	  if(strncmp(tag, "MB", 2)!=0)
	      return;
  
	long fileID = 0;
	wxString(tag, wxConvUTF8).SubString(2,4).ToLong(&fileID);
  
	while (!file.Eof()) {
	  ModelBank cd;
  
	  // name
	  file.Read(&val, sizeof(size_t));
	  file.Read(name, val);
	  cd.name = wxString(name, wxConvUTF8).SubString(0, val-1);
  
	  // filename
	  file.Read(&val, sizeof(size_t));
	  file.Read(filename, val);
	  cd.fileName = wxString(filename, wxConvUTF8).SubString(0, val-1);
	  
	  // Position and rotation only started being saved in "MB02" model banks.
	  // This is for compatability.
	  if (fileID > 1) {
	    file.Read(&cd.pos, sizeof(glm::vec3));
	    file.Read(&cd.rot, sizeof(glm::vec3));
	  } else {
	    cd.pos = glm::vec3(0,0,0);
	    cd.rot = glm::vec3(0,0,0);
	  }
  
	  // model type
	  file.Read(&cd.modelType, sizeof(unsigned int));
  
	  if (cd.modelType == MT_NORMAL) {
  
	    // Texture count
	    int texCount = 0;
	    file.Read(&texCount, sizeof(int));
  
	    for (ssize_t j=0; j<texCount; j++) {
	      // filename
	      file.Read(&val, sizeof(size_t));
	      file.Read(textures, val);
	      cd.textures.Add(wxString(textures, wxConvUTF8).SubString(0, val-1));
	    }
  
	  } else if (cd.modelType == MT_CHAR) {
	    // Skin Colour
	    file.Read(&cd.skinColor, sizeof(unsigned int));
	    // Face Type
	    file.Read(&cd.faceType, sizeof(unsigned int));
	    // Hair Colour
	    file.Read(&cd.hairColor, sizeof(unsigned int));
	    // Hair Style
	    file.Read(&cd.hairStyle, sizeof(unsigned int));
	    // Facial Features style
	    file.Read(&cd.facialHair, sizeof(unsigned int));
	    // Race
	    file.Read(&cd.race, sizeof(unsigned int));
	    // Gender
	    file.Read(&cd.gender, sizeof(unsigned int));
	    // use NPC?
	    file.Read(&cd.useNPC, sizeof(unsigned int));
  
	    //bool showUnderwear, showEars, showHair, showFacialHair, showFeet;
	    file.Read(&cd.showUnderwear, sizeof(bool));
	    file.Read(&cd.showEars, sizeof(bool));
	    file.Read(&cd.showHair, sizeof(bool));
	    file.Read(&cd.showFacialHair, sizeof(bool));
	    file.Read(&cd.showFeet, sizeof(bool));
  
	    for (ssize_t j=0; j<NUM_CHAR_SLOTS; j++) {
	      file.Read(&cd.equipment[j], sizeof(unsigned int));
	    }
	  }
  
	  bankList.push_back(cd);
	}
  
	UpdateList();
	*/
}
