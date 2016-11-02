#include "cpplogo/logging.h"

#ifdef _CPPLOGO_ENABLE_LOGGING
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

namespace cpplogo {

std::ostream& operator<< (std::ostream& strm, log_severity_level level)
{
  static const char* strings[] = {
    "TRA",
    "DBG",
    "...",
    "ERR",
  };
  if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings)) {
    strm << strings[level];
  } else {
    strm << static_cast<int>(level);
  }
  return strm;
}

/***********************************************************
* init_logging
* Set up logging options. 
* Should be expanded to allow for user control.
***********************************************************/
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", log_severity_level)
void init_logging(log_severity_level l)
{
  log_severity_level min_level = l;
  logging::add_console_log(
    std::cout,
    keywords::format = (
      expr::stream << "[" << severity << "] " << expr::message
    ),
    keywords::filter = ( severity >= min_level )
  );
} /* init_logging() */

}

#else

namespace cpplogo {

boost::iostreams::stream<boost::iostreams::null_sink> null_stream(boost::iostreams::null_sink{});

void init_logging(log_severity_level l) {(void)l;}

}

#endif
