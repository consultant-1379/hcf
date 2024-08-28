#include "date_time_parser.h"
#include "rule_executor.h"
#include "rule_callbacks_handler.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_rulecallbackshandler.h"
#include "hcf/trace/tracing_macros.h"

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

#define PRINT_MATRIX_IN
//#undef PRINT_MATRIX_IN // comment this row to disable print_matrix in input

#define PRINT_MATRIX_OUT 1
using namespace std;
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(printout_parameter,SUFFIX_OP)
int __CLASS_NAME__::apply(std::string& /*printout*/, std::vector<std::string> & /*header*/, SUFFIX_CONCAT(printout_parameter,SUFFIX_C)* /* container*/)
{
	return 0;
}

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(begin,SUFFIX_OP)

int __CLASS_NAME__::apply(std::string& printout, std::vector<std::string> & /*header*/, SUFFIX_CONCAT(printout_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'begin'!");
  HCF_CC_TRACE(Apply, "Applying filter!", "Ignore", printout.c_str());

  if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
 	 HCF_CC_TRACE(Apply, "Filter input matrix: ", "Begin", printout.c_str());
    HCF_CC_LOG(LEVEL_INFO, "Begin Input == '%s'!", printout.c_str());
  }

  SUFFIX_CONCAT(begin,SUFFIX_C) begin = container->begin_c;
	std::istringstream is2(printout);
	std::string begin_result; //after_begin.append();
	string line;
	std::string begin_str(begin._value);
	int found = 0;
	HCF_CC_LOG(LEVEL_NOTICE, "Begin with == '%s'!", begin._value.c_str());
  HCF_CC_LOG(LEVEL_NOTICE, "Skip full line set to '%s'!", begin._skip_full_line.c_str());
  HCF_CC_TRACE(Apply, "Filter value is: ", "Begin", begin._value.c_str());
  HCF_CC_TRACE(Apply, "Skip full line argument is set to: ", "Begin", begin._skip_full_line.c_str());
  int skip_full_line = (begin._skip_full_line.empty()) ? 0 : (begin._skip_full_line == "true") ? 1 : 0 ;

  while(getline(is2, line,'\n'))
  {
    if(line.empty()) {
      HCF_CC_TRACE(Apply, "Empty line found, skipping it!", "Begin", "");
    	continue;
    }
    size_t pos = 0;
    if (!found && ((pos = (line.find(begin_str))) != std::string::npos))
      {
        found=1;
        if(skip_full_line) {
          HCF_CC_TRACE(Apply, "All the line must be skipped, continuing!", "Begin", "");
          continue;
        }
        else {
          HCF_CC_TRACE(Apply, "A valid match was found, appending to the result!", "Begin", "");
          begin_result.append((line.substr (pos + begin_str.size())) + "\n"); // check end row
        }
      }
    else if (found) {
      HCF_CC_TRACE(Apply, "The desired string was found, appending all the line to the result!", "Begin", "");
      begin_result.append(line + "\n");
    }
  }
	printout.clear();
	printout = begin_result;
	if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		  HCF_CC_TRACE(Apply, "Filter output matrix: ", "Begin", printout.c_str());
		  HCF_CC_LOG(LEVEL_INFO, "Begin Output == '%s'!", printout.c_str());
	}
	return 0;
}


#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(header,SUFFIX_OP)
int __CLASS_NAME__::apply( std::string& /*printout*/, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(printout_parameter,SUFFIX_C) * /*container*/){
	HCF_CC_TRACE_FUNCTION;
	return 0;
}


#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(ignore,SUFFIX_OP)

int __CLASS_NAME__::apply(std::string& printout, std::vector<std::string> & /*header*/, SUFFIX_CONCAT(printout_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'ignore'!");
  HCF_CC_TRACE(Apply, "Applying filter!", "Ignore", printout.c_str());

  if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
  	HCF_CC_TRACE(Apply, "Filter input matrix: ", "Ignore", printout.c_str());
  	HCF_CC_LOG(LEVEL_INFO, "Ignore Input == '%s'!", printout.c_str());
  }

	list<SUFFIX_CONCAT(ignore,SUFFIX_C)> ignore_list = container->ignore_list;

	for(list<SUFFIX_CONCAT(ignore,SUFFIX_C)>::iterator it = ignore_list.begin(); it != ignore_list.end(); ++it)
	{

		std::istringstream is(printout);
		string line;
		size_t posfound = 0;
		int maxlines; // se maxlines non è definito il valore deve essere a 0
		int intext = (it->_intext.empty()) ? 0 : (it->_intext == "true") ? 1 : 0 ;
		maxlines = (it->_lines.empty()) ? 0 : ::atoi(it->_lines.c_str());
		HCF_CC_LOG(LEVEL_INFO, "It's request to ignore the string '%s'!", it->_value.c_str());
		HCF_CC_LOG(LEVEL_INFO, "It's request to ignore maximum %d lines!", maxlines);
		HCF_CC_TRACE(Apply, "Received a request to ignore the following string:", "Ignore", it->_value.c_str());
		HCF_CC_TRACE(Apply, "Received a request to ignore maximum the following number of lines:", "Ignore", std::to_string(maxlines).c_str());

		int found = 0;
		std::string ignore_result;
		while(getline(is, line,'\n'))
		{
			if(line.empty() || line[0] == 13) { // 13 == ^M
			    HCF_CC_LOG(LEVEL_INFO, "Found empty line...skipping!");
					HCF_CC_TRACE(Apply, "Found an empty line, skipping it!", "Ignore", "");
			    maxlines--;
			    continue;
			}

			if ((posfound = line.find(it->_value)) != std::string::npos)
			{
				if(!intext && posfound != 0){
					HCF_CC_TRACE(Apply, "Match found, appending the line to the result!", "Ignore", "");
					ignore_result.append(line + "\n");
					line.clear();
					continue;
				}
				found = 1;
			} else if (found && ((maxlines--) <= 0)) {
				HCF_CC_TRACE(Apply, "Match found and maxlines expired!", "Ignore", "");
				ignore_result.append(line + "\n");
			}
			else if (!found) {
				HCF_CC_TRACE(Apply, "Match not found", "Ignore", "");
				ignore_result.append(line + "\n");
			}
			line.clear();
		}
		printout.clear();
		printout = ignore_result;

	}
	if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
	  HCF_CC_TRACE(Apply, "Filter output matrix: ", "Ignore", printout.c_str());
	  HCF_CC_LOG(LEVEL_INFO, "Ignore Output == '%s'!", printout.c_str());
	}
	return 0;
}

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(end,SUFFIX_OP)

int __CLASS_NAME__::apply(std::string& printout, std::vector<std::string> & /*header*/, SUFFIX_CONCAT(printout_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'end'!");
	HCF_CC_TRACE(Apply, "Applying filter!", "End", printout.c_str());

	if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
    HCF_CC_LOG(LEVEL_INFO, "End Input == '%s'!", printout.c_str());
    HCF_CC_TRACE(Apply, "Filter input matrix: ", "End", printout.c_str());
	}

	SUFFIX_CONCAT(end,SUFFIX_C) end = container->end_c;
	std::istringstream is3(printout);
	std::string end_normal_result;
	string line;
	size_t posfound = 0;

	HCF_CC_LOG(LEVEL_INFO, "Skip full line set to '%s'!", end._skip_full_line.c_str());
	HCF_CC_TRACE(Apply, "Skip full line argument is set to: ", "End", end._skip_full_line.c_str());
	int skip_full_line = (end._skip_full_line.empty()) ? 0 : (end._skip_full_line == "true") ? 1 : 0 ;

	int normal_tag_found = 0;

 	if(!end._error_list.empty()){
 		HCF_CC_TRACE(Apply, "Processing the following number of End-Error tags: ", "End", std::to_string(end._error_list.size()).c_str());
		for (size_t err=0; err < end._error_list.size();err++){
			int maxlines = (end._error_list[err]._maxlines.empty()) ? 0 : ::atoi(end._error_list[err]._maxlines.c_str());
			HCF_CC_TRACE(Apply, "The value of maxlines attribute is:", "End", std::to_string(maxlines).c_str());
			// se maxlines non è definito il valore deve essere a 0
			int intext = (end._error_list[err]._intext.empty()) ? 0 : (end._error_list[err]._intext == "true") ? 1 : 0 ;
			HCF_CC_TRACE(Apply, "The value of intext attribute is:", "End", end._error_list[err]._intext.c_str());

			int maxlines_count = 0;
			while(getline(is3, line,'\n'))
			{
				if ((maxlines != 0) && (++maxlines_count == maxlines)) {
					HCF_CC_TRACE(Apply, "Maxlines has expired, breaking!", "End", "");
					break;
				}
				if(line.empty() || line[0] == 13) {
					HCF_CC_TRACE(Apply, "Found an empty line, skipping it!", "End", "");
					continue;
				}
				if ((posfound = line.find(end._error_list[err]._value)) != std::string::npos)
				{
					HCF_CC_TRACE(Apply, "A match has been found!", "End", "");
					if(!intext && posfound != 0) continue;
					return exec_operation::evaluation_return_code::EVAL_RC_ERROR_END_ERROR_FILTER_MATCH; //break;   // Considering failed
				}
			}
		}
	}

  if (!end._normal.empty()) {
 		HCF_CC_TRACE(Apply, "Processing the End-Normal tag: ", "End", "");
    while(getline(is3, line,'\n'))
    {
        size_t pos = 0;
      if(line.empty()) {
     		HCF_CC_TRACE(Apply, "Empty line found, skipping it!", "End", "");
      	continue;
      }
      if ((pos = line.find(end._normal)) != std::string::npos)
      {
        normal_tag_found=1;
        if(skip_full_line) {
  				HCF_CC_TRACE(Apply, "A match has been found and all the line must be skipped!", "End", "");
          break;
        }
        else
        {
  				HCF_CC_TRACE(Apply, "A match has been found: appending the string to the result", "End", "");
          end_normal_result.append((line.substr (0,pos)) + "\n"); // check end row
          break;
        }
      }
      else {
				HCF_CC_TRACE(Apply, "No match has been found: appending the string to the result", "End", "");
      	end_normal_result.append(line + "\n");
      }
    }
  }

	if(normal_tag_found)
	{
		HCF_CC_TRACE(Apply, "The End-Normal printout was found!", "End", "");
		printout.clear();
		printout = end_normal_result;
	}
	if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
	  HCF_CC_TRACE(Apply, "Filter output matrix: ", "End", printout.c_str());
	  HCF_CC_LOG(LEVEL_INFO, "End Output == '%s'!", printout.c_str());
	}
	return 0;
}


