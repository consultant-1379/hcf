/*
 * date_time_parser.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: xgiufer
 */
#include <iostream>

#include "hcf/trace/com_ericsson_common_hcf_srv_datetimeparser.h"
#include "hcf/trace/tracing_macros.h"

#include "date_time_parser.h"

using namespace std;

#undef __CLASS_NAME__
#define __CLASS_NAME__ date_time_formatter

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __CLASS_NAME__::get_date_time_format_to_scan(std::string & output_format, date::position::date_position_t (& order)[3], std::string & separator) {
	HCF_CC_TRACE_FUNCTION;
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
	std::string format_s("");
	separator = "";
	int first__ = 0, second__ = 0, date__= 0;

  if (_fmt_str.empty()) {
  	HCF_CC_TRACE(GetDateTimeFormatToScan, "Empty format string provided: returning error!", "");
    return -1;
  }

  const char * fmt_c = _fmt_str.c_str();
	HCF_CC_TRACE(GetDateTimeFormatToScan, "Format string provided: starting to loop over it!", fmt_c);

  for (;;)
  {
  	unsigned char c = *fmt_c;

  	if (!c)	break;
  	fmt_c++;

  	switch(c) {
  	case 'Y':
  		year++;
  		break;
  	case 'M':
  		month++;
  		break;
  	case 'D':
  		day++;
  		break;
  	case 'h':
  		hour++;
  		break;
  	case 'm':
  		minute++;
  		break;
  	case 's':
  		second++;
  		break;
  	default:
  		if(is_special_character(c) != -1) {
  			format_s.append(1,c);
  			if(date__ == 3)// date == 3 full date parsed
  				separator.append(1,c);
  		}
  	}

  	if(hour || minute || second)
  		date__= 0;

  	//check next
  	unsigned char n = *fmt_c;
  	if (n++ == c) {
  		HCF_CC_TRACE(GetDateTimeFormatToScan, "Next char is the same! Nothing to do!", fmt_c);
  		continue;
  	}

  	//what i've found? - I need to reset after verification
  	if(year) {
  		date__++;
  		format_s.append(get_year_format(year)); year = 0;
  		(!first__) ? first__=1, order[date::position::_YEAR] = date::position::FIRST_POS :
  				(!second__) ? second__=1, order[date::position::_YEAR] = date::position::SECOND_POS :
  						order[date::position::_YEAR] = date::position::THIRD_POS;
  	}
  	else if(month) {
  		date__++;
  		format_s.append(get_month_format(month)); month = 0;
  		(!first__) ? first__=1, order[date::position::_MONTH] = date::position::FIRST_POS :
  				(!second__) ? second__=1, order[date::position::_MONTH] = date::position::SECOND_POS :
  						order[date::position::_MONTH] = date::position::THIRD_POS;
  	}
  	else if(day) {
  		date__++;
  		format_s.append(DD); day=0;
  		(!first__) ? first__=1, order[date::position::_DAY] = date::position::FIRST_POS :
  				(!second__) ? second__=1, order[date::position::_DAY] = date::position::SECOND_POS :
  						order[date::position::_DAY] = date::position::THIRD_POS;
  	}
  	//i'll ever consider the time order as hour minute second
  	else if(hour) {format_s.append(hh); hour=0;}
  	else if(minute) {format_s.append(mm); minute=0;}
  	else if(second) {format_s.append(ss); second=0;}
  }

  HCF_CC_TRACE(GetDateTimeFormatToScan, "Output format string correctly created!", format_s.c_str());
  output_format = format_s;
  return 0;
}

#undef __CLASS_NAME__
#define __CLASS_NAME__ date_time_parser

__CLASS_NAME__::__CLASS_NAME__ (): _sfmt(), _date(), _formatter(), _date_time() {
	HCF_CC_TRACE_FUNCTION;
  _date_position[date::position::_YEAR] = date::position::DEFAULT_POS;
  _date_position[date::position::_MONTH] = date::position::DEFAULT_POS;
  _date_position[date::position::_DAY] = date::position::DEFAULT_POS;
}

int __CLASS_NAME__::get_format_position (const date::position::date_position_t (& pos)[3]) {
	HCF_CC_TRACE_FUNCTION;
  if (pos[date::position::_YEAR] != pos[date::position::_MONTH] && pos[date::position::_MONTH]!= pos[date::position::_DAY])
    return ((pos[date::position::_YEAR] << 6) + (pos[date::position::_MONTH] << 3) + (pos[date::position::_DAY]));

  return DATE_FORMAT_UNKNOWN;
}

