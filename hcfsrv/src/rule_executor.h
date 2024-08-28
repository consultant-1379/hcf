/*
 * rule_executor.h
 *
 *  Created on: Mar 5, 2015
 *      Author: xgiufer
 */

#ifndef RULE_EXECUTOR_H_
#define RULE_EXECUTOR_H_


#include "app_types.h"

#include "rule_callbacks_handler.h"

using namespace std;
 class matrix_handler{
  enum MatrixHadlerError{

    MATRIX_ERROR_GENERIC = -199,
    MATRIX_ERROR_INDEX_NOT_FOUND,
    MATRIX_ERROR_NO_COLUMNS_DEFINED,
    MATRIX_ERROR_NO_HEADERS_DEFINED,
    MATRIX_ERROR_HEADER_GREATER_THEN_MATRIX,
    MATRIX_ERROR_HEADER_NOT_FOUND,

    MATRIX_ERROR_LAST_ERROR = -100,  //increment first element, to not confuse with callback error
    MATRIX_NO_ERROR = 0

  };

public:
  inline matrix_handler(){};
  inline virtual ~matrix_handler(){};


public:
  static hcf::srv::skip_log_t _skip_log;
  static int check_headers(const string printout, list<SUFFIX_CONCAT(header,SUFFIX_C)> & header_list);
  static int fetch_required_columns_by_header(SUFFIX_CONCAT(header,SUFFIX_C) & header, vector<string> & columns, matrix_str_t & matrix);
  static int extract_printout_by_header(list<SUFFIX_CONCAT(header,SUFFIX_C)> & header_list,
                                 const string full_printout,
                                 list<pair<SUFFIX_CONCAT(header,SUFFIX_C),string> > & header_printout);
  static int load_matrix(matrix_str_t * matrix, const string & printout, const size_t columns_size, const string& delim = " ");
  static int print_matrix(const matrix_str_t & matrix);
  static int find_header_index_by_string(const vector<string> & header, const string & column, size_t & index);
};



class rule_executor
{
private:

  map<string,pair< vector<string>,matrix_str_t> > _set_id_map; //first identifier==setname/column_name - second.first==matrix_header, second.second==matrix
  rule_callbacks_handler _rch;
  hcf::srv::skip_log_t _skip_log;

public:
  inline rule_executor() : _set_id_map(), _rch(), _skip_log(hcf::srv::NO_SKIP) {}
  virtual inline ~rule_executor() {}

  inline hcf::srv::skip_log_t get_skip_log () const { return _skip_log; }
  inline void set_skip_log (const hcf::srv::skip_log_t & skip_log) { _skip_log = skip_log; }

private:

  int apply_operation_on_filters(matrix_str_t & matrix, vector<string>  & header, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) & filter_cnt, const exec_operation::on_filter_operation_t on_filter_type);
  int analyze_rule_container(SUFFIX_CONCAT(parameter,SUFFIX_C) * rule_container);
  int analyze_os_container(SUFFIX_CONCAT(os,SUFFIX_C) & rule_container);
  int analyze_filter_container(matrix_str_t & matrix, vector<string>  & header,SUFFIX_CONCAT(filter,SUFFIX_C) & filter);
  int analyze_evaluation_container(SUFFIX_CONCAT(evaluation,SUFFIX_C) & eval_container);
  int analyze_evaluation_container_by_logical_operator(SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) & eval, exec_operation::evaluation_logic_t operator_l);

public:
  int evaluate_rule(SUFFIX_CONCAT(parameter,SUFFIX_C) * rule_container);

};

#endif /* RULE_EXECUTOR_H_ */
