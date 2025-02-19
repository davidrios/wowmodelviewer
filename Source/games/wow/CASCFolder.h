/*
 * CASCFolder.h
 *
 *  Created on: 22 oct. 2014
 *      Author: Jeromnimo
 */

#pragma once

#include <vector>

typedef void* HANDLE;

#include "GameFolder.h" // GameConfig

#ifdef _WIN32
#    ifdef BUILDING_WOW_DLL
#        define _CASCFOLDER_API_ __declspec(dllexport)
#    else
#        define _CASCFOLDER_API_ __declspec(dllimport)
#    endif
#else
#    define _CASCFOLDER_API_
#endif

class _CASCFOLDER_API_ CASCFolder
{
public:
	CASCFolder();

	void init(const QString& path);

	QString locale() { return m_currentConfig.locale; }
	QString version() { return m_currentConfig.version; }

	std::vector<core::GameConfig> configsFound() { return m_configs; }
	bool setConfig(core::GameConfig config);

	int lastError() { return m_openError; }

	bool fileExists(int id);

	bool openFile(int id, HANDLE* result);
	bool closeFile(HANDLE file);

	// int fileDataId(std::string & filename);

private:
	CASCFolder(const CASCFolder&);

	//void initLocales();
	//void initVersion();
	void initBuildInfo();
	void addExtraEncryptionKeys();

	int m_currentCascLocale;
	core::GameConfig m_currentConfig;

	QString m_folder;
	int m_openError;
	HANDLE hStorage;

	std::vector<core::GameConfig> m_configs;
};
