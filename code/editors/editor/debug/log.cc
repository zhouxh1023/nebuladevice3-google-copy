//------------------------------------------------------------------------------
//  log.cc
//  (C) 2011 xoyojank
//------------------------------------------------------------------------------
#include "stdeditor.h"
#include "editor/debug/log.h"
#include "timing/calendartime.h"
#include "io/uri.h"
#include "core/coreserver.h"
#include "editor/util/typeconvert.h"
using namespace System::Diagnostics;

//------------------------------------------------------------------------------
namespace Editor
{
//------------------------------------------------------------------------------
namespace Debug
{
using namespace Timing;

//------------------------------------------------------------------------------
bool
Log::OnAttach()
{
	Service::OnAttach();

	// build a file name for the log file
	::Util::String calString = CalendarTime::Format("{YEAR}_{MONTH}_{DAY}_{HOUR}_{MINUTE}_{SECOND}", CalendarTime::GetLocalTime());
	::Util::String fileName;
	fileName.Format("bin:logfiles/%s_%s.log", Core::CoreServer::Instance()->GetAppName().Value(), calString.AsCharPtr());
	fileName = IO::URI(fileName).GetHostAndLocalPath();
	System::String^ logPath = Util::TypeConvert::ToManagedString(fileName);
	System::IO::FileStream^ traceLog = gcnew System::IO::FileStream(logPath, System::IO::FileMode::OpenOrCreate);
	if (System::IO::File::Exists(logPath))
	{
		this->traceListener = gcnew TextWriterTraceListener(traceLog);
		System::Diagnostics::Trace::Listeners->Add(this->traceListener);
		System::Diagnostics::Trace::AutoFlush = true;
		System::Diagnostics::Trace::WriteLine(System::String::Format("Log Start: {0}", System::DateTime::Now));
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
void
Log::OnDetach()
{
	System::Diagnostics::Trace::WriteLine(System::String::Format("Log End: {0}", System::DateTime::Now));
	this->traceListener->Flush();
	System::Diagnostics::Trace::Listeners->Remove(this->traceListener);
	Service::OnDetach();
}

}// Debug

}// Editor