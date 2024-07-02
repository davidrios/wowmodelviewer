#include "LogStackWalker.h"
#include "logger/Logger.h"

void LogStackWalker::OnStackFrame(const wxStackFrame& frame)
{
	int level = frame.GetLevel();
	QString func = QString::fromWCharArray(frame.GetName().c_str());
	QString filename = QString::fromWCharArray(frame.GetFileName().c_str());
	int line = frame.GetLine();

	LOG_ERROR << level << func << "(" << filename << "-" << line << ")";
}