#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(filter_parameter,SUFFIX_OP)
int __CLASS_NAME__::apply( matrix_str_t  &/*set_matrix*/, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * /*container*/){
	HCF_CC_TRACE_FUNCTION;
	return 0;
}

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(equal,SUFFIX_OP)

int __CLASS_NAME__::apply( matrix_str_t& matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Equal", "");
	HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'equal'!");
#ifdef PRINT_MATRIX_IN
	matrix_handler::print_matrix(matrix);
#endif
	list<SUFFIX_CONCAT(equal,SUFFIX_C)> equal_list = container->equal_list;

	for(list<SUFFIX_CONCAT(equal,SUFFIX_C)>::iterator it = equal_list.begin(); it != equal_list.end(); ++it)
	  {
	    string name(it->_name);
	    string value(it->_value);
	    string type((it->_type.empty()) ? "number": it->_type);
	  	HCF_CC_TRACE(Apply, "The value of name attribute is:", "Equal", name.c_str());
	  	HCF_CC_TRACE(Apply, "The value of value attribute is:", "Equal", value.c_str());
	  	HCF_CC_TRACE(Apply, "The value of type attribute is:", "Equal", type.c_str());

	    matrix_str_t matrix_equal;

	    size_t column_index = 0;
	    int index_result = matrix_handler::find_header_index_by_string(header, name, column_index);
	    if(index_result < 0) {
		  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Equal", index_result);
	    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
	    }

	    if(type == "param")
	      {
		  		HCF_CC_TRACE(Apply, "The customizable parameter with the following name is required:", "Equal", value.c_str());
	        std::map<std::string, std::pair<std::string, std::string>>::const_iterator it_param = container->_rule_attrs->find(value);
	        if(it_param == container->_rule_attrs->end())
	          {
	        		HCF_CC_TRACE(ApplyErr, "Undefined customizable parameter provided!", "Equal", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
	            return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
	          }
	        type = it_param->second.first;
	        value = it_param->second.second;
	  	  	HCF_CC_TRACE(Apply, "The new value of value attribute is:", "Equal", value.c_str());
	  	  	HCF_CC_TRACE(Apply, "The new value of type attribute is:", "Equal", type.c_str());
	      }
	    HCF_CC_LOG(LEVEL_NOTICE, "Fetched param: type = %s, value = %s", type.c_str(), value.c_str());
	    for(size_t row=0; row<matrix.size();row++)
	      {
	        if(!(column_index <= matrix[row].size()))
	          {
	            HCF_CC_LOG(LEVEL_WARNING, "Columns oversize!  == %zu!\n",matrix[row].size());
	        		HCF_CC_TRACE(ApplyErr, "Columns oversize found!", "Equal", exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE);
	            return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE;
	          }
	        if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
	          HCF_CC_LOG(LEVEL_INFO,"%s", matrix[row][column_index].c_str());
		  	  	HCF_CC_TRACE(Apply, "Working on the following line:", "Equal", matrix[row][column_index].c_str());
	        }

	        if ((type == "number") && (::atof((matrix[row][column_index]).c_str()) == ::atof(value.c_str())))
	          {
	            if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
	              HCF_CC_LOG(LEVEL_NOTICE, "Found number equal at column %zu and row %zu!\n", column_index,row);
	  		  	  	HCF_CC_TRACE(Apply, "Found number value equal to the provided one!", "Equal", matrix[row][column_index].c_str());
	            }
	            matrix_equal.push_back(matrix[row]);
	          }
	        else if ((type == "string") && (matrix[row][column_index] == value))
	          {
	            if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
	              HCF_CC_LOG(LEVEL_NOTICE, "Found string equal at column %zu and row %zu!\n", column_index,row);
	  		  	  	HCF_CC_TRACE(Apply, "Found string value equal to the provided one!", "Equal", matrix[row][column_index].c_str());
	            }
	            matrix_equal.push_back(matrix[row]);
	          }
	      }

	    matrix.clear();
	    matrix = matrix_equal;

	  }

#ifdef PRINT_MATRIX_OUT
  matrix_handler::print_matrix(matrix);
#endif
	return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(greater,SUFFIX_OP)

int __CLASS_NAME__::apply( matrix_str_t& matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Greater", "");
	HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'greater'!");
#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif

	list<SUFFIX_CONCAT(greater,SUFFIX_C)> greater_list = container->greater_list;

	for(list<SUFFIX_CONCAT(greater,SUFFIX_C)>::iterator it = greater_list.begin(); it != greater_list.end(); ++it)
	{
		string name(it->_name);
		string value(it->_value);
		string type((it->_type.empty()) ? "number": it->_type);
  	HCF_CC_TRACE(Apply, "The value of name attribute is:", "Greater", name.c_str());
  	HCF_CC_TRACE(Apply, "The value of value attribute is:", "Greater", value.c_str());
  	HCF_CC_TRACE(Apply, "The value of type attribute is:", "Greater", type.c_str());

		matrix_str_t matrix_greater;

		size_t column_index = 0;
		int index_result = matrix_handler::find_header_index_by_string(header, name, column_index);
		if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Greater", index_result);
			return -1;
		}

		if(type == "param")
		{
  		HCF_CC_TRACE(Apply, "The customizable parameter with the following name is required:", "Greater", value.c_str());
			std::map<std::string, std::pair<std::string, std::string>>::const_iterator it_param = container->_rule_attrs->find(value);
			if(it_param == container->_rule_attrs->end())
			{
    		HCF_CC_TRACE(ApplyErr, "Undefined customizable parameter provided!", "Greater", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
				HCF_CC_LOG(LEVEL_ERROR, "Param name not found in map!");
				return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
			}
			type = it_param->second.first;
			value = it_param->second.second;
	  	HCF_CC_TRACE(Apply, "The new value of value attribute is:", "Greater", value.c_str());
	  	HCF_CC_TRACE(Apply, "The new value of type attribute is:", "Greater", type.c_str());
		}
		HCF_CC_LOG(LEVEL_NOTICE, "Fetched param: type = %s, value = %s", type.c_str(), value.c_str());
		for(size_t row=0; row<matrix.size();row++)
		{
			if(!(column_index <= matrix[row].size())) {
				HCF_CC_LOG(LEVEL_WARNING, "Columns oversize!  == %zu!\n",matrix[row].size());
    		HCF_CC_TRACE(ApplyErr, "Columns oversize found!", "Greater", exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE);
			}

			if (column_index <= matrix[row].size() )
			{
			    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
			      HCF_CC_LOG(LEVEL_INFO,"%s", matrix[row][column_index].c_str());
		  	  	HCF_CC_TRACE(Apply, "Working on the following line:", "Greater", matrix[row][column_index].c_str());
			    }
				if ((type == "number") && (::atof((matrix[row][column_index]).c_str()) /*is greater then*/ > ::atof(value.c_str())))
				{
				  if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
				    HCF_CC_LOG(LEVEL_NOTICE, "Found number value greater at column %zu and row %zu!\n", column_index,row);
		  	  	HCF_CC_TRACE(Apply, "Found number value greater than the provided one!", "Greater", matrix[row][column_index].c_str());
				  }
				  matrix_greater.push_back(matrix[row]);
				}else if ((type == "string") && (matrix[row][column_index] > value))
				{
				    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
				      HCF_CC_LOG(LEVEL_NOTICE, "Found string value greater at column %zu and row %zu!\n", column_index,row);
			  	  	HCF_CC_TRACE(Apply, "Found string value greater than the provided one!", "Greater", matrix[row][column_index].c_str());
				    }
					matrix_greater.push_back(matrix[row]);
				}
			}
		}
		matrix.clear();
		matrix = matrix_greater;
	}
#ifdef PRINT_MATRIX_OUT
  matrix_handler::print_matrix(matrix);
#endif
	return 0 ;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(less,SUFFIX_OP)

