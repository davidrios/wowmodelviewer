#include "Game.h"

core::Game* core::Game::m_instance = nullptr;

core::Game::Game() : m_folder(nullptr), m_db(nullptr)
{
}

void core::Game::init(core::GameFolder* folder, core::GameDatabase* db)
{
	m_db = db;
	m_folder = folder;
	if (m_folder)
		m_folder->init();
}

void core::Game::addCustomFiles(const QString& path, bool bypassOriginalFiles)
{
	if (m_folder)
		m_folder->addCustomFiles(path, bypassOriginalFiles);
}
