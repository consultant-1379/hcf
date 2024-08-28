/*
 * rule_executor.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: xgiufer
 */

#include "hcf/core/logger.h"
#include "rule_executor.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_ruleexecutor.h"
#include "hcf/trace/tracing_macros.h"

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

hcf::srv::skip_log_t matrix_handler::_skip_log = hcf::srv::NO_SKIP;

int matrix_handler::check_headers(const string printout,list<SUFFIX_CONCAT(header,SUFFIX_C)> & header_list){
	HCF_CC_TRACE_FUNCTION;
  HCF_CC_LOG(LEVEL_INFO, "Analyzing header list with %zu elements",header_list.size());
	HCF_CC_TRACE(CheckHeaders, "Analyzing the header list with the following number of elements:", std::to_string(header_list.size()).c_str());
	if(header_list.empty())
	{
		HCF_CC_LOG(LEVEL_ERROR, "Error! Empty header list!");
		HCF_CC_TRACE(CheckHeadersErr, "Empty header list provided!", MATRIX_ERROR_NO_HEADERS_DEFINED);
		return MATRIX_ERROR_NO_HEADERS_DEFINED;
	}
	string line;
	string word;
	list<SUFFIX_CONCAT(header,SUFFIX_C)>  new_header_list;
	int level_one_match = 0;

	for (list<SUFFIX_CONCAT(header,SUFFIX_C)>::iterator it=header_list.begin(); it != header_list.end(); ++it)
	  {
	    if(it->_headerPresence == "false")
	      {
	        HCF_CC_LOG(LEVEL_INFO, "Header presence = false. Considering fake header!");
	      	HCF_CC_TRACE(CheckHeaders, "Header presence attribute has FALSE value, using fake header!", "");
	        new_header_list.push_back(*it);
	      }else { //true or not defined

	          if(it->_level == "1")
	            {
	  	      		HCF_CC_TRACE(CheckHeaders, "First level header found!", "");
	              level_one_match = 0; // reset level 1
	            }else if(it->_level != "1" && level_one_match)
	              {
	                HCF_CC_LOG(LEVEL_INFO, "The level 1 already has been found and it match! This header will be skipped");
		  	      		HCF_CC_TRACE(CheckHeaders, "The first level header has been already found, skipping this one!", "");
	                continue;
	              }
	          HCF_CC_LOG(LEVEL_INFO, "Analyzing header with level %s",it->_level.c_str());
	      		HCF_CC_TRACE(CheckHeaders, "Analyzing the header with the following level:", it->_level.c_str());
	          std::istringstream is(printout);
	          vector<SUFFIX_CONCAT(header,SUFFIX_C)::column>::iterator c = it->_columns.begin();
	          HCF_CC_LOG(LEVEL_INFO, "Proceeding to discover in printout the column:%s",c->_value.c_str());
	      		HCF_CC_TRACE(CheckHeaders, "Discovering the printout in the following column:", c->_value.c_str());
	          while(getline(is,line,'\n'))
	            {
	              if(line.empty()) continue;
	              if(line.find(c->_value) != string::npos)
	                {
	                  int header_match = 0;
	                  size_t pos = 0;
	                  string token;
	                  size_t columns_match = 0;
	                  HCF_CC_LOG(LEVEL_INFO, "Good. Found the first column...It will be checked if all other match");
	      	      		HCF_CC_TRACE(CheckHeaders, "First column found: checking if all the other match!", "");
	                  while ((c != it->_columns.end()) && ((pos = line.find(c->_value, pos)) != std::string::npos)) {
	                      token = line.substr(pos, pos + c->_value.size());
	                      columns_match++;
	                      ++c;
	                  }

	                  HCF_CC_LOG(LEVEL_INFO, "The number of columns expected is:%zu, the number of columns read is:%zu",it->_columns.size(),columns_match);
	                  if (columns_match == it->_columns.size()) {
		      	      		HCF_CC_TRACE(CheckHeaders, "All the expected columns matched in the printout!", "");
	                    header_match = 1;
	                  }
	                  if(header_match) {level_one_match = 1;new_header_list.push_back(*it);}
	                  HCF_CC_LOG(LEVEL_INFO, "The header%s match completely!",(header_match) ? "": "DO NOT");
	      	      		HCF_CC_TRACE(CheckHeaders, "The header mathcing result is:", (header_match) ? "COMPLETE MATCH" : "PARTIAL MATCH");
	                  line.clear();
	                  break;
	                }
	            }
	      }
	  }

	if (new_header_list.empty()) {
		// No header matched the provided printout: log and return error!
		HCF_CC_LOG(LEVEL_ERROR, "The provided printout doesn't contain any header!");
		HCF_CC_TRACE(CheckHeadersErr, "The provided printout doesn't contain any header!", MATRIX_ERROR_HEADER_NOT_FOUND);
		return MATRIX_ERROR_HEADER_NOT_FOUND;
	}

	header_list.clear();
	header_list = new_header_list;
	return MATRIX_NO_ERROR;
}
int matrix_handler::fetch_required_columns_by_header(SUFFIX_CONCAT(header,SUFFIX_C) & header, vector<string> & columns, matrix_str_t & matrix){
	HCF_CC_TRACE_FUNCTION;
  HCF_CC_LOG(LEVEL_NOTICE, "Fetching the column 'required' and resizing the matrix");
	HCF_CC_TRACE(CheckRequiredColumnsByHeader, "Fetching the required columns from printout: the matrix has the number of columns:", std::to_string(header._columns.size()).c_str());
	if (header._columns.empty())
	{
		HCF_CC_LOG(LEVEL_WARNING, "Error! Header has no columns defined!");
		HCF_CC_TRACE(CheckRequiredColumnsByHeaderErr, "The provided header hasn't columns!", MATRIX_ERROR_NO_COLUMNS_DEFINED);
		return MATRIX_ERROR_NO_COLUMNS_DEFINED;
	}
	size_t index = 0; //first column
	for(vector<SUFFIX_CONCAT(header,SUFFIX_C)::column>::iterator it  = header._columns.begin(); it != header._columns.end(); ++it ) {
	    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
	      HCF_CC_LOG(LEVEL_NOTICE, "The column %s (%s)", it->_value.c_str(), it->_required.c_str());
	    	HCF_CC_TRACE(CheckRequiredColumnsByHeader, "Working with the following column:", it->_value.c_str());
	    	HCF_CC_TRACE(CheckRequiredColumnsByHeader, "This required value for this column is:", it->_required.c_str());
	    }
		if(it->_required == "true")
			{
    		HCF_CC_TRACE(CheckRequiredColumnsByHeader, "The column is required, adding to the list!", "");
		    columns.push_back(it->_value);
		    index++;
			}
		else {
			HCF_CC_LOG(LEVEL_NOTICE, "Found column not required! It will be discarded!");
  		HCF_CC_TRACE(CheckRequiredColumnsByHeader, "The column is NOT required, skipping it!", "");
			for (std::vector< std::vector<std::string> >::iterator it_elem = matrix.begin(); it_elem != matrix.end(); ++it_elem)
			  {
			    HCF_CC_LOG(LEVEL_INFO, "Erasing element at index (%zu) of column: %s in matrix with %zu elements\n", index, it->_value.c_str(), it_elem->size() );
					HCF_CC_TRACE(CheckRequiredColumnsByHeader, "Erasing column for each row in the printout!", "");
			    it_elem->erase(it_elem->begin() + index);
			  }
			}

	}
	if(columns.size() == 0) {	// if all columns are set as false
		HCF_CC_TRACE(CheckRequiredColumnsByHeaderErr, "No columns were found!", MATRIX_ERROR_NO_COLUMNS_DEFINED);
	  return MATRIX_ERROR_NO_COLUMNS_DEFINED;
	}

	return MATRIX_NO_ERROR;
}
int matrix_handler::extract_printout_by_header(list<SUFFIX_CONCAT(header,SUFFIX_C)> & header_list,
		const string full_printout,
		list<pair<SUFFIX_CONCAT(header,SUFFIX_C),string> > & header_printout)
{
	HCF_CC_TRACE_FUNCTION;
	int found = 0;
	string printout;
	std::istringstream is(full_printout);
	string line;
	HCF_CC_LOG(LEVEL_INFO, "Trying to extract the printout referred to each header. Header list of %zu elements",header_list.size());
	HCF_CC_TRACE(ExtractPrintoutByHeader, "Trying to extract the printout referred to each header. There is the following number of headers:", std::to_string(header_list.size()).c_str());

	for (list<SUFFIX_CONCAT(header,SUFFIX_C)>::iterator it = header_list.begin(), next = it; it != header_list.end(); ++it)
	  {
	    int maxlines = (it->_maxlines.empty()) ? -1 : ::atoi(it->_maxlines.c_str());
	  	HCF_CC_TRACE(ExtractPrintoutByHeader, "The value of maxlines attribute is:", it->_maxlines.c_str());

	    if(it->_headerPresence == "false")
	      {
	        HCF_CC_LOG(LEVEL_INFO, "There is no need to extract header because it's a fake header");
	  	  	HCF_CC_TRACE(ExtractPrintoutByHeader, "There is no need to extract header because it's a fake header", "");
	        if(it->_maxlines.empty()) {
	    	  	HCF_CC_TRACE(ExtractPrintoutByHeader, "Maxlines attribute is empty, considering entire printout!", "");
	          printout = full_printout;
	        }
	        else{
	        		HCF_CC_TRACE(ExtractPrintoutByHeader, "Maxlines attribute is not empty, taking only some printout lines!", "");
	            while(getline(is,line,'\n') && (maxlines--) != 0 )
	              {
	                printout.append(line + "\n");
	              }
	        }
	      }else{
	          found = 0;
	          vector<SUFFIX_CONCAT(header,SUFFIX_C)::column>::iterator c = it->_columns.begin();
	          if(++next != header_list.end()) {
	            HCF_CC_LOG(LEVEL_INFO, "Next header:%s",(next)->_columns.begin()->_value.c_str());
	        		HCF_CC_TRACE(ExtractPrintoutByHeader, "The next header is:", (next)->_columns.begin()->_value.c_str());
	          }

	          while(getline(is,line,'\n'))
	            {
	              if(line.empty()){
	                  if(maxlines-- != 0)
	                    continue;
	                  else
	                    break;
	              }

	              if(line.find(c->_value + it->_colSeparator) != string::npos)
	                {
	                  HCF_CC_LOG(LEVEL_INFO, "Discovered the header line in the printout!");
	    	        		HCF_CC_TRACE(ExtractPrintoutByHeader, "Header line found in the printout!", "");
	                  found = 1;
	                  continue;
	                }else if (found)
	                  {
	                    if(next != header_list.end()) {
	                      HCF_CC_LOG(LEVEL_INFO, "Next header:%s",(next)->_columns.begin()->_value.c_str());
	        	        		HCF_CC_TRACE(ExtractPrintoutByHeader, "The next header is:", (next)->_columns.begin()->_value.c_str());
	                    }
	                    if((next != header_list.end()) && line.find((next)->_columns.begin()->_value) != string::npos)
	                      {
	                        HCF_CC_LOG(LEVEL_INFO, "Found next header...skipping and breaking");
		        	        		HCF_CC_TRACE(ExtractPrintoutByHeader, "Found next header, skipping and breaking!", "");
	                        break;
	                      }else if ((maxlines--) != 0)
	                        {
	                          printout.append(line + "\n");
	                        }
	                      else
	                        {
	                          HCF_CC_LOG(LEVEL_INFO, "Maxlines reached! No need to catch others line.");
			        	        		HCF_CC_TRACE(ExtractPrintoutByHeader, "Maxlines reached, no other lines will be caught!", "");
	                          break;
	                        }
	                  }
	            }
	      }
	    //insert into pair
	    pair<SUFFIX_CONCAT(header,SUFFIX_C),string> header_pr_checked;
	    header_pr_checked.first = *it;
	    header_pr_checked.second = printout;
	    printout.clear();
	    header_printout.push_back(header_pr_checked);
	  }
	return MATRIX_NO_ERROR;
}