int __CLASS_NAME__::apply( matrix_str_t& matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Less", "");
	HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'less'!");
#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif
	list<SUFFIX_CONCAT(less,SUFFIX_C)> less_list = container->less_list;

	for(list<SUFFIX_CONCAT(less,SUFFIX_C)>::iterator it = less_list.begin(); it != less_list.end(); ++it)
	{
		string name(it->_name);
		string value(it->_value);
		string type((it->_type.empty()) ? "number": it->_type);
  	HCF_CC_TRACE(Apply, "The value of name attribute is:", "Less", name.c_str());
  	HCF_CC_TRACE(Apply, "The value of value attribute is:", "Less", value.c_str());
  	HCF_CC_TRACE(Apply, "The value of type attribute is:", "Less", type.c_str());

		std::vector< std::vector<std::string> > matrix_less;

		size_t column_index = 0;
		int index_result = matrix_handler::find_header_index_by_string(header, name, column_index);
    if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Less", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }

		if(type == "param")
		{
  		HCF_CC_TRACE(Apply, "The customizable parameter with the following name is required:", "Less", value.c_str());
			std::map<std::string, std::pair<std::string, std::string>>::const_iterator it_param = container->_rule_attrs->find(value);
			if(it_param == container->_rule_attrs->end())
			{
    		HCF_CC_TRACE(ApplyErr, "Undefined customizable parameter provided!", "Less", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
				HCF_CC_LOG(LEVEL_ERROR, "Param name not found in map!");
				return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
			}
			type = it_param->second.first;
			value = it_param->second.second;
	  	HCF_CC_TRACE(Apply, "The new value of value attribute is:", "Less", value.c_str());
	  	HCF_CC_TRACE(Apply, "The new value of type attribute is:", "Less", type.c_str());
		}
		HCF_CC_LOG(LEVEL_NOTICE, "Fetched param: type = %s, value = %s", type.c_str(), value.c_str());
		for(size_t row=0; row<matrix.size();row++)
		{
			if(!(column_index <= matrix[row].size())) {
				HCF_CC_LOG(LEVEL_WARNING, "Columns oversize!  == %zu!",matrix[row].size());
    		HCF_CC_TRACE(ApplyErr, "Columns oversize found!", "Less", exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE);
			}

			if (column_index <= matrix[row].size() )
			{
			    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
			      HCF_CC_LOG(LEVEL_INFO,"%s", matrix[row][column_index].c_str());
		  	  	HCF_CC_TRACE(Apply, "Working on the following line:", "Less", matrix[row][column_index].c_str());
			    }
				if ((type == "number") && (::atof((matrix[row][column_index]).c_str()) /*is less then*/ < ::atof(value.c_str())))
				{
				    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
				      HCF_CC_LOG(LEVEL_NOTICE, "Found number value less at column %zu and row %zu!", column_index,row);
  		  	  	HCF_CC_TRACE(Apply, "Found number value less than the provided one!", "Less", matrix[row][column_index].c_str());
				    }
					matrix_less.push_back(matrix[row]);
				}
				else if ((type == "string") && (matrix[row][column_index] < value))
				{
				    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
				      HCF_CC_LOG(LEVEL_NOTICE, "Found string value less at column %zu and row %zu!", column_index,row);
  		  	  	HCF_CC_TRACE(Apply, "Found string value less than the provided one!", "Less", matrix[row][column_index].c_str());
				    }
					matrix_less.push_back(matrix[row]);
				}
			}
		}
		matrix.clear();
		matrix = matrix_less;
	}
#ifdef PRINT_MATRIX_OUT
  matrix_handler::print_matrix(matrix);
#endif
	return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(match,SUFFIX_OP)

int __CLASS_NAME__::apply( matrix_str_t& matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Match", "");
	HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'match'!");
#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif
	list<SUFFIX_CONCAT(match,SUFFIX_C)> match_list = container->match_list;

	for(list<SUFFIX_CONCAT(match,SUFFIX_C)>::iterator it = match_list.begin(); it != match_list.end(); ++it)
	{
		string name(it->_name);
		string value(it->_value);
		string type((it-> _type.empty()) ? "number": it->_type);
  	HCF_CC_TRACE(Apply, "The value of name attribute is:", "Match", name.c_str());
  	HCF_CC_TRACE(Apply, "The value of value attribute is:", "Match", value.c_str());
  	HCF_CC_TRACE(Apply, "The value of type attribute is:", "Match", type.c_str());

		matrix_str_t matrix_match;

		size_t column_index = 0;
		int index_result = matrix_handler::find_header_index_by_string(header, name, column_index);
    if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Match", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }

		if(type == "param")
		{
  		HCF_CC_TRACE(Apply, "The customizable parameter with the following name is required:", "Match", value.c_str());
			std::map<std::string, std::pair<std::string, std::string>>::const_iterator it_param = container->_rule_attrs->find(value);
			if(it_param == container->_rule_attrs->end())
			{
				HCF_CC_LOG(LEVEL_ERROR, "Param name not found in map!");
    		HCF_CC_TRACE(ApplyErr, "Undefined customizable parameter provided!", "Match", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
				return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
			}
			type = it_param->second.first;
			value = it_param->second.second;
	  	HCF_CC_TRACE(Apply, "The new value of value attribute is:", "Match", value.c_str());
	  	HCF_CC_TRACE(Apply, "The new value of type attribute is:", "Match", type.c_str());
		}
		HCF_CC_LOG(LEVEL_NOTICE, "Fetched param: type = %s, value = %s", type.c_str(), value.c_str());
		for(size_t row=0; row<matrix.size();row++)
		{
			if(!(column_index <= matrix[row].size())) {
				HCF_CC_LOG(LEVEL_WARNING, "Columns oversize!  == %zu!",matrix[row].size());
    		HCF_CC_TRACE(ApplyErr, "Columns oversize found!", "Match", exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE);
			}

			if (column_index <= matrix[row].size() )
			{
			    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
			      HCF_CC_LOG(LEVEL_INFO,"%s", matrix[row][column_index].c_str());
		  	  	HCF_CC_TRACE(Apply, "Working on the following line:", "Match", matrix[row][column_index].c_str());
			    }
				if (((matrix[row][column_index]).find(value)!= std::string::npos))
				{
				    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
				      HCF_CC_LOG(LEVEL_NOTICE, "Found value that match at column %zu and row %zu!", column_index,row);
  		  	  	HCF_CC_TRACE(Apply, "Found matching value to the provided one!", "Match", matrix[row][column_index].c_str());
				    }
					matrix_match.push_back(matrix[row]);
				}
			}
		}
		matrix.clear();
		matrix = matrix_match;
	}
#ifdef PRINT_MATRIX_OUT
  matrix_handler::print_matrix(matrix);
#endif
	return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(not_equal,SUFFIX_OP)

int __CLASS_NAME__::apply( matrix_str_t& matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Not Equal", "");
	HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'not_equal'!");
#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif
	list<SUFFIX_CONCAT(not_equal,SUFFIX_C)> not_equal_list = container->not_equal_list;

	for(list<SUFFIX_CONCAT(not_equal,SUFFIX_C)>::iterator it = not_equal_list.begin(); it != not_equal_list.end(); ++it)
	  {
	    string name(it->_name);
	    string value(it->_value);
	    string type((it-> _type.empty()) ? "number": it->_type);
	  	HCF_CC_TRACE(Apply, "The value of name attribute is:", "Not Equal", name.c_str());
	  	HCF_CC_TRACE(Apply, "The value of value attribute is:", "Not Equal", value.c_str());
	  	HCF_CC_TRACE(Apply, "The value of type attribute is:", "Not Equal", type.c_str());

	    matrix_str_t matrix_not_equal;

	    size_t column_index = 0;
	    int index_result = matrix_handler::find_header_index_by_string(header, name, column_index);
	    if(index_result < 0) {
		  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Not Equal", index_result);
	    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
	    }

	    if(type == "param")
	      {
	  		HCF_CC_TRACE(Apply, "The customizable parameter with the following name is required:", "Not Equal", value.c_str());
	        std::map<std::string, std::pair<std::string, std::string>>::const_iterator it_param = container->_rule_attrs->find(value);
	        if(it_param == container->_rule_attrs->end())
	          {
							HCF_CC_TRACE(ApplyErr, "Undefined customizable parameter provided!", "Not Equal", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
							HCF_CC_LOG(LEVEL_ERROR, "Param name not found in map!");
	            return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
	          }
	        type = it_param->second.first;
	        value = it_param->second.second;
	  	  	HCF_CC_TRACE(Apply, "The new value of value attribute is:", "Not Equal", value.c_str());
	  	  	HCF_CC_TRACE(Apply, "The new value of type attribute is:", "Not Equal", type.c_str());
	      }
	    HCF_CC_LOG(LEVEL_NOTICE, "Fetched param: type = %s, value = %s", type.c_str(), value.c_str());
	    for(size_t row=0; row<matrix.size();row++)
	      {
	        if(!(column_index <= matrix[row].size()))
	          {
	            HCF_CC_LOG(LEVEL_WARNING, "Columns oversize!  == %zu!\n",matrix[row].size());
	        		HCF_CC_TRACE(ApplyErr, "Columns oversize found!", "Not Equal", exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE);
	            return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE;
	          }

	        if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
	          HCF_CC_LOG(LEVEL_INFO,"%s", matrix[row][column_index].c_str());
		  	  	HCF_CC_TRACE(Apply, "Working on the following line:", "Not Equal", matrix[row][column_index].c_str());
	        }
	        if ((type == "number") && (::atof((matrix[row][column_index]).c_str()) != ::atof(value.c_str())))
	          {
	            if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
	              HCF_CC_LOG(LEVEL_NOTICE, "Found number not equal at column %zu and row %zu!\n", column_index,row);
	  		  	  	HCF_CC_TRACE(Apply, "Found number value not equal to the provided one!", "Not Equal", matrix[row][column_index].c_str());
	            }
	            matrix_not_equal.push_back(matrix[row]);
	          }
	        else if ((type == "string") && (matrix[row][column_index] != value))
	          {
	            if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
	              HCF_CC_LOG(LEVEL_NOTICE, "Found string not equal at column %zu and row %zu!\n", column_index,row);
	  		  	  	HCF_CC_TRACE(Apply, "Found string value not equal to the provided one!", "Not Equal", matrix[row][column_index].c_str());
	            }
	            matrix_not_equal.push_back(matrix[row]);
	          }
	      }

	    matrix.clear();
	    matrix = matrix_not_equal;
	  }

#ifdef PRINT_MATRIX_OUT
  matrix_handler::print_matrix(matrix);
#endif
	return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(range,SUFFIX_OP)

