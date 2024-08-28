/*
 * filter_command.h
 *
 *  Created on: Jan 26, 2015
 *      Author: xgiufer
 */

#ifndef RULE_CALLBACK_HANDLER_H_
#define RULE_CALLBACK_HANDLER_H_

#include <stdio.h>
#include <istream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "hcf/core/logger.h"
#include "ruleset_file_containers.h"

#define SUFFIX_C _container
#define SUFFIX_OP _operation
#define SUFFIX_CONCAT(x,y) SUFFIX_CONCAT_IMPL(x,y)
#define SUFFIX_CONCAT_IMPL(x, y) x ## y
#define PERCENTAGE(number, percent) ((number)*(percent)/100.0)

#define __TMPL_CLASS_NAME__ operation_executor
#define CLASS_NAME_OP(n) SUFFIX_CONCAT(n, SUFFIX_OP)
#define __METHOD_P(MTH,P,H,C) int MTH(std::string& P, std::vector<std::string> & H,SUFFIX_CONCAT(printout_parameter,SUFFIX_C) *C);
#define __METHOD_F(MTH,M,H,C) int MTH(matrix_str_t & M, std::vector<std::string> & H, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) *C);
#define __METHOD_OF(MTH,M,H,C) int MTH(matrix_str_t & M, matrix_str_t &  H, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) *C);
#define __METHOD_E(MTH,M,H,C) int MTH(std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> >  & M, std::vector<std::string> & H, SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) *C);



#define DECLARE_CLASS_OP_P(n,p) \
		class CLASS_NAME_OP(n) :public p \
		{\
		public:\
		inline CLASS_NAME_OP(n)(){};\
		inline virtual  ~CLASS_NAME_OP(n)(){};\
		__METHOD_P(apply,printout,header,container)\
		};

#define DECLARE_CLASS_OP_F(n,p) \
		class CLASS_NAME_OP(n) :public p \
		{\
		public:\
		inline CLASS_NAME_OP(n)(){};\
		inline virtual  ~CLASS_NAME_OP(n)(){};\
		__METHOD_F(apply,matrix,header,container)\
		};

#define DECLARE_CLASS_OP_OF(n,p) \
    class CLASS_NAME_OP(n) :public p \
    {\
    public:\
    inline CLASS_NAME_OP(n)(){};\
    inline virtual  ~CLASS_NAME_OP(n)(){};\
    __METHOD_OF(apply,matrix_l,matrix_r,container)\
    };

#define DECLARE_CLASS_OP_E(n,p) \
		class CLASS_NAME_OP(n) :public p \
		{\
		public:\
		inline CLASS_NAME_OP(n)(){};\
		inline virtual  ~CLASS_NAME_OP(n)(){};\
		__METHOD_E(apply,matrix,header,container)\
		};

typedef std::vector< std::vector<std::string> > matrix_str_t;

namespace exec_operation {
  typedef enum PrintoutOperation {
    OP_PRINTOUT_SIZE_MIN        = 0,

    OP_PRINTOUT_BEGIN      = 0,
    OP_PRINTOUT_END           ,
    OP_PRINTOUT_IGNORE        ,
    OP_PRINTOUT_HEADER        ,      //4 elements

    OP_PRINTOUT_SIZE_MAX,
    OP_PRINTOUT_SIZE = OP_PRINTOUT_SIZE_MAX - OP_PRINTOUT_SIZE_MIN

  } printout_operation_t;

  typedef enum FilterOperation {
    OP_FILTER_SIZE_MIN           = 0,

    OP_FILTER_EQUAL         = OP_FILTER_SIZE_MIN,
    OP_FILTER_GREATER         ,
    OP_FILTER_LESS            ,
    OP_FILTER_MATCH           ,
    OP_FILTER_NOT_EQUAL       ,
    OP_FILTER_RANGE           ,//6 elements


    OP_FILTER_SIZE_MAX,
    OP_FILTER_SIZE                   = OP_FILTER_SIZE_MAX - OP_FILTER_SIZE_MIN

  } filter_operation_t;


  typedef enum OnFilterOperation {
    OP_ON_FILTER_SIZE_MIN     = 0,
    OP_ON_FILTER_INTERSECT    = OP_ON_FILTER_SIZE_MIN,
    OP_ON_FILTER_UNITE        ,

    OP_ON_FILTER_SIZE_MAX,
    OP_ON_FILTER_SIZE                = OP_ON_FILTER_SIZE_MAX - OP_ON_FILTER_SIZE_MIN

  } on_filter_operation_t;