int matrix_handler::load_matrix(matrix_str_t * matrix, const string & printout, const size_t columns_size, const string& delim){
	HCF_CC_TRACE_FUNCTION;
	string      line;
	string      word;

	// clear first
	matrix->clear();
	std::istringstream is(printout);

	// parse line by line
	HCF_CC_LOG(LEVEL_INFO, "The matrix will have %zu columns", columns_size);
	HCF_CC_TRACE(LoadMatrix, "The matrix will have the following number of columns:", std::to_string(columns_size).c_str());

	int l = 0;
	while (getline(is, line,'\n'))
	{
   if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
      HCF_CC_LOG(LEVEL_INFO, "line %d): %s",l++, line.c_str());
  		HCF_CC_TRACE(LoadMatrix, "Working on the following line:", line.c_str());
   }
		matrix->push_back(vector<string>());

		for (string::const_iterator i = line.begin(); i != line.end(); ++ i)
		  {
		    //If is not printable
//		    if(!isprint(*i))


		    // If i is not a delim, then append it to word
		    if (delim.find(*i) == string::npos)
		      {
		        if(*i != 13)
		          word += *i;
		        if (i + 1 != line.end()) // If it's the last char, do not continue
		          continue;
		      }
		    // if word is still empty, it means the previous char is also a
		    // delim (several delims appear together). Ignore this char.
		    if (word.empty())
		      continue;

		    if(matrix_handler::_skip_log != hcf::srv::SKIP_PRINTOUT && matrix_handler::_skip_log != hcf::srv::SKIP_ALL) {
		  		HCF_CC_TRACE(LoadMatrix, "Working on the following word:", word.c_str());
		      HCF_CC_LOG(LEVEL_INFO, "%s",word.c_str());
		    }

		    if(matrix->back().size() < columns_size)
		      {
		  			HCF_CC_TRACE(LoadMatrix, "Adding a new column to the matrix!", word.c_str());
		        matrix->back().push_back(word);
		        word.clear();
		      }else if (matrix->back().size() >= columns_size )
		        {
		          HCF_CC_LOG(LEVEL_INFO, "Warning! Maximum columns size reached!... Appending all next words at the last column.");

		          if(matrix->back().size() == 0) { //last column
				  			HCF_CC_TRACE(LoadMatrix, "This is the first word of the last column!", "");
		            matrix->back().push_back(word);
		          }
		          else {// append to the last
				  			HCF_CC_TRACE(LoadMatrix, "This isn't the the first word of the last column: appending also a delimiter character!", "");
		            matrix->back().back() += (delim + word);
		          }
		          word.clear();
		        }
		  }
		if(matrix->back().size() == 0)
		  matrix->pop_back();
		else if(matrix->back().size() < columns_size)
		  {
		    HCF_CC_LOG(LEVEL_ERROR, "Error! Header size is greater then matrix size, invalid header definition");
				HCF_CC_TRACE(LoadMatrixErr, "Header size is greater than matrix size, invalid header definition!", MATRIX_ERROR_HEADER_GREATER_THEN_MATRIX);
		    return MATRIX_ERROR_HEADER_GREATER_THEN_MATRIX;
		  }
	}

	return MATRIX_NO_ERROR;
}
int matrix_handler::print_matrix(const vector< vector<string> > & matrix){
	HCF_CC_TRACE_FUNCTION;
  HCF_CC_LOG(LEVEL_WARNING, "skip log set to %d ",matrix_handler::_skip_log);
  HCF_CC_TRACE(PrintMatrix, "The value of skipLog attribute is:", std::to_string(matrix_handler::_skip_log).c_str());

  if(matrix_handler::_skip_log == hcf::srv::SKIP_PRINTOUT || matrix_handler::_skip_log == hcf::srv::SKIP_ALL)
    return MATRIX_NO_ERROR;

  HCF_CC_LOG(LEVEL_INFO, "**************************************************");
  HCF_CC_LOG(LEVEL_INFO, "*         The Matrix is (SIZE == %zu)            *", matrix.size());
  HCF_CC_LOG(LEVEL_INFO, "**************************************************");
  HCF_CC_TRACE(PrintMatrix, "**************************************************", "");
  HCF_CC_TRACE(PrintMatrix, "*           The Matrix is (SIZE is)              *", std::to_string(matrix.size()).c_str());
  HCF_CC_TRACE(PrintMatrix, "**************************************************", "");

	for (std::vector< std::vector<std::string> >::const_iterator it = matrix.begin(); it != matrix.end(); ++ it)
	{
		HCF_CC_LOG(LEVEL_INFO, "The number of columns to print is: %zu", it->size());
	  HCF_CC_TRACE(PrintMatrix, "The number of columns to print is:", std::to_string(it->size()).c_str());

		for (std::vector<std::string>::const_iterator itit = it->begin(); itit != it->end(); ++ itit) {
			HCF_CC_LOG(LEVEL_INFO, "C%02zu) |%s|",std::distance(it->begin(),itit), itit->c_str());
		  HCF_CC_TRACE(PrintMatrix, "", itit->c_str());
		}
	}
	HCF_CC_LOG(LEVEL_INFO, "**************************************************");
	HCF_CC_LOG(LEVEL_INFO, "**************************************************");
  HCF_CC_TRACE(PrintMatrix, "**************************************************", "");
  HCF_CC_TRACE(PrintMatrix, "**************************************************", "");
	return MATRIX_NO_ERROR;
}
int matrix_handler::find_header_index_by_string(const vector<string> & header, const string & column, size_t & index){
	HCF_CC_TRACE_FUNCTION;

	if (column.empty()) {
		HCF_CC_LOG(LEVEL_WARNING, "Column empty");
		HCF_CC_TRACE(FindHeaderIndexByStringErr, "The provided column string is empty!", MATRIX_ERROR_INDEX_NOT_FOUND);
		return MATRIX_ERROR_INDEX_NOT_FOUND;
	}

	HCF_CC_LOG(LEVEL_INFO, "Searching index of header");
	HCF_CC_TRACE(FindHeaderIndexByString, "Looking for the index in the header for the provided column!", "");

	for(size_t i=0; i < header.size(); i++) {
		if (column.compare(header[i]) == 0) {
			HCF_CC_LOG(LEVEL_INFO, "Found column in the defined header at position %lu!", (index = i));
			HCF_CC_TRACE(FindHeaderIndexByString, "The requested column has the following index:", std::to_string(i).c_str());
			return MATRIX_NO_ERROR;
		}
	}

	HCF_CC_LOG(LEVEL_INFO, "Error! Header position not found!");
	HCF_CC_TRACE(FindHeaderIndexByStringErr, "Header position not found!", MATRIX_ERROR_INDEX_NOT_FOUND);
	return MATRIX_ERROR_INDEX_NOT_FOUND;

}