int __CLASS_NAME__::set_date_time_parsed (const int position_fmt, const std::string & parsed_fmt_to_scan, const std::string & input_date) {
  // TODO - INSERT OTHER DATE FORMAT WITH STRING
	HCF_CC_TRACE_FUNCTION;

  //insert date time format name
  unsigned int year = 1970, mon = 1, day = 0, hour = 0, min = 0, sec = 0;

  HCF_CC_LOG(LEVEL_INFO, "Received - format:%03X - fmt_to_scan:%s - input_date:%s", position_fmt, parsed_fmt_to_scan.c_str(), input_date.c_str());
  HCF_CC_TRACE(SetDateTimeParsed, "Received a request to parse and store the following date!",
  		position_fmt, parsed_fmt_to_scan.c_str(), input_date.c_str());

  switch(position_fmt){
    case DATE_FORMAT_YMD:
      sscanf(input_date.c_str(),parsed_fmt_to_scan.c_str(), &year, &mon, &day, &hour, &min, &sec);
      break;

    case DATE_FORMAT_YDM:
      sscanf(input_date.c_str(),parsed_fmt_to_scan.c_str(), &year, &day, &mon, &hour, &min, &sec);
      break;

    case DATE_FORMAT_MYD:
      sscanf(input_date.c_str(),parsed_fmt_to_scan.c_str(), &mon, &year, &day, &hour, &min, &sec);
      break;

    case DATE_FORMAT_MDY:
      sscanf(input_date.c_str(),parsed_fmt_to_scan.c_str(), &mon, &day, &year, &hour, &min, &sec);
      break;

    case DATE_FORMAT_DYM:
      sscanf(input_date.c_str(),parsed_fmt_to_scan.c_str(), &day, &year, &mon, &hour, &min, &sec);
      break;

    case DATE_FORMAT_DMY:
      sscanf(input_date.c_str(),parsed_fmt_to_scan.c_str(),&day, &mon, &year, &hour, &min, &sec);
      break;

    default:
      HCF_CC_TRACE(SetDateTimeParsedErr, "Unsupported format provided!", -1);
    	HCF_CC_LOG(LEVEL_ERROR, "Format unrecognized!");
      return -1;
  }

  HCF_CC_LOG(LEVEL_INFO,"PARSED:   Y:%d, M:%02d, D:%02d, h:%02d, m:%02d, s:%02d",year,mon,day,hour,min,sec);
  if (const int call_result = _date_time.set_date_time(year, mon, day, hour, min, sec) != 0) {
    HCF_CC_TRACE(SetDateTimeParsedErr, "Call '_date_time.set_date_time' failed.", call_result);
    return -1;
  }

  HCF_CC_LOG(LEVEL_INFO,"DATETIME: Y:%d, M:%02d, D:%02d, h:%02d, m:%02d, s:%02d",
      _date_time.year(),_date_time.month(),_date_time.day(),_date_time.hour(),_date_time.min(),_date_time.sec());
  return 0;
}

int __CLASS_NAME__::set_date_time (const std::string & in_fmt,const std::string & in_date, const std::string & in_time) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(SetDateTime, "Received a request to set the following values in the internal data structures:",
			in_fmt.c_str(), in_date.c_str(), in_time.c_str());
	HCF_CC_LOG(LEVEL_NOTICE, "Setting date and time as for format:%s date:%s time:%s",in_fmt.c_str(),in_date.c_str(),in_time.c_str());

	HCF_CC_TRACE(SetDateTime, "Retrieving the format string!", in_fmt.c_str(), in_date.c_str(), in_time.c_str());
	std::string separator;
  _formatter.set_format(in_fmt);
  _formatter.get_date_time_format_to_scan(_sfmt, _date_position,separator);

  HCF_CC_LOG(LEVEL_NOTICE, "Retrieved separator '%s' between date and time",separator.c_str());
  int format = get_format_position(_date_position);
  std::string in_datetime = in_date + separator + in_time;

	HCF_CC_TRACE(SetDateTime, "Parsing the provided date and time values!", in_fmt.c_str(), in_date.c_str(), in_time.c_str());
  if (const int call_result = set_date_time_parsed(format, _sfmt, in_datetime)) {
  	HCF_CC_TRACE(SetDateTimeErr, "Call 'set_date_time_parsed' failed.", call_result);
    return -1;
  }

  return 0;
}