  typedef enum EvaluationOperation {
    OP_EVALUATION_SIZE_MIN           = 0,

    OP_EVALUATION_AND                = OP_EVALUATION_SIZE_MIN,
    OP_EVALUATION_OR              ,
    OP_EVALUATION_NOT             ,
    OP_EVALUATION_ALL_IN_PERIOD   ,
    OP_EVALUATION_ALL_IN_RANGE    ,
    OP_EVALUATION_COLUMNS_EQUAL   ,
    OP_EVALUATION_COLUMNS_GREATER ,
    OP_EVALUATION_COLUMNS_LESS    ,
    OP_EVALUATION_IS_EMPTY        ,
    OP_EVALUATION_IS_EQUAL        ,
    OP_EVALUATION_IS_IN_PERIOD    ,
    OP_EVALUATION_IS_NOT_EMPTY    ,
    OP_EVALUATION_IS_NOT_EQUAL    ,
    OP_EVALUATION_IS_SUBSET       , //14 elements

    OP_EVALUATION_SIZE_MAX             ,
    OP_EVALUATION_SIZE                   = OP_EVALUATION_SIZE_MAX - OP_EVALUATION_SIZE_MIN,

    OP_TOTAL_SIZE = OP_PRINTOUT_SIZE + OP_FILTER_SIZE + OP_EVALUATION_SIZE
  } evaluation_operation_t;

  typedef enum EvaluationLogic {
    OP_EVAL_DEFAULT  = 0,
    OP_EVAL_AND,
    OP_EVAL_OR,
    OP_EVAL_NOT

  }evaluation_logic_t;

  namespace evaluation_return_code {
     enum EvaluationReturnCode {
      EVAL_RC_ERROR_ELEMENT_NOT_FOUND = -99,
      EVAL_RC_ERROR_INDEX_NOT_FOUND,
      EVAL_RC_ERROR_INDEX_OUT_OF_RANGE,
      EVAL_RC_ERROR_NO_EVALUATION_DONE,
      EVAL_RC_ERROR_OS_LIST_EMPTY,
      EVAL_RC_ERROR_END_ERROR_FILTER_MATCH,
      EVAL_RC_ERROR_HEADER_CHECK_FAILED,
      EVAL_RC_ERROR_IN_FETCH_REQUIRED_COLUMN,
      EVAL_RC_ERROR_IN_ANALYSIS_OF_FILTERS,
      EVAL_RC_ERROR_LOADING_MATRIX,

      EVAL_RC_ERROR_WITH_AND_DEFAULT,
      EVAL_RC_ERROR_WITH_AND_OPERATOR,
      EVAL_RC_ERROR_WITH_OR_OPERATOR,
      EVAL_RC_ERROR_WITH_NOT_OPERATOR,
      EVAL_RC_ERROR_GENERIC_ERROR = -1,
      EVAL_RC_FALSE = 0,
      EVAL_RC_TRUE = 1

    };

  };
}


template <class class_name, typename return_type,typename input_printout, typename input_header, typename parameter >
class __TMPL_CLASS_NAME__
{
public:

  typedef return_type (class_name::*method)(input_printout, input_header,parameter);
  __TMPL_CLASS_NAME__(class_name* _class_instance, method _method)
  : class_instance_(_class_instance),
    method_(_method)
  {}

  return_type operator()(input_printout _printout, input_header _header, parameter _parameter)
  {
    return (class_instance_->*method_)(_printout, _header, _parameter);
  }

  return_type execute(input_printout _printout, input_header _header,parameter _parameter)
  {
    return operator()(_printout, _header, _parameter);
  }

private:

  class_name*  class_instance_;
  method  method_;
};

#undef __ROOT_CLASS_NAME__
#define __ROOT_CLASS_NAME__ SUFFIX_CONCAT(parameter,SUFFIX_OP)

class __ROOT_CLASS_NAME__
{
public:
  //=============//
  // Constructor //
  //============//
  inline __ROOT_CLASS_NAME__(){};

  //============//
  // Destructor //
  //============//
  inline virtual ~__ROOT_CLASS_NAME__(){};
  inline virtual int apply( std::string& /*pr*/, std::vector<std::string> & /*header*/, SUFFIX_CONCAT(printout_parameter,SUFFIX_C) * /*container*/) {
  	return 0;
  }
  inline virtual int apply( matrix_str_t& /*pr*/, std::vector<std::string> & /*header*/, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * /*container*/) {
  	return 0;
  }
  inline virtual int apply( matrix_str_t& /*left*/, matrix_str_t & /*right*/, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * /*container*/) {
    return 0;
  }
  inline virtual int apply( std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > & /*set_matrix*/, std::vector<std::string> &/* header*/, SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * /*container*/) {
  	return 0;
  }
};