int rule_executor::apply_operation_on_filters(matrix_str_t & matrix, vector<string>  & header, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) & filter_cnt, const exec_operation::on_filter_operation_t on_filter_type){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ApplyOperationOnFilters, "Received a request to apply the following operation on filters:", on_filter_type);

  vector<matrix_str_t> unite_matrices;
  vector<matrix_str_t> intersect_matrices;

  //for loop - on every list
  for(list<SUFFIX_CONCAT(equal,SUFFIX_C)>::iterator it_filter = filter_cnt.equal_list.begin(); it_filter != filter_cnt.equal_list.end(); ++it_filter)
    {
      HCF_CC_LOG(LEVEL_NOTICE, "EQUAL");
    	HCF_CC_TRACE(ApplyOperationOnFilters, "Applying all the EQUAL filters!", on_filter_type);
    	SUFFIX_CONCAT(filter,SUFFIX_C) one_filter;
      one_filter.filter_parameter._rule_attrs = filter_cnt._rule_attrs;
      one_filter.filter_parameter.equal_list.push_back(*it_filter); //save element on temporary container
      matrix_str_t matrix_tmp = matrix;
      if(analyze_filter_container(matrix_tmp, header, one_filter)!=0) {
      	HCF_CC_TRACE(ApplyOperationOnFiltersErr, "Call 'analyze_filter_container' failed.", -1);
      	return -1;
      }
      (on_filter_type == exec_operation::OP_ON_FILTER_UNITE) ? unite_matrices.push_back(matrix_tmp) : intersect_matrices.push_back(matrix_tmp); //save into vector of matrix
    }
  for(list<SUFFIX_CONCAT(greater,SUFFIX_C)>::iterator it_filter = filter_cnt.greater_list.begin(); it_filter != filter_cnt.greater_list.end(); ++it_filter)
    {
      HCF_CC_LOG(LEVEL_NOTICE, "GREATER");
    	HCF_CC_TRACE(ApplyOperationOnFilters, "Applying all the GREATER filters!", on_filter_type);
      SUFFIX_CONCAT(filter,SUFFIX_C) one_filter;
      one_filter.filter_parameter._rule_attrs = filter_cnt._rule_attrs;
      one_filter.filter_parameter.greater_list.push_back(*it_filter); //save element on temporary container
      matrix_str_t matrix_tmp = matrix;
      if(analyze_filter_container(matrix_tmp, header, one_filter)!=0) {
      	HCF_CC_TRACE(ApplyOperationOnFiltersErr, "Call 'analyze_filter_container' failed.", -1);
      	return -1;
      }
      (on_filter_type == exec_operation::OP_ON_FILTER_UNITE) ? unite_matrices.push_back(matrix_tmp) : intersect_matrices.push_back(matrix_tmp); //save into vector of matrix
    }
  for(list<SUFFIX_CONCAT(less,SUFFIX_C)>::iterator it_filter = filter_cnt.less_list.begin(); it_filter != filter_cnt.less_list.end(); ++it_filter)
    {
      HCF_CC_LOG(LEVEL_NOTICE, "LESS");
    	HCF_CC_TRACE(ApplyOperationOnFilters, "Applying all the LESS filters!", on_filter_type);
      SUFFIX_CONCAT(filter,SUFFIX_C) one_filter;
      one_filter.filter_parameter._rule_attrs = filter_cnt._rule_attrs;
      one_filter.filter_parameter.less_list.push_back(*it_filter); //save element on temporary container
      matrix_str_t matrix_tmp = matrix;
      if(analyze_filter_container(matrix_tmp, header, one_filter)!=0) {
      	HCF_CC_TRACE(ApplyOperationOnFiltersErr, "Call 'analyze_filter_container' failed.", -1);
      	return -1;
      }
      (on_filter_type == exec_operation::OP_ON_FILTER_UNITE) ? unite_matrices.push_back(matrix_tmp) : intersect_matrices.push_back(matrix_tmp); //save into vector of matrix
    }
  for(list<SUFFIX_CONCAT(match,SUFFIX_C)>::iterator it_filter = filter_cnt.match_list.begin(); it_filter != filter_cnt.match_list.end(); ++it_filter)
    {
      HCF_CC_LOG(LEVEL_NOTICE, "MATCH");
    	HCF_CC_TRACE(ApplyOperationOnFilters, "Applying all the MATCH filters!", on_filter_type);
      SUFFIX_CONCAT(filter,SUFFIX_C) one_filter;
      one_filter.filter_parameter._rule_attrs = filter_cnt._rule_attrs;
      one_filter.filter_parameter.match_list.push_back(*it_filter); //save element on temporary container
      matrix_str_t matrix_tmp = matrix;
      if(analyze_filter_container(matrix_tmp, header, one_filter)!=0) {
      	HCF_CC_TRACE(ApplyOperationOnFiltersErr, "Call 'analyze_filter_container' failed.", -1);
      	return -1;
      }
      (on_filter_type == exec_operation::OP_ON_FILTER_UNITE) ? unite_matrices.push_back(matrix_tmp) : intersect_matrices.push_back(matrix_tmp); //save into vector of matrix
    }
  for(list<SUFFIX_CONCAT(not_equal,SUFFIX_C)>::iterator it_filter = filter_cnt.not_equal_list.begin(); it_filter != filter_cnt.not_equal_list.end(); ++it_filter)
    {
      HCF_CC_LOG(LEVEL_NOTICE, "NOT_EQUAL");
    	HCF_CC_TRACE(ApplyOperationOnFilters, "Applying all the NOT EQUAL filters!", on_filter_type);
      SUFFIX_CONCAT(filter,SUFFIX_C) one_filter;
      one_filter.filter_parameter._rule_attrs = filter_cnt._rule_attrs;
      one_filter.filter_parameter.not_equal_list.push_back(*it_filter); //save element on temporary container
      matrix_str_t matrix_tmp = matrix;
      if(analyze_filter_container(matrix_tmp, header, one_filter)!=0) {
      	HCF_CC_TRACE(ApplyOperationOnFiltersErr, "Call 'analyze_filter_container' failed.", -1);
      	return -1;
      }
      (on_filter_type == exec_operation::OP_ON_FILTER_UNITE) ? unite_matrices.push_back(matrix_tmp) : intersect_matrices.push_back(matrix_tmp); //save into vector of matrix
    }
  for(list<SUFFIX_CONCAT(range,SUFFIX_C)>::iterator it_filter = filter_cnt.range_list.begin(); it_filter != filter_cnt.range_list.end(); ++it_filter)
    {
      HCF_CC_LOG(LEVEL_NOTICE, "RANGE");
    	HCF_CC_TRACE(ApplyOperationOnFilters, "Applying all the RANGE filters!", on_filter_type);
      SUFFIX_CONCAT(filter,SUFFIX_C) one_filter;
      one_filter.filter_parameter._rule_attrs = filter_cnt._rule_attrs;
      one_filter.filter_parameter.range_list.push_back(*it_filter); //save element on temporary container
      matrix_str_t matrix_tmp = matrix;
      if(analyze_filter_container(matrix_tmp, header, one_filter)!=0) {
      	HCF_CC_TRACE(ApplyOperationOnFiltersErr, "Call 'analyze_filter_container' failed.", -1);
      	return -1;
      }
      (on_filter_type == exec_operation::OP_ON_FILTER_UNITE) ? unite_matrices.push_back(matrix_tmp) : intersect_matrices.push_back(matrix_tmp); //save into vector of matrix
    }

  //apply all intersection
  int filtering_result = 0;
  matrix_str_t unite_matrix;
  matrix_str_t intersect_matrix;

	HCF_CC_TRACE(ApplyOperationOnFilters, "After applying all the filters, check if the results must be united or intersected!", on_filter_type);

  if (on_filter_type == exec_operation::OP_ON_FILTER_UNITE){
      if(unite_matrices.size() < 2) //at least 2 elements
        {
          HCF_CC_LOG(LEVEL_NOTICE, "Error! It cannot be possible to unite only one matrix ");
      		HCF_CC_TRACE(ApplyOperationOnFiltersErr, "Cannot unite only one matrix!", -1);
          return -1;
        }
      unite_matrix = unite_matrices[0];
      for(size_t i=1; i< unite_matrices.size(); ++i){ //start from second element
          filtering_result = _rch.apply_filter(exec_operation::OP_ON_FILTER_UNITE, unite_matrix, unite_matrices[i], &filter_cnt);
          if (filtering_result != 0) {
          	HCF_CC_TRACE(ApplyOperationOnFiltersErr, "Call 'apply_filter' failed.", filtering_result);
          	return filtering_result;
          }
      }
      matrix = unite_matrix;
  }else if (on_filter_type == exec_operation::OP_ON_FILTER_INTERSECT){
      if(intersect_matrices.size() < 2)
        {
          HCF_CC_LOG(LEVEL_NOTICE, "Error! It cannot be possible to intersect only one matrix ");
          HCF_CC_LOG(LEVEL_NOTICE, "Error! It cannot be possible to intersect only one matrix ");
          return -1;
        }
      intersect_matrix = intersect_matrices[0];
      for(size_t i=1; i< intersect_matrices.size(); ++i){ //start from second element
          filtering_result = _rch.apply_filter(exec_operation::OP_ON_FILTER_INTERSECT, intersect_matrix, intersect_matrices[i], &filter_cnt);
          if (filtering_result != 0) {
          	HCF_CC_TRACE(ApplyOperationOnFiltersErr, "Call 'apply_filter' failed.", filtering_result);
          	return filtering_result;
          }
      }
      matrix = intersect_matrix;
  }

  return 0;
}

