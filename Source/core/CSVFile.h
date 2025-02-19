#pragma once

#include "dbfile.h"

#ifdef _WIN32
#    ifdef BUILDING_CORE_DLL
#        define _CSVFILE_API_ __declspec(dllexport)
#    else
#        define _CSVFILE_API_ __declspec(dllimport)
#    endif
#else
#    define _CSVFILE_API_
#endif

class _CSVFILE_API_ CSVFile : public DBFile
{
public:
	explicit CSVFile(QString file);
	~CSVFile();

	bool open();

	bool close();

	std::vector<std::string> get(unsigned int recordIndex, const core::TableStructure* structure) const;

private:
	QString m_file;
	std::vector<QString> m_fields;
	std::vector<std::vector<std::string>> m_values;
};