/************************************************************/
/*             PRINTOUT CONTAINER SET                         */
/************************************************************/
#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(printout_parameter,SUFFIX_OP)
DECLARE_CLASS_OP_P(printout_parameter,__ROOT_CLASS_NAME__)

/***************/
/*   BEGIN     */
/***************/
DECLARE_CLASS_OP_P(begin,__BASE_CLASS_NAME__)
/***************/
/*   END       */
/***************/
DECLARE_CLASS_OP_P(end,__BASE_CLASS_NAME__)
/***************/
/*   IGNORE    */
/***************/
DECLARE_CLASS_OP_P(ignore,__BASE_CLASS_NAME__)
/***************/
/*   HEADER    */
/***************/
DECLARE_CLASS_OP_P(header,__BASE_CLASS_NAME__)

/************************************************************/
/*             FILTER CONTAINER SET                         */
/************************************************************/
#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(filter_parameter,SUFFIX_OP)
class __BASE_CLASS_NAME__ :public parameter_operation \
{
public:
  inline __BASE_CLASS_NAME__(){};
  inline virtual  ~__BASE_CLASS_NAME__(){};
  int apply(std::vector< std::vector<std::string> >& matrix, std::vector<std::string> & header, filter_parameter_container *container);
};
/***************/
/*   EQUAL     */
/***************/
DECLARE_CLASS_OP_F(equal,__BASE_CLASS_NAME__)

/***************/
/*   GREATER   */
/***************/
DECLARE_CLASS_OP_F(greater,__BASE_CLASS_NAME__)

/***************/
/*    LESS     */
/***************/
DECLARE_CLASS_OP_F(less,__BASE_CLASS_NAME__)

/***************/
/*    MATCH    */
/***************/
DECLARE_CLASS_OP_F(match,__BASE_CLASS_NAME__)

/*****************/
/*   NOT-EQUAL   */
/*****************/
DECLARE_CLASS_OP_F(not_equal,__BASE_CLASS_NAME__)

/***************/
/*   RANGE     */
/***************/
DECLARE_CLASS_OP_F(range,__BASE_CLASS_NAME__)

/***************/
/*    UNITE    */
/***************/
DECLARE_CLASS_OP_OF(unite,__BASE_CLASS_NAME__)

/***************/
/*  INTERSECT  */
/***************/
DECLARE_CLASS_OP_OF(intersect,__BASE_CLASS_NAME__)

/************************************************************/
/*             EVALUATION CONTAINER SET                     */
/************************************************************/
#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(evaluation_parameter,SUFFIX_OP)
DECLARE_CLASS_OP_E(evaluation_parameter,__ROOT_CLASS_NAME__)

/***************/
/*   AND       */
/***************/
DECLARE_CLASS_OP_E(and,__BASE_CLASS_NAME__)

/***************/
/*   OR       */
/***************/
DECLARE_CLASS_OP_E(or,__BASE_CLASS_NAME__)

/***************/
/*   NOT       */
/***************/
DECLARE_CLASS_OP_E(not,__BASE_CLASS_NAME__)

/*********************/
/*   ALL-IN-PERIOD   */
/*********************/
DECLARE_CLASS_OP_E(all_in_period,__BASE_CLASS_NAME__)

/*********************/
/*   IS-IN-PERIOD    */
/*********************/
DECLARE_CLASS_OP_E(is_in_period,__BASE_CLASS_NAME__)

/*********************/
/*   ALL-IN-RANGE    */
/*********************/
DECLARE_CLASS_OP_E(all_in_range,__BASE_CLASS_NAME__)

/*********************/
/*   COLUMN-EQUAL    */
/*********************/
DECLARE_CLASS_OP_E(columns_equal,__BASE_CLASS_NAME__)

/************************/
/*   COLUMN-GREATER     */
/************************/
DECLARE_CLASS_OP_E(columns_greater,__BASE_CLASS_NAME__)

/********************/
/*   COLUMN-LESS    */
/********************/
DECLARE_CLASS_OP_E(columns_less,__BASE_CLASS_NAME__)

/******************/
/*   IS-EMPTY     */
/******************/
DECLARE_CLASS_OP_E(is_empty,__BASE_CLASS_NAME__)

/**********************/
/*   IS-NOT-EMPTY     */
/**********************/
DECLARE_CLASS_OP_E(is_not_empty,__BASE_CLASS_NAME__)

