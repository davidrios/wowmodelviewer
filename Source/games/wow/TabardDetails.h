#pragma once

class GameFile;

#include <QString>
class QXmlStreamReader;
class QXmlStreamWriter;

#include <vector>

#ifdef _WIN32
#    ifdef BUILDING_WOW_DLL
#        define _TABARDDETAILS_API_ __declspec(dllexport)
#    else
#        define _TABARDDETAILS_API_ __declspec(dllimport)
#    endif
#else
#    define _TABARDDETAILS_API_
#endif

class _TABARDDETAILS_API_ TabardDetails
{
public:
	TabardDetails();

	bool showCustom;

	GameFile* GetIconTex(int slot);
	GameFile* GetBorderTex(int slot);
	GameFile* GetBackgroundTex(int slot);

	int GetMaxIcon();
	int GetMaxIconColor(int icon);
	int GetMaxBorder();
	int GetMaxBorderColor(int border);
	int GetMaxBackground();

	void save(QXmlStreamWriter&);
	void load(QXmlStreamReader&);

	int getIcon();
	int getIconColor();
	int getBorder();
	int getBorderColor();
	int getBackground();

	void setIcon(int icon);
	void setIconColor(int color);
	void setBorder(int border);
	void setBorderColor(int color);
	void setBackground(int background);

	void setTabardId(int itemid);

	void setIconId(int id);
	void setBorderId(int id);
	void setBackgroundId(int id);

private:
	static const std::vector<QString> ICON_COLOR_VECTOR;
	static const std::vector<QString> BORDER_COLOR_VECTOR;
	static const std::vector<QString> BACKGROUND_COLOR_VECTOR;

	int iconId;
	int iconColor;
	int borderId;
	int borderColor;
	int backgroundId;

	int tier;

	std::vector<int> backgrounds;
	std::vector<int> icons;
	std::vector<int> borders;
};
