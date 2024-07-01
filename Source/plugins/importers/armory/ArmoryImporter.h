/*----------------------------------------------------------------------*\
| This file is part of WoW Model Viewer                                  |
|                                                                        |
| WoW Model Viewer is free software: you can redistribute it and/or      |
| modify it under the terms of the GNU General Public License as         |
| published by the Free Software Foundation, either version 3 of the     |
| License, or (at your option) any later version.                        |
|                                                                        |
| WoW Model Viewer is distributed in the hope that it will be useful,    |
| but WITHOUT ANY WARRANTY; without even the implied warranty of         |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          |
| GNU General Public License for more details.                           |
|                                                                        |
| You should have received a copy of the GNU General Public License      |
| along with WoW Model Viewer.                                           |
| If not, see <http://www.gnu.org/licenses/>.                            |
\*----------------------------------------------------------------------*/

/*
 * ArmoryImporter.h
 *
 *  Created on: 9 dec. 2013
 *   Copyright: 2013 , WoW Model Viewer (http://wowmodelviewer.net)
 */

#pragma once

#include <QtPlugin>

#define _IMPORTERPLUGIN_CPP_ // to define interface
#include "ImporterPlugin.h"
#undef _IMPORTERPLUGIN_CPP_

class ArmoryImporter final : public ImporterPlugin
{
	Q_INTERFACES(ImporterPlugin)
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "wowmodelviewer.importers.WowheadImporter" FILE "armoryimporter.json")

public:
	ArmoryImporter() = default;
	~ArmoryImporter() = default;

	bool acceptURL(QString url) const override;

	NPCInfos* importNPC(QString url) const override { return nullptr; };
	CharInfos* importChar(QString url) const override;
	ItemRecord* importItem(QString url) const override;

private:
	enum ImportType
	{
		CHARACTER,
		ITEM
	};

	int readJSONValues(ImportType type, const QString& url, QJsonObject& result) const;
	QByteArray getURLData(const QString& inputUrl) const;
	static bool hasMember(const QJsonValueRef& check, const QString& lookfor);
	static bool hasTransmog(const QJsonValueRef& check);
};
