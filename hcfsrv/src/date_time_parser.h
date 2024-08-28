/*
 * date_time_parser.h
 *
 *  Created on: Apr 9, 2015
 *      Author: xgiufer
 */

#ifndef DATE_TIME_PARSER_H_
#define DATE_TIME_PARSER_H_
#include "string.h"
#include <string>
#include "hcf/core/logger.h"
#include "date_time.h"

static const char *month_names[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

static const char *month_3char_names[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};


static const char special_char[] = {
    ' ',
    '/',
    ':',
    '-',
    '.',
    'T' //special format
};

namespace date
{
  //date position
  namespace position {
    typedef enum DateFormatPosition{
      _YEAR = 0,
      _MONTH = 1,
      _DAY = 2
    }
    date_format_position_t;
    typedef enum DatePartPosition {
      DEFAULT_POS = 0,
      FIRST_POS =  4,
      SECOND_POS = 2,
      THIRD_POS =  1
    }date_position_t;

  } //namespace position
} //namespace date

#define YYYY "%4d" // year 2000
#define YY   "%2d" // 01 from 2000
#define MMMM "%s"  // January
#define MMM  "%3s" // Jan
#define MM   "%2d" // 01 month
#define DD   "%2d" // 01 day
#define hh   "%2d"
#define mm   "%2d"
#define ss   "%2d"

#undef __CLASS_NAME__
#define __CLASS_NAME__ date_time_formatter
class __CLASS_NAME__ {

private:
  std::string _fmt_str;

public:
  __CLASS_NAME__(): _fmt_str(){};
  ~__CLASS_NAME__(){}



  void set_format(std::string format){
    _fmt_str = format;
  }

  /*
   *output_format - after scansion return the format to use in sscanf
   *order         - after scan return the order of year/month/day
   * */
  int get_date_time_format_to_scan(std::string & output_format, date::position::date_position_t (& order)[3], std::string & separator);


  int is_special_character(char c)
  {
    //HCF_CC_LOG(LEVEL_INFO,"special character\n");
    switch(c){
      case ' ':
        return 0;
      case '/':
        return 1;
      case ':':
        return 2;
      case '-':
        return 3;
      case'.':
        return 4;
      case'T':
        return 5;
      default:
        HCF_CC_LOG(LEVEL_INFO,"special default");
        return -1;
    }
  }

  int is_month_3character(const char (&month)[3]){

    for(size_t i=0; i < sizeof(month_3char_names)/sizeof(month_3char_names[0]); i++){
        if( ::strcmp(month, month_3char_names[i]) == 0)
          return i;
    }
    return -1;
  }

  int is_month(const char * month){

    for(size_t i=0; i < sizeof(month_names)/sizeof(month_names[0]); i++){
        if( ::strcmp(month, month_names[i]) == 0)
          return i;
    }
    return -1;
  }
  const char * get_year_format(const int year){
    HCF_CC_LOG(LEVEL_INFO,"year format %d",year);
    return (year == 4) ? YYYY : (year == 2) ? YY : "";
  }
  const char * get_month_format(const int month){
    HCF_CC_LOG(LEVEL_INFO,"month format %d",month);
    return (month == 4) ? MMMM : (month == 3) ? MMM : (month == 2) ? MM : "";
  }

};

#undef __CLASS_NAME__
#define __CLASS_NAME__ date_time_parser

/* create bitmask for datetime format*/
//3 bit for each part
//first position shift 6x
//second position shift 3x
                               //YYY MMM DDD
#define DATE_FORMAT_YMD 0x111  //100 010 001
#define DATE_FORMAT_YDM 0x10A  //100 001 010

#define DATE_FORMAT_MYD 0x0A1  //010 100 001
#define DATE_FORMAT_MDY 0x062  //001 100 010

#define DATE_FORMAT_DYM 0x8c   //010 001 100
#define DATE_FORMAT_DMY 0x054  //001 010 100
#define DATE_FORMAT_UNKNOWN 0x054


class __CLASS_NAME__
{
private:
  std::string _sfmt;     //format to use in sscanf
  date::position::date_position_t _date_position[3]; // order of element of date
  std::string _date;
  date_time_formatter _formatter;


  int get_format_position(const date::position::date_position_t (& pos)[3]);
  int set_date_time_parsed(const int position_fmt, const std::string & parsed_fmt_to_scan, const std::string & input_date);

public:
  __CLASS_NAME__();
  virtual inline ~__CLASS_NAME__() {}
  hcf::srv::date_time _date_time;
  int set_date_time(const std::string & in_fmt, const std::string & in_date, const std::string & in_time = "");

};


#endif /* DATE_TIME_PARSER_H_ */