int rule_executor::analyze_os_container(SUFFIX_CONCAT(os,SUFFIX_C) & os_cnt){
	HCF_CC_TRACE_FUNCTION;

  vector<string> header;
  matrix_str_t matrix;

	HCF_CC_TRACE(AnalyzeOSContainer, "Analyzing the rule OS container: applying BEGIN operation (if any).");
	if(!os_cnt.printout.begin_c._value.empty())
    _rch.apply_filter(exec_operation::OP_PRINTOUT_BEGIN, os_cnt.command_printout, header, &os_cnt.printout );

	HCF_CC_TRACE(AnalyzeOSContainer, "Analyzing the rule OS container: applying ERROR operations (if any).");
  if(!os_cnt.printout.end_c._error_list.empty() || !os_cnt.printout.end_c._normal.empty())
    if(_rch.apply_filter(exec_operation::OP_PRINTOUT_END, os_cnt.command_printout, header, &os_cnt.printout )) {
    	HCF_CC_TRACE(AnalyzeOSContainer, "Call 'apply_filter(END)' failed, returning FALSE!");
      return exec_operation::evaluation_return_code::EVAL_RC_FALSE;
    }

	HCF_CC_TRACE(AnalyzeOSContainer, "Analyzing the rule OS container: applying IGNORE operations (if any).");
  if(!os_cnt.printout.ignore_list.empty())
    _rch.apply_filter(exec_operation::OP_PRINTOUT_IGNORE,os_cnt.command_printout, header, &os_cnt.printout );

  HCF_CC_LOG(LEVEL_NOTICE, "Trying to understand the kind of printout...");
	HCF_CC_TRACE(AnalyzeOSContainer, "Checking if the printout is row-based or column-based.");
  if(os_cnt.printout.header_list.empty())
    {
      // For a row based, put as header the PRINTOUT result name
      header.push_back(os_cnt.printout.result);
      HCF_CC_LOG(LEVEL_NOTICE, "ROW based printout detected");
    	HCF_CC_TRACE(AnalyzeOSContainer, "ROW-based printout detected!");
      matrix_handler::load_matrix(&matrix, os_cnt.command_printout, 0); //matrix row based with only one column

    }else
      {
        HCF_CC_LOG(LEVEL_NOTICE, "COLUMN based printout detected");
      	HCF_CC_TRACE(AnalyzeOSContainer, "COLUMN-based printout detected!");
        //verify only valid header (by level)
        list<pair<SUFFIX_CONCAT(header,SUFFIX_C),string> > _header_printout;
        if(matrix_handler::check_headers(os_cnt.command_printout, os_cnt.printout.header_list) < 0) {
        	HCF_CC_TRACE(AnalyzeOSContainer, "Call 'check_headers' failed, returning ERROR!");
          return exec_operation::evaluation_return_code::EVAL_RC_ERROR_HEADER_CHECK_FAILED;
        }

        //extract all printout for each header found
      	HCF_CC_TRACE(AnalyzeOSContainer, "Extracting header from the printout!");
        matrix_handler::extract_printout_by_header(os_cnt.printout.header_list,os_cnt.command_printout, _header_printout);

        //--------------------------
        //TODO - HANDLE MORE HEADERS
        //--------------------------

        for(list<pair<SUFFIX_CONCAT(header,SUFFIX_C),string> >::iterator header_it = _header_printout.begin();
            header_it != _header_printout.end(); ++header_it )
          {
            string separator = (header_it->first._colSeparator.empty() ? " " : header_it->first._colSeparator);
          	HCF_CC_TRACE(AnalyzeOSContainer, "Loading the matrix using the extracted header!");

          	if(matrix_handler::load_matrix(&matrix, header_it->second, header_it->first._columns.size(),separator)) {
            	HCF_CC_TRACE(AnalyzeOSContainer, "Call 'load_matrix' failed, returning ERROR!");
              return exec_operation::evaluation_return_code::EVAL_RC_ERROR_LOADING_MATRIX;
          	}

          	//resize matrix if needed
          	HCF_CC_TRACE(AnalyzeOSContainer, "Resizing matrix, erasing not required columns!");
            if(matrix_handler::fetch_required_columns_by_header(header_it->first, header, matrix) < 0)
              {
                HCF_CC_LOG(LEVEL_ERROR,"Error! Problem fetching columns by header.");
              	HCF_CC_TRACE(AnalyzeOSContainer, "Call 'fetch_required_columns_by_header' failed, returning ERROR!");
                return exec_operation::evaluation_return_code::EVAL_RC_ERROR_IN_FETCH_REQUIRED_COLUMN;
              }

						HCF_CC_LOG(LEVEL_NOTICE,"Printing the matrix result after parsing of printout");
          	HCF_CC_TRACE(AnalyzeOSContainer, "Matrix correctly loaded and resized!");
						matrix_handler::print_matrix(matrix);

            //store matrix
            for(vector<string>::iterator it = header.begin(); it != header.end(); ++it){
                _set_id_map[*it] = pair<vector<string>, matrix_str_t>(header, matrix);
            }
          }
      }

  // Put into SET IDs map the PRINTOUT result, in order to use it later
  _set_id_map[os_cnt.printout.result] = pair<vector<string>, matrix_str_t>(header, matrix);
	HCF_CC_TRACE(AnalyzeOSContainer, "Initials operations correctly executed, executing filtering operations (if any)!");

  //analyizing filter
  for(std::list<SUFFIX_CONCAT(filter,SUFFIX_C)> ::iterator filter_it = os_cnt.filter_list.begin();
      filter_it != os_cnt.filter_list.end(); ++filter_it ){
      matrix_str_t matrix_tmp = matrix; // to avoid modification of primary matrix
      if(analyze_filter_container(matrix_tmp, header, *filter_it) != 0) {
      	HCF_CC_TRACE(AnalyzeOSContainer, "Call 'analyze_filter_container' failed, returning ERROR!");
        return exec_operation::evaluation_return_code::EVAL_RC_ERROR_IN_ANALYSIS_OF_FILTERS;
      }
      else {
				HCF_CC_LOG(LEVEL_NOTICE, "Good. The filter result will be saved in order to be evaluated");
				HCF_CC_TRACE(AnalyzeOSContainer, "Filtering operation correctly executed, saving its result!");
				_set_id_map[filter_it->result] = pair<vector<string>, matrix_str_t>(header, matrix_tmp);
      }
  }
  return exec_operation::evaluation_return_code::EVAL_RC_TRUE;
}
int rule_executor::analyze_filter_container(matrix_str_t & matrix, vector<string> & header, SUFFIX_CONCAT(filter,SUFFIX_C) & filter_cnt){
	HCF_CC_TRACE_FUNCTION;

	int filtering_result = 0;
	HCF_CC_LOG(LEVEL_NOTICE, "Analyzing the filter container. The filters to apply will be chosen!");
	HCF_CC_TRACE(AnalyzeFilterContainer, "Applying the EQUAL filters (if any)!");

	if(!filter_cnt.filter_parameter.equal_list.empty())
		filtering_result = _rch.apply_filter(exec_operation::OP_FILTER_EQUAL, matrix, header , &filter_cnt.filter_parameter);
	if (filtering_result != 0) {
		HCF_CC_TRACE(AnalyzeFilterContainerErr, "Call 'apply_filter(EQUAL)' failed", filtering_result);
		return filtering_result;
	}

	HCF_CC_TRACE(AnalyzeFilterContainer, "Applying the GREATER filters (if any)!");
	if(!filter_cnt.filter_parameter.greater_list.empty())
		filtering_result = _rch.apply_filter(exec_operation::OP_FILTER_GREATER, matrix, header , &filter_cnt.filter_parameter);
	if (filtering_result != 0) {
		HCF_CC_TRACE(AnalyzeFilterContainerErr, "Call 'apply_filter(GREATER)' failed", filtering_result);
		return filtering_result;
	}

	HCF_CC_TRACE(AnalyzeFilterContainer, "Applying the LESS filters (if any)!");
	if(!filter_cnt.filter_parameter.less_list.empty())
		filtering_result = _rch.apply_filter(exec_operation::OP_FILTER_LESS, matrix, header , &filter_cnt.filter_parameter);
	if (filtering_result != 0) {
		HCF_CC_TRACE(AnalyzeFilterContainerErr, "Call 'apply_filter(LESS)' failed", filtering_result);
		return filtering_result;
	}

	HCF_CC_TRACE(AnalyzeFilterContainer, "Applying the MATCH filters (if any)!");
	if(!filter_cnt.filter_parameter.match_list.empty())
		filtering_result = _rch.apply_filter(exec_operation::OP_FILTER_MATCH, matrix, header , &filter_cnt.filter_parameter);
	if (filtering_result != 0) {
		HCF_CC_TRACE(AnalyzeFilterContainerErr, "Call 'apply_filter(MATCH)' failed", filtering_result);
		return filtering_result;
	}

	HCF_CC_TRACE(AnalyzeFilterContainer, "Applying the NOT-EQUAL filters (if any)!");
	if(!filter_cnt.filter_parameter.not_equal_list.empty())
		filtering_result = _rch.apply_filter(exec_operation::OP_FILTER_NOT_EQUAL, matrix, header , &filter_cnt.filter_parameter);
	if (filtering_result != 0) {
		HCF_CC_TRACE(AnalyzeFilterContainerErr, "Call 'apply_filter(NOT-EQUAL)' failed", filtering_result);
		return filtering_result;
	}

	HCF_CC_TRACE(AnalyzeFilterContainer, "Applying the RANGE filters (if any)!");
	if(!filter_cnt.filter_parameter.range_list.empty())
		filtering_result = _rch.apply_filter(exec_operation::OP_FILTER_RANGE, matrix, header , &filter_cnt.filter_parameter);
	if (filtering_result != 0) {
		HCF_CC_TRACE(AnalyzeFilterContainerErr, "Call 'apply_filter(RANGE)' failed", filtering_result);
		return filtering_result;
	}

	//exec op on filter
	if(!filter_cnt.unite_list.empty() ){
	    HCF_CC_LOG(LEVEL_NOTICE, "Applying the UNITE operation on filter");
	  	HCF_CC_TRACE(AnalyzeFilterContainer, "Applying the UNITE operation on filters!");
	    for(list<SUFFIX_CONCAT(filter_parameter,SUFFIX_C)>::iterator it = filter_cnt.unite_list.begin(); it != filter_cnt.unite_list.end(); ++it)
	      if((filtering_result = apply_operation_on_filters(matrix, header, *it, exec_operation::OP_ON_FILTER_UNITE)) != 0) {
	    		HCF_CC_TRACE(AnalyzeFilterContainerErr, "Call 'apply_operation_on_filters(UNITE)' failed", filtering_result);
	      	return  filtering_result;
	      }
	}else if(!filter_cnt.intersect_list.empty()){
	    HCF_CC_LOG(LEVEL_NOTICE, "Applying the INTERSECT operation on filter");
	  	HCF_CC_TRACE(AnalyzeFilterContainer, "Applying the INTERSECT operation on filters!");
	    for(list<SUFFIX_CONCAT(filter_parameter,SUFFIX_C)>::iterator it = filter_cnt.intersect_list.begin(); it != filter_cnt.intersect_list.end(); ++it)
	      if((filtering_result = apply_operation_on_filters(matrix, header, *it, exec_operation::OP_ON_FILTER_INTERSECT)) != 0) {
	    		HCF_CC_TRACE(AnalyzeFilterContainerErr, "Call 'apply_operation_on_filters(INTERSECT)' failed", filtering_result);
	      	return  filtering_result;
	      }
	}

	return filtering_result;
}

