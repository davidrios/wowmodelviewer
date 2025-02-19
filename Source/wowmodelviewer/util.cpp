#include "util.h"
#ifdef _WINDOWS
#include <windows.h>
#include <wx/msw/winundef.h>
#endif
#include <wx/bitmap.h>
#include <wx/choicdlg.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>
#include <wx/mstream.h>
#include "UserSkins.h"

wxString gamePath;
wxString cfgPath;
wxString bgImagePath;
wxString armoryPath;
wxString customDirectoryPath;
int customFilesConflictPolicy = 0;
int displayItemAndNPCId = 0;
UserSkins userSkins;
UserSkins& gUserSkins = userSkins;
bool useRandomLooks = true;
long langID = -1;
wxString langName;
long langOffset = -1;
long interfaceID = 0;
int ssCounter = 100; // ScreenShot Counter
int imgFormat = 0;

wxString locales[] = {
	wxT("enUS"), wxT("koKR"), wxT("frFR"), wxT("deDE"), wxT("zhCN"), wxT("zhTW"), wxT("esES"), wxT("esMX"), wxT("ruRU")
};

// Round a float, down to the specified decimal
float round(float input, int limit = 2)
{
	if (limit > 0)
	{
		input *= (10 ^ limit);
	}
	input = static_cast<int>(input + 0.5);
	if (limit > 0)
	{
		input /= (10 ^ limit);
	}
	return input;
}

wxString getGamePath(bool noSet)
{
#ifdef _WINDOWS
	HKEY key;
	unsigned long t, s;
	char path[1024];
	memset(path, 0, sizeof(path));

	wxArrayString sNames;
	wxString folder, newPath;

	// if it failed, look for World of Warcraft install
	const wxString regpaths[] =
	{
		// _WIN64
		wxT("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft"),
		wxT("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft\\PTR"),
		wxT("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft\\Beta"),
		//_WIN32, but for compatible
		wxT("SOFTWARE\\Blizzard Entertainment\\World of Warcraft"),
		wxT("SOFTWARE\\Blizzard Entertainment\\World of Warcraft\\PTR"),
		wxT("SOFTWARE\\Blizzard Entertainment\\World of Warcraft\\Beta"),
	};

	for (const auto& regpath : regpaths)
	{
		long l = RegOpenKeyEx((HKEY)HKEY_LOCAL_MACHINE, regpath, 0, KEY_QUERY_VALUE, &key);

		if (l == ERROR_SUCCESS)
		{
			s = sizeof(path);
			l = RegQueryValueEx(key, wxT("InstallPath"), nullptr, &t, reinterpret_cast<LPBYTE>(path), &s);
			wxString spath = QString::fromLatin1(path).toStdWString();
			if (l == ERROR_SUCCESS && wxDir::Exists(spath) && sNames.Index(spath) == wxNOT_FOUND)
				sNames.Add(spath);
			RegCloseKey(key);
		}
	}

	if (sNames.size())
	{
		sNames.Add(L"Other");
		folder = wxGetSingleChoice(wxT("Please select a Path:"), wxT("Path"), sNames);
	}
	// If we found an install then set the game path, otherwise just set to C:\ for now

	if (folder == wxEmptyString || folder == wxString(L"Other"))
	{
		folder = gamePath;
		if (folder == wxEmptyString)
			folder = wxT("C:\\Program Files\\World of Warcraft\\");
		newPath = wxDirSelector(wxT("Please select your World of Warcraft folder:"), folder);
		if (newPath.IsEmpty()) // user probably hit cancel
			return newPath;
	}
	else
		newPath = folder;
	if (!newPath.IsEmpty() && newPath.Last() != SLASH)
		newPath.Append(SLASH);
	if (!newPath.EndsWith(wxString(L"Data\\")))
		newPath.Append(wxString(L"Data\\"));
#elif _MAC // Mac OS X
  newPath = wxT("/Applications/World of Warcraft/");
  if (!wxFileExists(gamePath+wxT("Data/common.MPQ")) && !wxFileExists(gamePath+wxT("Data/art.MPQ")) )
    newPath = wxDirSelector(wxT("Please select your World of Warcraft folder:"), newPath);
  if (!newPath.IsEmpty() && newPath.Last() != SLASH)
    newPath.Append(SLASH);
  newPath.Append(wxT("Data/"));
#else // Linux
  newPath = wxT(".")+SLASH;
  if (!wxFileExists(newPath+wxT("Wow.exe")))
    newPath = wxDirSelector(wxT("Please select your World of Warcraft folder:"), newPath);
  if (!newPath.IsEmpty() && newPath.Last() != SLASH)
    newPath.Append(SLASH);
  newPath.Append(wxT("Data/"));
#endif
	if (!noSet)
		gamePath = newPath;
	return newPath;
}