/******************/
/*   IS-EQUAL     */
/******************/
DECLARE_CLASS_OP_E(is_equal,__BASE_CLASS_NAME__)

/*********************/
/*   IS-NOT-EQUAL    */
/*********************/
DECLARE_CLASS_OP_E(is_not_equal,__BASE_CLASS_NAME__)

/******************/
/*   IS-SUBSET     */
/******************/
DECLARE_CLASS_OP_E(is_subset,__BASE_CLASS_NAME__)


typedef __TMPL_CLASS_NAME__ <SUFFIX_CONCAT(parameter,SUFFIX_OP)  , int, std::string &, std::vector<std::string> &, SUFFIX_CONCAT(printout_parameter,SUFFIX_C) * > executor_p;
typedef __TMPL_CLASS_NAME__ <SUFFIX_CONCAT(parameter,SUFFIX_OP)  , int, matrix_str_t &, std::vector<std::string> &, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * > executor_f;
typedef __TMPL_CLASS_NAME__ <SUFFIX_CONCAT(parameter,SUFFIX_OP)  , int, matrix_str_t &, matrix_str_t &, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * > executor_of;
typedef __TMPL_CLASS_NAME__ <SUFFIX_CONCAT(parameter,SUFFIX_OP)  , int, std::map<std::string,std::pair< std::vector<std::string>,matrix_str_t> > &, std::vector<std::string> &, SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * > executor_e;


class rule_callbacks_handler
{

public:
  rule_callbacks_handler();
  virtual ~rule_callbacks_handler();

private:
  //printout operations
  SUFFIX_CONCAT(begin,SUFFIX_OP)                   _begin_op;
  SUFFIX_CONCAT(end,SUFFIX_OP)                       _end_op;
  SUFFIX_CONCAT(ignore,SUFFIX_OP)                 _ignore_op;
  SUFFIX_CONCAT(header,SUFFIX_OP)                 _header_op;
  //filter operations
  SUFFIX_CONCAT(equal,SUFFIX_OP)                   _equal_op;
  SUFFIX_CONCAT(greater,SUFFIX_OP)               _greater_op;
  SUFFIX_CONCAT(less,SUFFIX_OP)                     _less_op;
  SUFFIX_CONCAT(match,SUFFIX_OP)                   _match_op;
  SUFFIX_CONCAT(not_equal,SUFFIX_OP)           _not_equal_op;
  SUFFIX_CONCAT(range,SUFFIX_OP)                   _range_op;
  //on filter operations
  SUFFIX_CONCAT(unite,SUFFIX_OP)                   _unite_op;
  SUFFIX_CONCAT(intersect,SUFFIX_OP)           _intersect_op;
  //evaluation operations
  SUFFIX_CONCAT(and,SUFFIX_OP)                       _and_op;
  SUFFIX_CONCAT(or,SUFFIX_OP)                         _or_op;
  SUFFIX_CONCAT(not,SUFFIX_OP)                       _not_op;
  SUFFIX_CONCAT(all_in_period,SUFFIX_OP)   _all_in_period_op;
  SUFFIX_CONCAT(all_in_range,SUFFIX_OP)     _all_in_range_op;
  SUFFIX_CONCAT(columns_equal,SUFFIX_OP)     _column_equal_op;
  SUFFIX_CONCAT(columns_greater,SUFFIX_OP) _column_greater_op;
  SUFFIX_CONCAT(columns_less,SUFFIX_OP)       _column_less_op;
  SUFFIX_CONCAT(is_empty,SUFFIX_OP)             _is_empty_op;
  SUFFIX_CONCAT(is_equal,SUFFIX_OP)             _is_equal_op;
  SUFFIX_CONCAT(is_in_period,SUFFIX_OP)     _is_in_period_op;
  SUFFIX_CONCAT(is_not_empty,SUFFIX_OP)     _is_not_empty_op;
  SUFFIX_CONCAT(is_not_equal,SUFFIX_OP)     _is_not_equal_op;
  SUFFIX_CONCAT(is_subset,SUFFIX_OP)           _is_subset_op;

  //  //printout operations executors
  executor_p             begin_exec;
  executor_p               end_exec;
  executor_p            ignore_exec;
  executor_p            header_exec;

  //filter operations executors
  executor_f             equal_exec;
  executor_f           greater_exec;
  executor_f              less_exec;
  executor_f             match_exec;
  executor_f         not_equal_exec;
  executor_f             range_exec;

  executor_of             unite_exec;
  executor_of         intersect_exec;