int rule_executor::analyze_evaluation_container_by_logical_operator(SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) & eval_container, exec_operation::evaluation_logic_t operator_l)
{
	HCF_CC_TRACE_FUNCTION;

  int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_ERROR_NO_EVALUATION_DONE;
  vector<string> header;

  //Evaluating all elements of OPERATOR list
  HCF_CC_LOG(LEVEL_NOTICE, "Executing all evaluations by OPERATOR(%d) list",operator_l);
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Executing all the evaluations using the following logical operator:", operator_l);

  //ALL IN PERIOD
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying ALL-IN-PERIOD evaluations (if any) with the following operator:", operator_l);
  if(!eval_container.all_in_period_list.empty())
    {
      for(list<SUFFIX_CONCAT(all_in_period,SUFFIX_C)>::iterator it = eval_container.all_in_period_list.begin(); it != eval_container.all_in_period_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.all_in_period_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying ALL-IN-PERIOD evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_ALL_IN_PERIOD, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break; // it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
    }
  //ALL IN RANGE
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying ALL-IN-RANGE evaluations (if any) with the following operator:", operator_l);
  if(!eval_container.all_in_range_list.empty()){
      for(list<SUFFIX_CONCAT(all_in_range,SUFFIX_C)>::iterator it = eval_container.all_in_range_list.begin(); it != eval_container.all_in_range_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.all_in_range_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying ALL-IN-RANGE evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_ALL_IN_RANGE, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }
  //COLUMN EQUAL
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying COLUMNS-EQUAL evaluations (if any) with the following operator:", operator_l);
  if(!eval_container.columns_equal_list.empty()){
      for(list<SUFFIX_CONCAT(columns_equal,SUFFIX_C)>::iterator it = eval_container.columns_equal_list.begin(); it != eval_container.columns_equal_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.columns_equal_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying COLUMNS-EQUAL evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_COLUMNS_EQUAL, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }
  //COLUMN GREATER
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying COLUMNS-GREATER evaluations (if any) with the following operator:", operator_l);
  if(!eval_container.columns_greater_list.empty()){
      for(list<SUFFIX_CONCAT(columns_greater,SUFFIX_C)>::iterator it = eval_container.columns_greater_list.begin(); it != eval_container.columns_greater_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.columns_greater_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying COLUMNS-GREATER evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_COLUMNS_GREATER, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }
  //COLUMN LESS
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying COLUMNS-LESS evaluations (if any) with the following operator:", operator_l);
  if(!eval_container.columns_less_list.empty()){
      for(list<SUFFIX_CONCAT(columns_less,SUFFIX_C)>::iterator it = eval_container.columns_less_list.begin(); it != eval_container.columns_less_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.columns_less_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying COLUMNS-LESS evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_COLUMNS_LESS, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }
  //IS EMPTY
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-EMPTY evaluations (if any) with the following operator:", operator_l);
  if(!eval_container.is_empty_list.empty()){
      for(list<SUFFIX_CONCAT(is_empty,SUFFIX_C)>::iterator it = eval_container.is_empty_list.begin(); it != eval_container.is_empty_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.is_empty_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-EMPTY evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_IS_EMPTY, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }
  //IS EQUAL
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-EQUAL evaluations (if any) with the following operator:", operator_l);
  if(!eval_container.is_equal_list.empty()){
      for(list<SUFFIX_CONCAT(is_equal,SUFFIX_C)>::iterator it = eval_container.is_equal_list.begin(); it != eval_container.is_equal_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.is_equal_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-EQUAL evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_IS_EQUAL, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }
  //IS IN PERIOD
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-IN-PERIOD evaluations (if any) with the following operator:", operator_l);
  if(!eval_container.is_in_period_list.empty()){
      for(list<SUFFIX_CONCAT(is_in_period,SUFFIX_C)>::iterator it = eval_container.is_in_period_list.begin(); it != eval_container.is_in_period_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.is_in_period_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-IN-PERIOD evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_IS_IN_PERIOD, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }
  //IS NOT EMPTY
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-NOT-EMPTYevaluations (if any) with the following operator:", operator_l);
  if(!eval_container.is_not_empty_list.empty()){
      for(list<SUFFIX_CONCAT(is_not_empty,SUFFIX_C)>::iterator it = eval_container.is_not_empty_list.begin(); it != eval_container.is_not_empty_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.is_not_empty_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-NOT-EMPTY evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_IS_NOT_EMPTY, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }
  //IS NOT EQUAL
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-NOT-EQUAL (if any) with the following operator:", operator_l);
  if(!eval_container.is_not_equal_list.empty()){
      for(list<SUFFIX_CONCAT(is_not_equal,SUFFIX_C)>::iterator it = eval_container.is_not_equal_list.begin(); it != eval_container.is_not_equal_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.is_not_equal_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-NOT-EQUAL evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_IS_NOT_EQUAL, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }
  //IS SUBSET
  HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-SUBSET (if any) with the following operator:", operator_l);
  if(!eval_container.is_subset_list.empty()){
      for(list<SUFFIX_CONCAT(is_subset,SUFFIX_C)>::iterator it = eval_container.is_subset_list.begin(); it != eval_container.is_subset_list.end(); ++it)
        {
          SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) eval_container_tmp;
          eval_container_tmp.is_subset_list.push_back(*it);

          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "Applying IS-SUBSET evaluation with the following operator:", operator_l);
          evaluation_result = _rch.apply_filter(exec_operation::OP_EVALUATION_IS_SUBSET, _set_id_map, header , &eval_container_tmp);
          HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The evaluation result is:", evaluation_result);
          switch (operator_l){
          case exec_operation::OP_EVAL_DEFAULT :
          case exec_operation::OP_EVAL_AND :
            if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) return  evaluation_result;
            break;
          case exec_operation::OP_EVAL_OR :
            if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_FALSE) return  evaluation_result;
            break;// it returns only with TRUE/FAILURE
          case exec_operation::OP_EVAL_NOT :
            return (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE) ?
                exec_operation::evaluation_return_code::EVAL_RC_FALSE : (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) ?
                    exec_operation::evaluation_return_code::EVAL_RC_TRUE : evaluation_result;
          };
        }
  }

  //In order to distinguish the result false-failure it's needed to return only result
  if (evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_ERROR_NO_EVALUATION_DONE) {
    HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The result of all evaluations is:", evaluation_result);
    HCF_CC_LOG(LEVEL_NOTICE, "All evaluation has been done! return code: (%d)",evaluation_result);
  }
  else {
    HCF_CC_TRACE(AnalyzeEvaluationContainerByLogicalOperator, "The result of all evaluations is, no more operations need to be executed:", evaluation_result);
    HCF_CC_LOG(LEVEL_NOTICE, "No more evaluation needed to be done!");
  }

  return evaluation_result;
}

int rule_executor::analyze_evaluation_container(SUFFIX_CONCAT(evaluation,SUFFIX_C) & eval){
	HCF_CC_TRACE_FUNCTION;

  vector<string> header;
  int evaluation_done = 0;
  HCF_CC_LOG(LEVEL_NOTICE, "EVALUATING the filter result! AND(%zu), OR(%zu), NOT(%zu)",eval.and_list.size(),eval.or_list.size(),eval.not_list.size() );

  //AND LIST
  HCF_CC_TRACE(AnalyzeEvaluationContainer, "Executing all the evaluations following the AND logic. The number of evaluations is:", eval.and_list.size());
  if(!eval.and_list.empty()){
      int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_ERROR_GENERIC_ERROR;
      evaluation_done++;
      HCF_CC_LOG(LEVEL_NOTICE, "EVALUATION using AND operator!");
      for(std::list<SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C)>::iterator it_op = eval.and_list.begin(); it_op != eval.and_list.end(); ++it_op)
        if((evaluation_result = (analyze_evaluation_container_by_logical_operator(*it_op, exec_operation::OP_EVAL_AND)))
            != exec_operation::evaluation_return_code::EVAL_RC_TRUE)
          {//log error
            HCF_CC_LOG(LEVEL_NOTICE, "the EVALUATION using AND operator is FALSE/FAILURE! return code:(%d)", evaluation_result);
            HCF_CC_TRACE(AnalyzeEvaluationContainer, "The return value of AND operator is FALSE/FAILURE:", evaluation_result);
            return evaluation_result;
          }
  }
  //OR LIST
  HCF_CC_TRACE(AnalyzeEvaluationContainer, "Executing all the evaluations following the OR logic. The number of evaluations is:", eval.or_list.size());
  if(!eval.or_list.empty()){
      int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_ERROR_GENERIC_ERROR;
      evaluation_done++;
      HCF_CC_LOG(LEVEL_NOTICE, "EVALUATION using OR operator!");
      for(std::list<SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C)>::iterator it_op = eval.or_list.begin(); it_op != eval.or_list.end(); ++it_op)
        {
          if((evaluation_result = (analyze_evaluation_container_by_logical_operator(*it_op, exec_operation::OP_EVAL_OR)))
              != exec_operation::evaluation_return_code::EVAL_RC_TRUE)
            {//log error
              HCF_CC_LOG(LEVEL_NOTICE, "the EVALUATION using OR operator is FALSE/FAILURE! return code:(%d)", evaluation_result);
              HCF_CC_TRACE(AnalyzeEvaluationContainer, "The return value of OR operator is FALSE/FAILURE:", evaluation_result);
              return evaluation_result;
            }
        }
  }
  //NOT LIST
  HCF_CC_TRACE(AnalyzeEvaluationContainer, "Executing all the evaluations following the NOT logic. The number of evaluations is:", eval.not_list.size());
  if(!eval.not_list.empty()){
      int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_ERROR_GENERIC_ERROR;
      evaluation_done++;
      HCF_CC_LOG(LEVEL_NOTICE, "EVALUATION using NOT operator!");
      for(std::list<SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C)>::iterator it_op = eval.not_list.begin(); it_op != eval.not_list.end(); ++it_op)
        if((evaluation_result = (analyze_evaluation_container_by_logical_operator(*it_op, exec_operation::OP_EVAL_NOT)))
            != exec_operation::evaluation_return_code::EVAL_RC_TRUE)
          {//log error
            HCF_CC_LOG(LEVEL_NOTICE, "the EVALUATION using NOT operator is FALSE/FAILURE! return code:(%d)", evaluation_result);
            HCF_CC_TRACE(AnalyzeEvaluationContainer, "The return value of NOT operator is FALSE/FAILURE:", evaluation_result);
            return evaluation_result;
          }
  }
  //DEFAULT LIST
  //analize without logical operator, all evaluation are in AND by default.
  //THE DEFAULT list it ever will be analyzed
  HCF_CC_TRACE(AnalyzeEvaluationContainer, "Executing all the evaluations following the DEFAULT(AND) logic.", 0);
  int evaluation_result = analyze_evaluation_container_by_logical_operator(eval.evaluation_parmater, exec_operation::OP_EVAL_DEFAULT);
      if(evaluation_done && (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_ERROR_NO_EVALUATION_DONE))
        return exec_operation::evaluation_return_code::EVAL_RC_TRUE;
      else if(evaluation_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE)
    {//log error
      HCF_CC_LOG(LEVEL_NOTICE, "the EVALUATION using DEFAULT/EMPTY operator is FALSE/FAILURE! return code:(%d)", evaluation_result);
      HCF_CC_TRACE(AnalyzeEvaluationContainer, "The return value of DEFAULT(AND) operator is FALSE/FAILURE:", evaluation_result);
      return evaluation_result;
    }

  return exec_operation::evaluation_return_code::EVAL_RC_TRUE;
}


int rule_executor::analyze_rule_container(SUFFIX_CONCAT(parameter,SUFFIX_C) * rule_container){
	HCF_CC_TRACE_FUNCTION;

	if (rule_container->os_list.empty()) {
		HCF_CC_LOG(LEVEL_WARNING, "Error OS tag not correctly parsed or incomplete!");
		HCF_CC_TRACE(AnalyzeRuleContainerErr, "The provided rule container hasn't OS elements!", exec_operation::evaluation_return_code::EVAL_RC_ERROR_OS_LIST_EMPTY);
		return exec_operation::evaluation_return_code::EVAL_RC_ERROR_OS_LIST_EMPTY;
	}

	for(std::list<SUFFIX_CONCAT(os,SUFFIX_C)>::iterator it_os = rule_container->os_list.begin(); it_os != rule_container->os_list.end(); ++it_os) {
		if (get_skip_log() != hcf::srv::SKIP_PRINTOUT && get_skip_log() != hcf::srv::SKIP_ALL) {
			HCF_CC_LOG(LEVEL_NOTICE, "Evaluating the printout:\n%s", it_os->command_printout.c_str());
			HCF_CC_TRACE(AnalyzeRuleContainer, "Evaluating the following printout: ", it_os->command_printout.c_str());
		}

		HCF_CC_TRACE(AnalyzeRuleContainer, "Analyzing the OS container!", "");
		int os_analysis_result = analyze_os_container(*it_os);
		if (os_analysis_result != exec_operation::evaluation_return_code::EVAL_RC_TRUE) {
			HCF_CC_LOG(LEVEL_ERROR, "Error: Analysis of OS tag failed! - OS analysis result: %d",os_analysis_result);
			HCF_CC_TRACE(AnalyzeRuleContainerErr, "Call 'analyze_os_container' failed.", os_analysis_result);
			return os_analysis_result;
		}
	}

	HCF_CC_TRACE(AnalyzeRuleContainer, "After analyzing the OS container(s), execute the evaluation section!", "");
	int evaluation_result = exec_operation::evaluation_return_code::EVAL_RC_TRUE;
	evaluation_result = analyze_evaluation_container(rule_container->evaluation);
	HCF_CC_TRACE(AnalyzeRuleContainer, "The result of rule evaluation is:", std::to_string(evaluation_result).c_str());
	switch(evaluation_result)
	{
	case exec_operation::evaluation_return_code::EVAL_RC_TRUE:
		HCF_CC_LOG(LEVEL_NOTICE, "GREAT: The EVALUATION result is TRUE!");
		break;
	case exec_operation::evaluation_return_code::EVAL_RC_FALSE:
		HCF_CC_LOG(LEVEL_NOTICE, "BAD news: The EVALUATION result is FALSE!");
		break;
	default:
		HCF_CC_LOG(LEVEL_ERROR, "ERROR: The EVALUATION of rules is failed! error:(%d)",evaluation_result);
		break;

	}
	return evaluation_result;

}

int rule_executor::evaluate_rule(SUFFIX_CONCAT(parameter,SUFFIX_C) * rule_container){
	HCF_CC_TRACE_FUNCTION;
  matrix_handler::_skip_log = get_skip_log();
	HCF_CC_TRACE(EvaluateRule, "Received a request to evaluate the rule. The value of skipLog attribute is:", matrix_handler::_skip_log);
	int result_evaluation = analyze_rule_container(rule_container);
	HCF_CC_LOG(LEVEL_NOTICE, "Analysis of rule container result:%d",result_evaluation);
	HCF_CC_TRACE(EvaluateRule, "The rule evaluation result is:", result_evaluation);
	return result_evaluation;
}