int __CLASS_NAME__::apply( matrix_str_t& matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Range", "");
	HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'range'!");
#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif
	list<SUFFIX_CONCAT(range,SUFFIX_C)> range_list = container->range_list;

	for(list<SUFFIX_CONCAT(range,SUFFIX_C)>::iterator it = range_list.begin(); it != range_list.end(); ++it)
	{
		string name(it->_name);
		string value_max(it->_max);
		string value_min(it->_min);
		string type((it->_type.empty()) ? "number": it->_type);
  	HCF_CC_TRACE(Apply, "The value of name attribute is:", "Range", name.c_str());
  	HCF_CC_TRACE(Apply, "The value of value_min attribute is:", "Range", value_min.c_str());
  	HCF_CC_TRACE(Apply, "The value of value_max attribute is:", "Range", value_max.c_str());
  	HCF_CC_TRACE(Apply, "The value of type attribute is:", "Range", type.c_str());

		std::vector< std::vector<std::string> > matrix_range;

		size_t column_index = 0;
		int index_result = matrix_handler::find_header_index_by_string(header, name, column_index);
    if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Range", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }

		if(type == "param")
		{
  		HCF_CC_TRACE(Apply, "The customizable parameter with the following name is required:", "Range", value_min.c_str());
  		HCF_CC_TRACE(Apply, "The customizable parameter with the following name is required:", "Range", value_max.c_str());
			std::map<std::string, std::pair<std::string, std::string>>::const_iterator it_param_min = container->_rule_attrs->find(value_min);
			std::map<std::string, std::pair<std::string, std::string>>::const_iterator it_param_max = container->_rule_attrs->find(value_max);
			if(it_param_min ==  container->_rule_attrs->end() || it_param_max ==  container->_rule_attrs->end())
			{
				HCF_CC_LOG(LEVEL_ERROR, "Param name not found in map!");
				return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
			}
			type = it_param_min->second.first;
			value_min = it_param_min->second.second;
			value_max = it_param_max->second.second;
	  	HCF_CC_TRACE(Apply, "The new value of value_min attribute is:", "Range", value_min.c_str());
	  	HCF_CC_TRACE(Apply, "The new value of value_max attribute is:", "Range", value_max.c_str());
		}
		HCF_CC_LOG(LEVEL_NOTICE, "Fetched param: type = %s, value_min = %s, value_max = %s", type.c_str(), value_min.c_str(), value_max.c_str());
		for(size_t row=0; row<matrix.size();row++)
		{
			if(!(column_index <= matrix[row].size())) {
				HCF_CC_LOG(LEVEL_WARNING, "Column oversize!  == %zu!",matrix[row].size());
    		HCF_CC_TRACE(ApplyErr, "Columns oversize found!", "Range", exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE);
			}

			if (column_index <= matrix[row].size() )
      {
			    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
			      HCF_CC_LOG(LEVEL_INFO,"%s", matrix[row][column_index].c_str());
		  	  	HCF_CC_TRACE(Apply, "Working on the following line:", "Range", matrix[row][column_index].c_str());
			    }
        if ((type == "number") &&
            (::atof((matrix[row][column_index]).c_str()) >= ::atof(value_min.c_str())) &&
            (::atof((matrix[row][column_index]).c_str()) <= ::atof(value_max.c_str())))
          {
            if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
              HCF_CC_LOG(LEVEL_NOTICE, "Found number value in range at column %zu and row %zu!", column_index,row);
  		  	  	HCF_CC_TRACE(Apply, "Found number value in the correct range!", "Range", matrix[row][column_index].c_str());
            }
            matrix_range.push_back(matrix[row]);
          }else if ((type == "string") &&
              ((matrix[row][column_index] >= value_min) &&
                  (matrix[row][column_index] <= value_max)))
            {
              if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
                HCF_CC_LOG(LEVEL_NOTICE, "Found string value in range at column %zu and row %zu!", column_index,row);
    		  	  	HCF_CC_TRACE(Apply, "Found string value in the correct range!", "Range", matrix[row][column_index].c_str());
              }
              matrix_range.push_back(matrix[row]);
            }
      }
		}
		matrix.clear();
		matrix = matrix_range;
	}

#ifdef PRINT_MATRIX_OUT
  matrix_handler::print_matrix(matrix);
#endif

	return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(unite,SUFFIX_OP)
int __CLASS_NAME__::apply( matrix_str_t& matrix_one, matrix_str_t& matrix_two,  SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * /*container*/){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Unite", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'unite'!");

  matrix_str_t  matrix_unite = matrix_one;
  for(matrix_str_t::const_iterator it_two = matrix_two.begin(); it_two != matrix_two.end();++it_two)
  {
      int found_eq = 0;
      for(matrix_str_t::const_iterator it_one = matrix_one.begin(); it_one != matrix_one.end();++it_one)
      {
          if (*it_one == *it_two)
           {
              found_eq = 1;
              HCF_CC_LOG(LEVEL_NOTICE, "Found element equal! It will not be added");
            	HCF_CC_TRACE(Apply, "Found element equal in the matrix: it will be not added!", "Unite", "");
              break;
           }
          else
            continue;
      }
      if (!found_eq){
          HCF_CC_LOG(LEVEL_NOTICE, "Adding to the matrix the element found");
        	HCF_CC_TRACE(Apply, "The element was not present in the matrix: adding it!", "Unite", "");
          matrix_unite.push_back(*it_two);
      }
  }

  matrix_handler::print_matrix(matrix_unite);
  matrix_one.clear();
  matrix_one = matrix_unite;

  return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(intersect,SUFFIX_OP)

int __CLASS_NAME__::apply( matrix_str_t& matrix_one, matrix_str_t& matrix_two,  SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * /*container*/){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Intersect", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'intersect'!");

    matrix_str_t matrix_intersect;
    matrix_str_t::const_iterator it_one = matrix_one.begin();
    matrix_str_t::const_iterator it_two = matrix_two.begin();


    if(matrix_one.empty() || matrix_two.empty())
      {
        HCF_CC_LOG(LEVEL_WARNING, "One or Two matrix empty - The intersection will be null!");
      	HCF_CC_TRACE(Apply, "One or both the input matrixes are empty: the intersection will be the empty set!", "Intersect", "");
        matrix_one.clear();
        matrix_one = matrix_intersect;
        return 0;
      }

    int match=0;
    while( it_one != matrix_one.end())
    {
      std::vector<std::string>::const_iterator itit_one = it_one->begin();
      std::vector<std::string>::const_iterator itit_two = it_two->begin();
      while(itit_one != it_one->end() && itit_two != it_two->end())
      {
        if(*itit_one != *itit_two) {match=0; break;}
        else {++itit_one; ++itit_two; match=1;}
      }
      if (match) {
          matrix_intersect.push_back(*it_one);
          ++it_one;
          it_two = matrix_two.begin(); //reset matrix two
          continue;
      }
      if(++it_two == matrix_two.end()){
        ++it_one;
        it_two = matrix_two.begin(); //reset matrix two
      }
    }
    matrix_handler::print_matrix(matrix_intersect);
    matrix_one.clear();
    matrix_one = matrix_intersect;

  return 0 ;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(evaluation_parameter,SUFFIX_OP)
