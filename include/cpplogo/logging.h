/***********************************************************
* logging.h
* Contains a wrapper around simple boost log functionality, 
* and functions to allow users to define how the logging 
* works.
***********************************************************/
#pragma once

namespace cpplogo {

//NOTE: Make sure the log string array in logging.cc is up to date
//any time this enum is changed.
enum log_severity_level
{
  trace,
  debug,
  output,
  error
};

void init_logging(log_severity_level l);

}

#ifdef _CPPLOGO_ENABLE_LOGGING
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

// This definition has to go after the severity struct
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(
  log, 
  boost::log::sources::severity_logger<cpplogo::log_severity_level>
)
#define LOG(x) BOOST_LOG_SEV(log::get(), x)

#else

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/null.hpp>

namespace cpplogo {
extern boost::iostreams::stream<boost::iostreams::null_sink> null_stream;
}

#define LOG(x) cpplogo::null_stream

#endif