#ifdef _WINDOWS
wxBitmap* createBitmapFromResource(const wxString& t_name, long type /* = wxBITMAP_TYPE_PNG */, int width /* = 0 */,
                                   int height /* = 0 */)
{
	wxBitmap* r_bitmapPtr = nullptr;

	char* a_data = nullptr;
	DWORD a_dataSize = 0;

	if (loadDataFromResource(a_data, a_dataSize, t_name))
	{
		r_bitmapPtr = getBitmapFromMemory(a_data, a_dataSize, type, width, height);
	}

	return r_bitmapPtr;
}

bool loadDataFromResource(char*& t_data, DWORD& t_dataSize, const wxString& t_name)
{
	bool r_result = false;

	const HRSRC a_resource = FindResource(nullptr, t_name.c_str(), RT_RCDATA);

	if (nullptr != a_resource)
	{
		const HGLOBAL a_resHandle = LoadResource(nullptr, a_resource);
		if (nullptr != a_resHandle)
		{
			t_data = static_cast<char*>(LockResource(a_resHandle));
			t_dataSize = SizeofResource(nullptr, a_resource);
			r_result = true;
		}
	}

	return r_result;
}
#endif

wxBitmap* getBitmapFromMemory(const char* t_data, const DWORD t_size, long type, int width, int height)
{
	wxMemoryInputStream a_is(t_data, t_size);

	wxImage newImage(wxImage(a_is, type, -1));

	if ((width != 0) && (height != 0))
		newImage.Rescale(width, height);
	return new wxBitmap(newImage, -1);
}

bool correctType(ssize_t type, ssize_t slot)
{
	if (type == IT_ALL)
		return true;

	switch (slot)
	{
	case CS_HEAD: return (type == IT_HEAD);
	case CS_SHOULDER: return (type == IT_SHOULDER);
	case CS_SHIRT: return (type == IT_SHIRT);
	case CS_CHEST: return (type == IT_CHEST || type == IT_ROBE);
	case CS_BELT: return (type == IT_BELT);
	case CS_PANTS: return (type == IT_PANTS);
	case CS_BOOTS: return (type == IT_BOOTS);
	case CS_BRACERS: return (type == IT_BRACERS);
	case CS_GLOVES: return (type == IT_GLOVES);

	// Slight correction.  Type 21 = Lefthand weapon, Type 22 = Righthand weapon
	//case CS_HAND_RIGHT:  return (type == IT_1HANDED || type == IT_GUN || type == IT_THROWN || type == IT_2HANDED || type == IT_CLAW || type == IT_DAGGER);
	//case CS_HAND_LEFT:  return (type == IT_1HANDED || type == IT_BOW || type == IT_SHIELD || type == IT_2HANDED || type == IT_CLAW || type == IT_DAGGER || type == IT_OFFHAND);
	case CS_HAND_RIGHT: return (type == IT_RIGHTHANDED || type == IT_GUN || type == IT_THROWN || type == IT_2HANDED ||
			type == IT_DAGGER);
	case CS_HAND_LEFT: return (type == IT_LEFTHANDED || type == IT_BOW || type == IT_SHIELD || type == IT_2HANDED ||
			type == IT_DAGGER || type == IT_OFFHAND);
	case CS_CAPE: return (type == IT_CAPE);
	case CS_TABARD: return (type == IT_TABARD);
	case CS_QUIVER: return (type == IT_QUIVER);
	default: ;
	}
	return false;
}