int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &/*set_matrix*/, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * /*container*/){
	HCF_CC_TRACE_FUNCTION;
	return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(and,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &/*set_matrix*/, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * /*container*/){
	HCF_CC_TRACE_FUNCTION;
	return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(or,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &/*set_matrix*/, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * /*container*/){
	HCF_CC_TRACE_FUNCTION;
	return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(not,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &/*set_matrix*/, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * /*container*/){
	HCF_CC_TRACE_FUNCTION;
	return 0;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(all_in_period,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "All-In-Period", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'all_in_period'!");
   list<SUFFIX_CONCAT(all_in_period,SUFFIX_C)> all_in_period_list = container->all_in_period_list;

  //int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
  for(list<SUFFIX_CONCAT(all_in_period,SUFFIX_C)>::iterator it = all_in_period_list.begin(); it != all_in_period_list.end(); ++it)
    {
      std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix = set_matrix.find(it->_set); //string set_name("")
      if(it_matrix ==  set_matrix.end()) {
		  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "All-In-Period", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
        return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
      }

      HCF_CC_LOG(LEVEL_INFO, "Verified set name: (%s)",it->_set.c_str());
  		HCF_CC_TRACE(Apply, "Working on the following set:", "All-In-Period", it->_set.c_str());

      header = it_matrix->second.first;
      matrix_str_t matrix = it_matrix->second.second;
      size_t column_date_index = 0;
      size_t column_time_index = 0;

      if(it->_format.empty()) {
		  	HCF_CC_TRACE(ApplyErr, "Empty format string provided!", "All-In-Period", exec_operation::evaluation_return_code::EVAL_RC_FALSE);
        return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
      }

      if(it->_col_date.empty() && !it->_col_time.empty())
        {
          HCF_CC_LOG(LEVEL_ERROR, "Error! It's not possible to define only the time value.");
  		  	HCF_CC_TRACE(ApplyErr, "Both date and time must be provided!!", "All-In-Period", exec_operation::evaluation_return_code::EVAL_RC_FALSE);
          return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
        }

      int index_result = matrix_handler::find_header_index_by_string(header, it->_col_date, column_date_index);
      if(index_result < 0 && it->_col_date.empty() && it->_col_time.empty()) {
      	column_date_index = 0; //default first column if column is not set to handle row based
    		HCF_CC_TRACE(Apply, "Date index not found: using the first one as default", "All-In-Period", it->_col_date.c_str());
      }
      else if(index_result < 0) {
		  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "All-In-Period", index_result);
	    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
	    }

      index_result = matrix_handler::find_header_index_by_string(header, it->_col_time, column_time_index);
      if(index_result < 0) {
      	HCF_CC_LOG(LEVEL_WARNING, "Warning! Column time is not found. May be that it's not needed");
		  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed: probably it's not needed!", "Unite", index_result);
	    }

	  	HCF_CC_TRACE(Apply, "The value of format attribute is:", "All-In-Period", it->_format.c_str());
	  	HCF_CC_TRACE(Apply, "The value of start_time attribute is:", "All-In-Period", it->_start_time.c_str());
	  	HCF_CC_TRACE(Apply, "The value of end_time attribute is:", "All-In-Period", it->_end_time.c_str());

      //TODO - ADD ERROR CHECK ON SET_DATE_TIME
      date_time_parser dt_parser_start;

      HCF_CC_LOG(LEVEL_INFO, "Setting ***START*** time");
      dt_parser_start.set_date_time(it->_format, it->_start_time);
      date_time_parser dt_parser_end;
      HCF_CC_LOG(LEVEL_INFO, "Setting ***END*** time");
      dt_parser_end.set_date_time(it->_format, it->_end_time);

      HCF_CC_LOG(LEVEL_INFO, "Checking ***VALUE*** time");
      for(size_t row=0; row<matrix.size();row++)
        {
          date_time_parser dt_parsed_element;
          if(column_date_index > matrix[row].size()  || column_time_index > matrix[row].size()) {
        		HCF_CC_TRACE(ApplyErr, "Columns oversize found!", "All-In-Period", exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE);
            return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE;
          }

          if(it->_col_time.empty())
            {
              HCF_CC_LOG(LEVEL_INFO, "matrix date %s",(matrix[row][column_date_index]).c_str());
      	  		HCF_CC_TRACE(Apply, "The read date value is :", "All-In-Period", (matrix[row][column_date_index]).c_str());
              dt_parsed_element.set_date_time(it->_format, (matrix[row][column_date_index]));
            }
          else
            {
              HCF_CC_LOG(LEVEL_INFO, "matrix date %s",(matrix[row][column_date_index]).c_str());
              HCF_CC_LOG(LEVEL_INFO, "matrix time %s",(matrix[row][column_time_index]).c_str());
      	  		HCF_CC_TRACE(Apply, "The read date value is :", "All-In-Period", (matrix[row][column_date_index]).c_str());
      	  		HCF_CC_TRACE(Apply, "The read time value is :", "All-In-Period", (matrix[row][column_time_index]).c_str());
              dt_parsed_element.set_date_time(it->_format, (matrix[row][column_date_index]),(matrix[row][column_time_index]));
            }

          if(dt_parsed_element._date_time < dt_parser_start._date_time ||
              dt_parsed_element._date_time > dt_parser_end._date_time)
            {
              HCF_CC_LOG(LEVEL_NOTICE, "Found datetime value out of range inserted!");
      	  		HCF_CC_TRACE(Apply, "The found value is out of the range: returning FALSE!", "All-In-Period", "");
            return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
          }
        }

    }
  HCF_CC_LOG(LEVEL_NOTICE, "All datetime values are between stard/end time");
	HCF_CC_TRACE(Apply, "All the values are in the range: returning TRUE!", "All-In-Period", "");
  return exec_operation::evaluation_return_code::EVAL_RC_TRUE;

};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(all_in_range,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "All-In-Range", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'all_in_range'!");
  list<SUFFIX_CONCAT(all_in_range,SUFFIX_C)> all_in_range_list = container->all_in_range_list;

	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
	for(list<SUFFIX_CONCAT(all_in_range,SUFFIX_C)>::iterator it = all_in_range_list.begin(); it != all_in_range_list.end(); ++it)
	{

		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix = set_matrix.find(it->_set); //string set_name("")
		if(it_matrix ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "All-In-Range", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}
		HCF_CC_LOG(LEVEL_NOTICE, "Verified set name: (%s)",it->_set.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set:", "All-In-Range", it->_set.c_str());

		string value_max(it->_max);
		string value_min(it->_min);
		string type;
		type = (it->_type.empty()) ? "number": it->_type;
		header = it_matrix->second.first;
		matrix_str_t matrix = it_matrix->second.second;
		matrix_str_t matrix_all_in_range;

  	HCF_CC_TRACE(Apply, "The value of value_max attribute is:", "All-In-Range", value_max.c_str());
  	HCF_CC_TRACE(Apply, "The value of value_min attribute is:", "All-In-Range", value_min.c_str());
  	HCF_CC_TRACE(Apply, "The value of type attribute is:", "All-In-Range", type.c_str());

#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif

		size_t column_index = 0;
		int index_result = matrix_handler::find_header_index_by_string(header, it->_col, column_index);
		if(index_result < 0 && it->_col.empty()) {
  		HCF_CC_TRACE(Apply, "Date index not found: using the first one as default", "All-In-Range", it->_col.c_str());
		  column_index = 0; //default first column if column is not set
		}
    else if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "All-In-Range", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }

		for(size_t row=0; row<matrix.size();row++)
		  {
		    if(!(column_index <= matrix[row].size())) {
		      HCF_CC_LOG(LEVEL_WARNING, "Column oversize!  == %zu!",matrix[row].size());
      		HCF_CC_TRACE(ApplyErr, "Columns oversize found!", "All-In-Range", exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE);
		    }

		    if (column_index <= matrix[row].size() )
		      {
		        if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		          HCF_CC_LOG(LEVEL_INFO,"%s", matrix[row][column_index].c_str());
			  	  	HCF_CC_TRACE(Apply, "Working on the following line:", "All-In-Range", matrix[row][column_index].c_str());
		        }
		        if (type == "number")
		          if(::atof((matrix[row][column_index]).c_str()) >= ::atof(value_min.c_str()) &&
		              ::atof((matrix[row][column_index]).c_str()) <= ::atof(value_max.c_str()))
		            {
		              if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		                HCF_CC_LOG(LEVEL_NOTICE, "Found number value in range at column %zu and row %zu!", column_index,row);
		    		  	  	HCF_CC_TRACE(Apply, "Found number value in the correct range!", "All-In-Range", matrix[row][column_index].c_str());
		              }
		              continue;
		            }else{
		                if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		                  HCF_CC_LOG(LEVEL_NOTICE, "Found number value NOT in range at column %zu and row %zu!", column_index,row);
			    		  	  	HCF_CC_TRACE(Apply, "Found number value NOT in the correct range: returning FALSE!", "All-In-Range", matrix[row][column_index].c_str());
		                }
		                return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
		            }
		        else if (type == "string"){
		            if(((matrix[row][column_index] >= value_min)) &&
		                ((matrix[row][column_index] <= value_max)))
		              {
		                if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		                  HCF_CC_LOG(LEVEL_NOTICE, "Found string value in range at column %zu and row %zu!", column_index,row);
			    		  	  	HCF_CC_TRACE(Apply, "Found string value in the correct range!", "All-In-Range", matrix[row][column_index].c_str());
		                }
		                continue;
		              }else{
		                  if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		                    HCF_CC_LOG(LEVEL_NOTICE, "Found string value NOT in range at column %zu and row %zu!", column_index,row);
				    		  	  	HCF_CC_TRACE(Apply, "Found string value NOT in the correct range: returning FALSE!", "All-In-Range", matrix[row][column_index].c_str());
		                  }
		                  return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
		              }
		        }
		      }
		  }
	}
	HCF_CC_LOG(LEVEL_NOTICE,"The result is:%d the evaluation result is %s",evaluation_result,
	      (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	HCF_CC_TRACE(Apply, "All the found values are in the correct range: returning TRUE!", "All-In-Range", "");

	return exec_operation::evaluation_return_code::EVAL_RC_TRUE;

};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(columns_equal,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Columns-Equal", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'columns_equal'!");
  list<SUFFIX_CONCAT(columns_equal,SUFFIX_C)>  columns_equal_list = container->columns_equal_list;
	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;

	for(list<SUFFIX_CONCAT(columns_equal,SUFFIX_C)>::iterator it = columns_equal_list.begin(); it != columns_equal_list.end(); ++it)
	{
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix = set_matrix.find(it->_set); //string set_name("")
		if(it_matrix ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Columns-Equal", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}

		HCF_CC_LOG(LEVEL_NOTICE, "Verified set name: (%s)",it->_set.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set:", "Columns-Equal", it->_set.c_str());

		header = it_matrix->second.first;
		matrix_str_t matrix = it_matrix->second.second;

#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif

		string column_left(it->_col_left);
		string column_right(it->_col_right);

		size_t column_left_index = 0;
		size_t column_right_index = 0;

  	HCF_CC_TRACE(Apply, "The value of col_left attribute is:", "Columns-Equal", it->_col_left.c_str());
  	HCF_CC_TRACE(Apply, "The value of col_right attribute is:", "Columns-Equal", it->_col_right.c_str());

		int index_result = matrix_handler::find_header_index_by_string(header, column_left, column_left_index);
    if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Columns-Equal", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }
		index_result = matrix_handler::find_header_index_by_string(header, column_right, column_right_index);
    if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Columns-Equal", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }

		for(size_t row=0; row<matrix.size();row++)
		{
			if(matrix[row][column_left_index] == matrix[row][column_right_index])
			{
			    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
			      HCF_CC_LOG(LEVEL_NOTICE, "Found string value equal at row %zu!", row);
		  	  	HCF_CC_TRACE(Apply, "Found string value equal between the columns!", "Columns-Equal", matrix[row][column_left_index].c_str());
			    }
				continue;
			}else{
			    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL)
			      HCF_CC_LOG(LEVEL_NOTICE, "Found string NOT equal at row %zu!",row);
			    	HCF_CC_TRACE(Apply, "Found string value NOT equal between the columns: returning FALSE!", "Columns-Equal", matrix[row][column_left_index].c_str());
				return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
			}
		}
	}
	HCF_CC_LOG(LEVEL_NOTICE,"The result is:%d the evaluation result is %s",evaluation_result,
	      (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	HCF_CC_TRACE(Apply, "All the values in the left and right columns are equal: returning TRUE!", "Columns-Equal", "");
	return exec_operation::evaluation_return_code::EVAL_RC_TRUE;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(columns_greater,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Columns-Greater", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'columns_greater'!");
  list<SUFFIX_CONCAT(columns_greater,SUFFIX_C)> columns_greater_list = container->columns_greater_list;

	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
	for(list<SUFFIX_CONCAT(columns_greater,SUFFIX_C)>::iterator it = columns_greater_list.begin(); it != columns_greater_list.end(); ++it)
	{
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix = set_matrix.find(it->_set); //string set_name("")
		if(it_matrix ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Columns-Greater", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}

		HCF_CC_LOG(LEVEL_NOTICE, "Verified set name: (%s)",it->_set.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set:", "Columns-Greater", it->_set.c_str());

		header = it_matrix->second.first;
		matrix_str_t matrix = it_matrix->second.second;

		string column_left(it->_col_left);
		string column_right(it->_col_right);
		string percent;
		percent = (it->_percent.empty()) ? "0" : it->_percent;
		string type;
		type = (it->_type.empty()) ? "number": it->_type;
		size_t column_left_index = 0;
		size_t column_right_index = 0;

  	HCF_CC_TRACE(Apply, "The value of col_left attribute is:", "Columns-Greater", it->_col_left.c_str());
  	HCF_CC_TRACE(Apply, "The value of col_right attribute is:", "Columns-Greater", it->_col_right.c_str());
  	HCF_CC_TRACE(Apply, "The value of type attribute is:", "Columns-Greater", type.c_str());
  	HCF_CC_TRACE(Apply, "The value of percent attribute is:", "Columns-Greater", percent.c_str());

#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif

		int index_result = matrix_handler::find_header_index_by_string(header, column_left, column_left_index);
    if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Columns-Greater", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }
		index_result = matrix_handler::find_header_index_by_string(header, column_right, column_right_index);
    if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Columns-Greater", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }
		//print_matrix(matrix);

		for(size_t row=0; row<matrix.size();row++)
		  {
		    if (type == "number")
		      if(
		          (::atof((matrix[row][column_left_index]).c_str()) + PERCENTAGE( ::atof((matrix[row][column_left_index]).c_str()), ::atof(percent.c_str())))
		          >
		    (::atof((matrix[row][column_right_index]).c_str())))
		        {
		          //printf("Found number value equal at row %d!", row);
		          if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		            HCF_CC_LOG(LEVEL_NOTICE, "Found number value greater at row %zu! string %s string %s",row, (matrix[row][column_left_index]).c_str(),(matrix[row][column_right_index]).c_str());
				  	  	HCF_CC_TRACE(Apply, "Found number value in the left column greater than the right column!", "Columns-Greater", matrix[row][column_left_index].c_str());
		          }
		          continue;
		        }else{
		            if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		              HCF_CC_LOG(LEVEL_NOTICE, "Found number value NOT greater at row %zu!",row);
					  	  	HCF_CC_TRACE(Apply, "Found number value in the left column NOT greater than the right column: returning FALSE!", "Columns-Greater", matrix[row][column_left_index].c_str());
		            }
		            return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
		        }
		    else if (type == "string")
		      {
		        if((matrix[row][column_left_index] > matrix[row][column_right_index]))
		          {
		            if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		              HCF_CC_LOG(LEVEL_NOTICE, "Found string value greater at row %zu!", row);
					  	  	HCF_CC_TRACE(Apply, "Found string value in the left column greater than the right column!", "Columns-Greater", matrix[row][column_left_index].c_str());
		            }
		            continue;
		          }else{
		              if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		                HCF_CC_LOG(LEVEL_NOTICE, "Found string value NOT greater at row %zu!",row);
						  	  	HCF_CC_TRACE(Apply, "Found string value in the left column NOT greater than the right column: returning FALSE!", "Columns-Greater", matrix[row][column_left_index].c_str());
		              }
		              return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
		          }
		      }
		  }
	}
	HCF_CC_LOG(LEVEL_NOTICE,"The result is:%d the evaluation result is %s",evaluation_result,
	      (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	HCF_CC_TRACE(Apply, "All the values in the left column are greater than the values in the right columns are equal: returning TRUE!", "Columns-Greater", "");
	return exec_operation::evaluation_return_code::EVAL_RC_TRUE;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(columns_less,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Columns-Less", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'columns_less'!");
  list<SUFFIX_CONCAT(columns_less,SUFFIX_C)> columns_less_list = container->columns_less_list;

	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
	for(list<SUFFIX_CONCAT(columns_less,SUFFIX_C)>::iterator it = columns_less_list.begin(); it != columns_less_list.end(); ++it)
	{
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix = set_matrix.find(it->_set); //string set_name("")
		if(it_matrix ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Columns-Less", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}

		HCF_CC_LOG(LEVEL_NOTICE, "Verified set name: (%s)",it->_set.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set:", "Columns-Less", it->_set.c_str());

		header = it_matrix->second.first;
		matrix_str_t matrix = it_matrix->second.second;

		string column_left(it->_col_left);
		string column_right(it->_col_right);
		string percent;
		percent = (it->_percent.empty()) ? "0" : it->_percent;
		string type;
		type = (it->_type.empty()) ? "number": it->_type;
		size_t column_left_index = 0;
		size_t column_right_index = 0;

		HCF_CC_TRACE(Apply, "The value of col_left attribute is:", "Columns-Less", it->_col_left.c_str());
  	HCF_CC_TRACE(Apply, "The value of col_right attribute is:", "Columns-Less", it->_col_right.c_str());
  	HCF_CC_TRACE(Apply, "The value of type attribute is:", "Columns-Less", type.c_str());
  	HCF_CC_TRACE(Apply, "The value of percent attribute is:", "Columns-Less", percent.c_str());

#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif

		int index_result = matrix_handler::find_header_index_by_string(header, column_left, column_left_index);
    if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Columns-Less", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }
		index_result = matrix_handler::find_header_index_by_string(header, column_right, column_right_index);
    if(index_result < 0) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Columns-Less", index_result);
    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
    }

		for(size_t row=0; row<matrix.size();row++)
		  {
		    if (type == "number")
		      if((::atof((matrix[row][column_left_index]).c_str()) + PERCENTAGE(::atof((matrix[row][column_left_index]).c_str()), ::atof(percent.c_str())))
		          <
		          ::atof((matrix[row][column_right_index]).c_str()))

		        {
		          if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		            HCF_CC_LOG(LEVEL_NOTICE, "Found number value less at row %zu! string + percent %s+%f string %s",
		              row,
		              (matrix[row][column_left_index]).c_str(),
		              PERCENTAGE(::atof((matrix[row][column_right_index]).c_str()), ::atof(percent.c_str())),
		              (matrix[row][column_right_index]).c_str());
				  	  	HCF_CC_TRACE(Apply, "Found number value in the left column smaller than the right column!", "Columns-Less", matrix[row][column_left_index].c_str());
		          }
		          continue;
		        }else{
		            if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
					  	  	HCF_CC_TRACE(Apply, "Found number value in the left column NOT smaller than the right column: returning FALSE!", "Columns-Less", matrix[row][column_left_index].c_str());
		              HCF_CC_LOG(LEVEL_NOTICE, "Found number value NOT less at row %zu! string %s string %s",row, (matrix[row][column_left_index]).c_str(), (matrix[row][column_right_index]).c_str());
		            }
		            return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
		        }
		    else if (type == "string")
		      {
		        if((matrix[row][column_left_index] < matrix[row][column_right_index]))
		          {
		            if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		              HCF_CC_LOG(LEVEL_NOTICE, "Found string value less at row %zu!", row);
					  	  	HCF_CC_TRACE(Apply, "Found string value in the left column smaller than the right column!", "Columns-Less", matrix[row][column_left_index].c_str());
		            }
		            continue;
		          }else{
		              if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		                HCF_CC_LOG(LEVEL_NOTICE, "Found string equal NOT less at row %zu!",row);
						  	  	HCF_CC_TRACE(Apply, "Found string value in the left column NOT smaller than the right column: returning FALSE!", "Columns-Less", matrix[row][column_left_index].c_str());
		              }
		              return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
		          }
		      }
		  }
	}
	HCF_CC_LOG(LEVEL_NOTICE,"The result is:%d the evaluation result is %s",evaluation_result,
	      (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	HCF_CC_TRACE(Apply, "All the values in the left column are smaller than the values in the right columns are equal: returning TRUE!", "Columns-Less", "");
	return exec_operation::evaluation_return_code::EVAL_RC_TRUE;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_empty,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Is-Empty", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'is_empty'!");
  list<SUFFIX_CONCAT(is_empty,SUFFIX_C)> is_empty_list = container->is_empty_list;

	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
	for(list<SUFFIX_CONCAT(is_empty,SUFFIX_C)>::iterator it = is_empty_list.begin(); it != is_empty_list.end(); ++it)
	{
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix = set_matrix.find(it->_set); //string set_name("")
		if(it_matrix ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Is-Empty", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}

		HCF_CC_LOG(LEVEL_NOTICE, "Verified set name: (%s)",it->_set.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set:", "Is-Empty", it->_set.c_str());

		matrix_str_t matrix = it_matrix->second.second;
#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif

		if (matrix.empty())
			evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
		else{
			evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_FALSE;
			break;
		}
	}
	HCF_CC_LOG(LEVEL_NOTICE,"The result is:%d the evaluation result is %s",evaluation_result,
	    (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	HCF_CC_TRACE(Apply, "The evaluation result is the following one:", "Is-Empty", (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	return evaluation_result;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_equal,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Is-Equal", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'is_equal'!");
  list<SUFFIX_CONCAT(is_equal,SUFFIX_C)> is_equal_list = container->is_equal_list;

	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
	for(list<SUFFIX_CONCAT(is_equal,SUFFIX_C)>::iterator it = is_equal_list.begin(); it != is_equal_list.end(); ++it)
	{
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix_l = set_matrix.find(it->_set_left); //string set_name("")
		if(it_matrix_l ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Is-Equal", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}

		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix_r = set_matrix.find(it->_set_right); //string set_name("")
		if(it_matrix_r ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Is-Equal", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}

		HCF_CC_LOG(LEVEL_NOTICE, "Verified set_left name: (%s)",it->_set_left.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "Verified set_right name: (%s)",it->_set_right.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set_left:", "Is-Equal", it->_set_left.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set_right:", "Is-Equal", it->_set_right.c_str());
		matrix_str_t matrix_l = it_matrix_l->second.second;
		matrix_str_t matrix_r = it_matrix_r->second.second;

		HCF_CC_LOG(LEVEL_INFO,"LEFT");
		matrix_handler::print_matrix(matrix_l);
		HCF_CC_LOG(LEVEL_INFO,"RIGHT");
		matrix_handler::print_matrix(matrix_r);
		if (matrix_l == matrix_r)
			evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
		else{
			evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_FALSE;
			break;
		}
	}
	HCF_CC_LOG(LEVEL_NOTICE,"The result is:%d the evaluation result is %s",evaluation_result,
	      (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	HCF_CC_TRACE(Apply, "The evaluation result is the following one:", "Is-Equal", (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	return evaluation_result;

};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_in_period,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   & set_matrix, std::vector<std::string> & header,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Is-In-Period", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'is_in_period'!");
  list<SUFFIX_CONCAT(is_in_period,SUFFIX_C)> is_in_period_list = container->is_in_period_list;

  //int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
  for(list<SUFFIX_CONCAT(is_in_period,SUFFIX_C)>::iterator it = is_in_period_list.begin(); it != is_in_period_list.end(); ++it)
    {
      std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix = set_matrix.find(it->_set); //string set_name("")
      if(it_matrix ==  set_matrix.end()) {
		  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Is-In-Period", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
        return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
      }

      HCF_CC_LOG(LEVEL_NOTICE, "Verified set name: (%s)",it->_set.c_str());
  		HCF_CC_TRACE(Apply, "Working on the following set:", "Is-In-Period", it->_set.c_str());

      header = it_matrix->second.first;
      matrix_str_t matrix = it_matrix->second.second;
      size_t column_date_index = 0;
      size_t column_time_index = 0;

      if(it->_format.empty()) {
		  	HCF_CC_TRACE(ApplyErr, "Empty format string provided!", "Is-In-Period", exec_operation::evaluation_return_code::EVAL_RC_FALSE);
        return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
      }

      int index_result = matrix_handler::find_header_index_by_string(header, it->_col_date, column_date_index);
      if(index_result < 0) {
		  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Is-In-Period", index_result);
	    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
	    }

      index_result = matrix_handler::find_header_index_by_string(header, it->_col_time, column_time_index);
      if(index_result < 0) {
		  	HCF_CC_TRACE(ApplyErr, "Call 'matrix_handler::find_header_index_by_string' failed.", "Is-In-Period", index_result);
	    	return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_NOT_FOUND;
	    }

	  	HCF_CC_TRACE(Apply, "The value of format attribute is:", "Is-In-Period", it->_format.c_str());
	  	HCF_CC_TRACE(Apply, "The value of start_time attribute is:", "Is-In-Period", it->_start_time.c_str());
	  	HCF_CC_TRACE(Apply, "The value of end_time attribute is:", "Is-In-Period", it->_end_time.c_str());

      //TODO - ADD ERROR CHECK ON SET_DATE_TIME
      date_time_parser dt_parser_start;
      HCF_CC_LOG(LEVEL_INFO, "Setting START time");
      dt_parser_start.set_date_time(it->_format, it->_start_time);
      date_time_parser dt_parser_end;
      HCF_CC_LOG(LEVEL_INFO, "Setting END time");
      dt_parser_end.set_date_time(it->_format, it->_end_time);

      HCF_CC_LOG(LEVEL_INFO, "Checking VALUE time");
      for(size_t row=0; row<matrix.size();row++)
        {
          date_time_parser dt_parsed_element;
          if(column_date_index > matrix[row].size()  || column_time_index > matrix[row].size()) {
        		HCF_CC_TRACE(ApplyErr, "Columns oversize found!", "Is-In-Period", exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE);
            return exec_operation::evaluation_return_code::EVAL_RC_ERROR_INDEX_OUT_OF_RANGE;
          }

          if(it->_col_time.empty())
            {
              if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
                HCF_CC_LOG(LEVEL_INFO, "matrix date %s",(matrix[row][column_date_index]).c_str());
        	  		HCF_CC_TRACE(Apply, "The read date value is :", "Is-In-Period", (matrix[row][column_date_index]).c_str());
              }
              dt_parsed_element.set_date_time(it->_format, (matrix[row][column_date_index]));
            }
          else
            {
              if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL)
                {
                  HCF_CC_LOG(LEVEL_INFO, "matrix date %s",(matrix[row][column_date_index]).c_str());
                  HCF_CC_LOG(LEVEL_INFO, "matrix time %s",(matrix[row][column_time_index]).c_str());
          	  		HCF_CC_TRACE(Apply, "The read date value is :", "Is-In-Period", (matrix[row][column_date_index]).c_str());
          	  		HCF_CC_TRACE(Apply, "The read time value is :", "Is-In-Period", (matrix[row][column_time_index]).c_str());
                }
              dt_parsed_element.set_date_time(it->_format, (matrix[row][column_date_index]),(matrix[row][column_time_index]));
            }

          if(dt_parsed_element._date_time >= dt_parser_start._date_time &&
              dt_parsed_element._date_time <= dt_parser_end._date_time)
            {
              if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
                HCF_CC_LOG(LEVEL_NOTICE, "Found datetime value between stard/end");
        	  		HCF_CC_TRACE(Apply, "The found value is in desired the range: returning TRUE!", "Is-In-Period", "");
              }
            return exec_operation::evaluation_return_code::EVAL_RC_TRUE;
          }
        }

    }
	HCF_CC_TRACE(Apply, "No date/time value is in the desired range: returning FALSE!", "Is-In-Period", "");
  return exec_operation::evaluation_return_code::EVAL_RC_FALSE;

};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_not_empty,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Is-Not-Empty", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'is_not_empty'!");
  list<SUFFIX_CONCAT(is_not_empty,SUFFIX_C)> is_not_empty_list = container->is_not_empty_list;

	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
	for(list<SUFFIX_CONCAT(is_not_empty,SUFFIX_C)>::iterator it = is_not_empty_list.begin(); it != is_not_empty_list.end(); ++it)
	{
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix = set_matrix.find(it->_set); //string set_name("")
		if(it_matrix ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Is-Not-Empty", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}

		HCF_CC_LOG(LEVEL_NOTICE, "Verified set name: (%s)",it->_set.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set:", "Is-Not-Empty", it->_set.c_str());

		matrix_str_t matrix = it_matrix->second.second;
#ifdef PRINT_MATRIX_IN
  matrix_handler::print_matrix(matrix);
#endif

		if (!matrix.empty())
			evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
		else{
			evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_FALSE;
			break;
		}
	}
	HCF_CC_LOG(LEVEL_NOTICE,"The result is:%d the evaluation result is %s",evaluation_result,
	      (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	HCF_CC_TRACE(Apply, "The evaluation result is the following one:", "Is-Not-Empty", (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	return evaluation_result;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_not_equal,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Is-Not-Equal", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'is_not_equal'!");
  list<SUFFIX_CONCAT(is_not_equal,SUFFIX_C)> is_not_equal_list = container->is_not_equal_list;

	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
	for(list<SUFFIX_CONCAT(is_not_equal,SUFFIX_C)>::iterator it = is_not_equal_list.begin(); it != is_not_equal_list.end(); ++it)
	{
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix_l = set_matrix.find(it->_set_left); //string set_name("")
		if(it_matrix_l ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Is-Not-Equal", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix_r = set_matrix.find(it->_set_right); //string set_name("")
		if(it_matrix_r ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Is-Not-Equal", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}

		HCF_CC_LOG(LEVEL_NOTICE, "Verified set_left name: (%s)",it->_set_left.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "Verified set_right name: (%s)",it->_set_right.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set_left:", "Is-Not-Equal", it->_set_left.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set_right:", "Is-Not-Equal", it->_set_right.c_str());

		matrix_str_t matrix_l = it_matrix_l->second.second;
		matrix_str_t matrix_r = it_matrix_r->second.second;

    HCF_CC_LOG(LEVEL_INFO,"LEFT");
    matrix_handler::print_matrix(matrix_l);
    HCF_CC_LOG(LEVEL_INFO,"RIGHT");
    matrix_handler::print_matrix(matrix_r);

		if (matrix_l != matrix_r)
			evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
		else{
			evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_FALSE;
			break;
		}
	}
	HCF_CC_LOG(LEVEL_NOTICE,"The result is:%d the evaluation result is %s",evaluation_result,
	      (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	HCF_CC_TRACE(Apply, "The evaluation result is the following one:", "Is-Not-Equal", (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
	return evaluation_result;
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_subset,SUFFIX_OP)

int __CLASS_NAME__::apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >   &set_matrix, std::vector<std::string> & /*header*/,  SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * container){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Apply, "Applying filter!", "Is-Subset", "");
  HCF_CC_LOG(LEVEL_NOTICE, "Applying filter 'is_subset'!");
  list<SUFFIX_CONCAT(is_subset,SUFFIX_C)> is_subset_list = container->is_subset_list;

	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;

	for(list<SUFFIX_CONCAT(is_subset,SUFFIX_C)>::iterator it = is_subset_list.begin(); it != is_subset_list.end(); ++it)
	{
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix_subset = set_matrix.find(it->_subset); //string set_name("")
		if(it_matrix_subset ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Is-Subset", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}
		std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > ::iterator it_matrix_superset = set_matrix.find(it->_superset); //string set_name("")
		if(it_matrix_superset ==  set_matrix.end()) {
	  	HCF_CC_TRACE(ApplyErr, "Call 'set_matrix.find' failed.", "Is-Subset", exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND);
			return exec_operation::evaluation_return_code::EVAL_RC_ERROR_ELEMENT_NOT_FOUND;
		}

		HCF_CC_LOG(LEVEL_NOTICE, "Verified set_subset name: (%s)",it->_subset.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "Verified set_superset name: (%s)",it->_superset.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set_subset:", "Is-Subset", it->_subset.c_str());
		HCF_CC_TRACE(Apply, "Working on the following set_superset:", "Is-Subset", it->_superset.c_str());

		matrix_str_t matrix_subset = it_matrix_subset->second.second;
		matrix_str_t matrix_superset = it_matrix_superset->second.second;

		HCF_CC_LOG(LEVEL_INFO,"SUBSET");
    matrix_handler::print_matrix(matrix_subset);
    HCF_CC_LOG(LEVEL_INFO,"SUPERSET");
    matrix_handler::print_matrix(matrix_superset);

		matrix_str_t::const_iterator it_subset = matrix_subset.begin();
		matrix_str_t::const_iterator it_superset = matrix_superset.begin();

		size_t match=0;
		while( it_subset != matrix_subset.end())
		{
			it_superset = matrix_superset.begin();
			while(it_superset != matrix_superset.end())
			{
				if(*it_superset == *it_subset) {
					match++;
					HCF_CC_TRACE(Apply, "Found a valid match of subset into superset!", "Is-Subset", "");
				}
				++it_superset;
			}
			++it_subset;
		}

		HCF_CC_LOG(LEVEL_NOTICE,"Match:%zu",match);
		(match == matrix_subset.size()) ?
		    evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result=exec_operation::evaluation_return_code::EVAL_RC_FALSE;

		HCF_CC_LOG(LEVEL_NOTICE,"The result is:%d the evaluation result is %s",evaluation_result,
		      (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");
		HCF_CC_TRACE(Apply, "The evaluation result is the following one:", "Is-Subset", (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ? "TRUE" : "FALSE");

		if(evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE)
		  return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
	}

	return exec_operation::evaluation_return_code::EVAL_RC_TRUE;
};

#undef __CLASS_NAME__


rule_callbacks_handler::rule_callbacks_handler():
		_begin_op(), _end_op(),_ignore_op(),_header_op(),_equal_op(),_greater_op(), _less_op(),
		_match_op(),_not_equal_op(), _range_op(), _unite_op(),_intersect_op(), _and_op(), _or_op(), _not_op(), _all_in_period_op(),
		_all_in_range_op(), _column_equal_op(), _column_greater_op(), _column_less_op(), _is_empty_op(),
		_is_equal_op(), _is_in_period_op(), _is_not_empty_op(), _is_not_equal_op(), _is_subset_op(),

		//linking of functor with object to invoke
		begin_exec(&_begin_op, &parameter_operation::apply),
		end_exec(&_end_op, &parameter_operation::apply),
		ignore_exec(&_ignore_op, &parameter_operation::apply),
		header_exec(&_header_op, &parameter_operation::apply),


		equal_exec(&_equal_op, &parameter_operation::apply),
		greater_exec(&_greater_op, &parameter_operation::apply),
		less_exec(&_less_op,&parameter_operation::apply),
		match_exec(&_match_op,&parameter_operation::apply),
		not_equal_exec(&_not_equal_op,&parameter_operation::apply),
		range_exec(&_range_op,&parameter_operation::apply),

		unite_exec(&_unite_op,&parameter_operation::apply),
		intersect_exec(&_intersect_op,&parameter_operation::apply),

		and_exec(&_and_op, &parameter_operation::apply),
		or_exec(&_or_op, &parameter_operation::apply),
		not_exec(&_not_op,&parameter_operation::apply),
		all_in_period_exec(&_all_in_period_op,&parameter_operation::apply),
		all_in_range_exec(&_all_in_range_op,&parameter_operation::apply),
		column_equal_exec(&_column_equal_op,&parameter_operation::apply),
		column_greater_exec(&_column_greater_op,&parameter_operation::apply),
		column_less_exec(&_column_less_op, &parameter_operation::apply),
		is_empty_exec(&_is_empty_op, &parameter_operation::apply),
		is_equal_exec(&_is_equal_op, &parameter_operation::apply),
		is_in_period_exec(&_is_in_period_op, &parameter_operation::apply),
		is_not_empty_exec(&_is_not_empty_op, &parameter_operation::apply),
		is_not_equal_exec(&_is_not_equal_op, &parameter_operation::apply),
		is_subset_exec(&_is_subset_op, &parameter_operation::apply)
{
	HCF_CC_TRACE_FUNCTION;
	operations_list_p[exec_operation::OP_PRINTOUT_BEGIN]   = &begin_exec;
	operations_list_p[exec_operation::OP_PRINTOUT_END]     = &end_exec;
	operations_list_p[exec_operation::OP_PRINTOUT_IGNORE]  = &ignore_exec;
	operations_list_p[exec_operation::OP_PRINTOUT_HEADER]  = &header_exec;

	operations_list_f[exec_operation::OP_FILTER_EQUAL]     = &equal_exec;
	operations_list_f[exec_operation::OP_FILTER_GREATER]   = &greater_exec;
	operations_list_f[exec_operation::OP_FILTER_LESS]      = &less_exec;
	operations_list_f[exec_operation::OP_FILTER_MATCH]     = &match_exec;
	operations_list_f[exec_operation::OP_FILTER_NOT_EQUAL] = &not_equal_exec;
	operations_list_f[exec_operation::OP_FILTER_RANGE]     = &range_exec;

	operations_list_of[exec_operation::OP_ON_FILTER_UNITE ]    = &unite_exec;
	operations_list_of[exec_operation::OP_ON_FILTER_INTERSECT] = &intersect_exec;

	operations_list_e[exec_operation::OP_EVALUATION_AND ]           = &and_exec;
	operations_list_e[exec_operation::OP_EVALUATION_OR]             = &or_exec;
	operations_list_e[exec_operation::OP_EVALUATION_NOT]            = &not_exec;
	operations_list_e[exec_operation::OP_EVALUATION_ALL_IN_PERIOD ] = &all_in_period_exec;
	operations_list_e[exec_operation::OP_EVALUATION_ALL_IN_RANGE]   = &all_in_range_exec;
	operations_list_e[exec_operation::OP_EVALUATION_COLUMNS_EQUAL]  = &column_equal_exec;
	operations_list_e[exec_operation::OP_EVALUATION_COLUMNS_GREATER]= &column_greater_exec;
	operations_list_e[exec_operation::OP_EVALUATION_COLUMNS_LESS]   = &column_less_exec;
	operations_list_e[exec_operation::OP_EVALUATION_IS_EMPTY]       = &is_empty_exec;
	operations_list_e[exec_operation::OP_EVALUATION_IS_EQUAL]       = &is_equal_exec;
	operations_list_e[exec_operation::OP_EVALUATION_IS_IN_PERIOD]   = &is_in_period_exec;
	operations_list_e[exec_operation::OP_EVALUATION_IS_NOT_EMPTY]   = &is_not_empty_exec;
	operations_list_e[exec_operation::OP_EVALUATION_IS_NOT_EQUAL]   = &is_not_equal_exec;
	operations_list_e[exec_operation::OP_EVALUATION_IS_SUBSET]      = &is_subset_exec;

	HCF_CC_LOG(LEVEL_INFO,"Operation list correctly created!");
	HCF_CC_TRACE(RuleCallbacksHandler, "The operation list has been correctly created!");
}

rule_callbacks_handler::~rule_callbacks_handler() { HCF_CC_TRACE_FUNCTION; }

int rule_callbacks_handler::apply_filter(const exec_operation::printout_operation_t op, std::string & printout,std::vector<std::string> &header, SUFFIX_CONCAT(printout_parameter,SUFFIX_C) * s)
{
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ApplyFilter, "Received a request to apply a printout operation!", op);
	HCF_CC_LOG(LEVEL_NOTICE, "Calling printout operation callback == %s(%d)!", get_operation_callback_text(op),op);
  return (*operations_list_p[op])(printout,header,s);
}
int rule_callbacks_handler::apply_filter(const exec_operation::filter_operation_t op, matrix_str_t & matrix,std::vector<std::string> & header, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * s)
{
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ApplyFilter, "Received a request to apply a filter operation!", op);
	HCF_CC_LOG(LEVEL_NOTICE, "Calling filter operation callback == %s(%d)!", get_operation_callback_text(op),op);
	return (*operations_list_f[op])(matrix,header,s);
}
int rule_callbacks_handler::apply_filter(const exec_operation::on_filter_operation_t op, matrix_str_t & matrix_left,matrix_str_t & matrix_right, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * s)
{
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ApplyFilter, "Received a request to apply a filter on filter operation!", op);
  HCF_CC_LOG(LEVEL_NOTICE, "Calling on_filter operation callback == %s(%d)!", get_operation_callback_text(op),op);
  return (*operations_list_of[op])(matrix_left,matrix_right,s);
}
int rule_callbacks_handler::apply_filter(const exec_operation::evaluation_operation_t op, std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > & set_id, std::vector<std::string> &header, SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * s)
{
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ApplyFilter, "Received a request to apply an evaluation operation!", op);
	HCF_CC_LOG(LEVEL_NOTICE, "Calling evaluation operation callback == %s(%d)!", get_operation_callback_text(op),op);
	return (*operations_list_e[op])(set_id,header,s);
}