  //evaluatfon operations executors
  executor_e               and_exec;
  executor_e                or_exec;
  executor_e               not_exec;
  executor_e     all_in_period_exec;
  executor_e      all_in_range_exec;
  executor_e      column_equal_exec;
  executor_e    column_greater_exec;
  executor_e       column_less_exec;
  executor_e          is_empty_exec;
  executor_e          is_equal_exec;
  executor_e      is_in_period_exec;
  executor_e      is_not_empty_exec;
  executor_e      is_not_equal_exec;
  executor_e         is_subset_exec;


  executor_p  * operations_list_p[exec_operation::OP_PRINTOUT_SIZE];
  executor_f  * operations_list_f[exec_operation::OP_FILTER_SIZE];
  executor_of * operations_list_of[exec_operation::OP_ON_FILTER_SIZE];
  executor_e  * operations_list_e[exec_operation::OP_EVALUATION_SIZE];

private:
  static const char * get_operation_callback_text(const exec_operation::printout_operation_t op){

    switch(op){
    case exec_operation::OP_PRINTOUT_BEGIN :
      return "begin";
    case exec_operation::OP_PRINTOUT_END   :
      return "end";
    case exec_operation::OP_PRINTOUT_IGNORE:
      return "ignore";
    case exec_operation::OP_PRINTOUT_HEADER:
      return "header";
    default:
      return "unknown";
    }
  }
  static const char * get_operation_callback_text(const exec_operation::filter_operation_t op){

    switch(op){
    case exec_operation::OP_FILTER_EQUAL     :
      return "equal";
    case exec_operation::OP_FILTER_GREATER   :
      return "greater";
    case exec_operation::OP_FILTER_LESS      :
      return "less";
    case exec_operation::OP_FILTER_MATCH     :
      return "match";
    case exec_operation::OP_FILTER_NOT_EQUAL :
      return "not_equal";
    case exec_operation::OP_FILTER_RANGE     :
      return "range";
    default:
      return "unknown";

    }
  }
  static const char * get_operation_callback_text(const exec_operation::on_filter_operation_t op){
    switch(op){
    case exec_operation::OP_ON_FILTER_UNITE        :
      return "unite";
    case exec_operation::OP_ON_FILTER_INTERSECT    :
      return "intersect";
    default:
      return "unknown";

    }
  }
  static const char * get_operation_callback_text(const exec_operation::evaluation_operation_t op){

    switch(op){
    case exec_operation::OP_EVALUATION_AND              :
      return "and";
    case exec_operation::OP_EVALUATION_OR               :
      return "or";
    case exec_operation::OP_EVALUATION_NOT              :
      return "not";
    case exec_operation::OP_EVALUATION_ALL_IN_PERIOD    :
      return "all_in_period";
    case exec_operation::OP_EVALUATION_ALL_IN_RANGE     :
      return "all_in_range";
    case exec_operation::OP_EVALUATION_COLUMNS_EQUAL    :
      return "columns_equal";
    case exec_operation::OP_EVALUATION_COLUMNS_GREATER  :
      return "columns_greater";
    case exec_operation::OP_EVALUATION_COLUMNS_LESS     :
      return "columns_less";
    case exec_operation::OP_EVALUATION_IS_EMPTY         :
      return "is_empty";
    case exec_operation::OP_EVALUATION_IS_EQUAL         :
      return "is_equal";
    case exec_operation::OP_EVALUATION_IS_IN_PERIOD     :
      return "is_in_period";
    case exec_operation::OP_EVALUATION_IS_NOT_EMPTY     :
      return "is_not_empty";
    case exec_operation::OP_EVALUATION_IS_NOT_EQUAL     :
      return "is_not_equal";
    case exec_operation::OP_EVALUATION_IS_SUBSET        :
      return "is_subset";
    default:
      return "unknown";

    }
  }


public:
  int apply_filter(const exec_operation::printout_operation_t op, std::string & printout,std::vector<std::string> &header, SUFFIX_CONCAT(printout_parameter,SUFFIX_C) * s);
  int apply_filter(const exec_operation::filter_operation_t op, matrix_str_t & matrix,std::vector<std::string> & header, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * s);
  int apply_filter(const exec_operation::on_filter_operation_t op, matrix_str_t & matrix_left,matrix_str_t & matrix_right, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) * s);
  int apply_filter(const exec_operation::evaluation_operation_t op, std::map<std::string,std::pair< std::vector<std::string> , matrix_str_t> > & set_id, std::vector<std::string> &header, SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) * s);
};

#endif/*RULE_CALLBACK_HANDLER_H_*/
