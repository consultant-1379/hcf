/*
 * ruleset_file_containers.h
 *
 *  Created on: Feb 2, 2015
 *      Author: xclaele
 */

#ifndef RULESET_FILE_CONTAINERS_H_
#define RULESET_FILE_CONTAINERS_H_

#include <string>
#include <list>
#include <vector>
#include <map>

#define SUFFIX_C _container
#define SUFFIX_CONCAT(x,y) SUFFIX_CONCAT_IMPL(x,y)
#define SUFFIX_CONCAT_IMPL(x, y) x ## y

/************************************************************/
/*             PRINTOUT CONTAINER SET                       */
/************************************************************/

/***************/
/*   BEGIN     */
/***************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(begin,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	/*member variable*/
	std::string _value;
	std::string _skip_full_line;
};

/***************/
/*   END       */
/***************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(end,SUFFIX_C)

class __CLASS_NAME__
{
public:
	class error
	{
	public:
		//=============//
		// Constructor //
		//============//
		inline error(){};

		//============//
		// Destructor //
		//============//
		inline virtual
		~error(){};

	public:
		/*member variable*/
		std::string _value;
		std::string _intext;
		std::string _maxlines;

	};
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	/*member variable*/
	std::string _normal;
	std::string _skip_full_line;
	std::vector<error> _error_list;

};

/***************/
/*   IGNORE    */
/***************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(ignore,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	/*member variable*/
	std::string _value;
	std::string _intext;
	std::string _lines;
};

/***********************************/
/*         HEADER - column         */
/***********************************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(header,SUFFIX_C)

class __CLASS_NAME__
{
public:
	/*     nested column class*/
	class column
	{
	public:
		inline column(){};
		inline virtual
		~column(){};
	public:
		std::string _required;
		std::string _value;
	};
	/*END - nested column class*/

public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::vector<column> _columns;
	std::string _colSeparator;
	std::string _headerPresence;
	std::string _level;
	std::string _maxlines;

public:
	inline void get_required_columns(std::vector<std::string> & required_columns)
	{
		for(std::vector<column>::iterator it = _columns.begin() ; it != _columns.end(); ++it)
		{
			if (it->_required == "true")
				required_columns.push_back(it->_value);
		}
	}
};

/***********************************/
/*   PRINTOUT PARAMETER CONTAINER  */
/***********************************/

#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ printout_parameter_container

class __BASE_CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __BASE_CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__BASE_CLASS_NAME__(){};

public:
	std::string result;

	SUFFIX_CONCAT(begin,SUFFIX_C) begin_c;
	SUFFIX_CONCAT(end,SUFFIX_C) end_c;
	std::list<SUFFIX_CONCAT(ignore,SUFFIX_C)> ignore_list;
	std::list<SUFFIX_CONCAT(header,SUFFIX_C)> header_list;
};

/************************************************************/
/*             FILTER CONTAINER SET                         */
/************************************************************/

/***************/
/*   EQUAL     */
/***************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(equal,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _name;
	std::string _value;
	std::string _type;
};

/***************/
/*   GREATER   */
/***************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(greater,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _name;
	std::string _value;
	std::string _type;
};

/***************/
/*    LESS     */
/***************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(less,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _name;
	std::string _value;
	std::string _type;
};

/***************/
/*    MATCH    */
/***************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(match,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _name;
	std::string _value;
	std::string _type;
};

/*****************/
/*   NOT-EQUAL   */
/*****************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(not_equal,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _name;
	std::string _value;
	std::string _type;
};

/***************/
/*   RANGE     */
/***************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(range,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _name;
	std::string _min;
	std::string _max;
	std::string _type;
};

/***********************************/
/*    FILTER PARAMETER CONTAINER   */
/***********************************/

#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(filter_parameter,SUFFIX_C)

class __BASE_CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//

	inline __BASE_CLASS_NAME__():_rule_attrs(0){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__BASE_CLASS_NAME__(){};

public:
	std::map<std::string, std::pair<std::string, std::string>> * _rule_attrs;

	std::list<SUFFIX_CONCAT(equal,SUFFIX_C)> equal_list;
	std::list<SUFFIX_CONCAT(greater,SUFFIX_C)> greater_list;
	std::list<SUFFIX_CONCAT(less,SUFFIX_C)> less_list;
	std::list<SUFFIX_CONCAT(match,SUFFIX_C)> match_list;
	std::list<SUFFIX_CONCAT(not_equal,SUFFIX_C)> not_equal_list;
	std::list<SUFFIX_CONCAT(range,SUFFIX_C)> range_list;
};

/************************/
/*   FILTER CONTAINER   */
/************************/

#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(filter,SUFFIX_C)

class __BASE_CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//

	inline __BASE_CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__BASE_CLASS_NAME__(){};

public:
	std::string result;

	SUFFIX_CONCAT(filter_parameter,SUFFIX_C) filter_parameter;
	std::list<SUFFIX_CONCAT(filter_parameter,SUFFIX_C)> intersect_list;
	std::list<SUFFIX_CONCAT(filter_parameter,SUFFIX_C)> unite_list;
};

/************************************************************/
/*             EVALUATION CONTAINER SET                     */
/************************************************************/

/*********************/
/*   ALL-IN-PERIOD   */
/*********************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(all_in_period,SUFFIX_C)


class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set;
	std::string _start_time;
	std::string _end_time;
	std::string _format;
	std::string _col_date;
	std::string _col_time;
};

/*********************/
/*   ALL-IN-RANGE    */
/*********************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(all_in_range,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set;
	std::string _col;
	std::string _min;
	std::string _max;
	std::string _type;
};

/*********************/
/*   COLUMNS-EQUAL    */
/*********************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(columns_equal,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set;
	std::string _col_left;
	std::string _col_right;
};

/************************/
/*   COLUMNS-GREATER     */
/************************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(columns_greater,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set;
	std::string _col_left;
	std::string _col_right;
	std::string _type;
	std::string _percent;
};

/********************/
/*   COLUMNS-LESS    */
/********************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(columns_less,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set;
	std::string _col_left;
	std::string _col_right;
	std::string _type;
	std::string _percent;
};

/******************/
/*   IS-EMPTY     */
/******************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_empty,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set;
};

/******************/
/*   IS-EQUAL     */
/******************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_equal,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set_left;
	std::string _set_right;
};

/*********************/
/*   IS-IN-PERIOD    */
/*********************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_in_period,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set;
	std::string _start_time;
	std::string _end_time;
	std::string _format;
	std::string _col_date;
	std::string _col_time;
};

/**********************/
/*   IS-NOT-EMPTY     */
/**********************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_not_empty,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set;
};

/*********************/
/*   IS-NOT-EQUAL    */
/*********************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_not_equal,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};

public:
	std::string _set_left;
	std::string _set_right;
};

/******************/
/*   IS-SUBSET     */
/******************/
#undef __CLASS_NAME__
#define __CLASS_NAME__ SUFFIX_CONCAT(is_subset,SUFFIX_C)

class __CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __CLASS_NAME__ (){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__CLASS_NAME__(){};
public:
	std::string _subset;
	std::string _superset;
};

/************************************/
/*  EVALUATION PARAMETER CONTAINER  */
/************************************/

#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C)

class __BASE_CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __BASE_CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__BASE_CLASS_NAME__(){};

public:
	std::list<SUFFIX_CONCAT(all_in_period,SUFFIX_C)> all_in_period_list;
	std::list<SUFFIX_CONCAT(all_in_range,SUFFIX_C)> all_in_range_list;
	std::list<SUFFIX_CONCAT(columns_equal,SUFFIX_C)> columns_equal_list;
	std::list<SUFFIX_CONCAT(columns_greater,SUFFIX_C)> columns_greater_list;
	std::list<SUFFIX_CONCAT(columns_less,SUFFIX_C)> columns_less_list;
	std::list<SUFFIX_CONCAT(is_empty,SUFFIX_C)> is_empty_list;
	std::list<SUFFIX_CONCAT(is_equal,SUFFIX_C)> is_equal_list;
	std::list<SUFFIX_CONCAT(is_in_period,SUFFIX_C)> is_in_period_list;
	std::list<SUFFIX_CONCAT(is_not_empty,SUFFIX_C)> is_not_empty_list;
	std::list<SUFFIX_CONCAT(is_not_equal,SUFFIX_C)> is_not_equal_list;
	std::list<SUFFIX_CONCAT(is_subset,SUFFIX_C)> is_subset_list;
};

/****************************/
/*   EVALUATION CONTAINER   */
/****************************/

#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(evaluation,SUFFIX_C)

class __BASE_CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __BASE_CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__BASE_CLASS_NAME__(){};

public:
	SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) evaluation_parmater;
	std::list<SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C)> and_list;
	std::list<SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C)> or_list;
	std::list<SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C)> not_list;
};

/***********************************/
/*           OS CONTAINER          */
/***********************************/

#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(os,SUFFIX_C)

class __BASE_CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __BASE_CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__BASE_CLASS_NAME__(){};

public:
	// Rule command
	std::string command;
	std::string command_printout;
	SUFFIX_CONCAT(printout_parameter,SUFFIX_C) printout;
	std::list<SUFFIX_CONCAT(filter,SUFFIX_C)> filter_list;
};

/***********************************/
/*   EXEC ENVIRONMENT CONTAINER    */
/***********************************/

#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(execEnv,SUFFIX_C)

class __BASE_CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __BASE_CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__BASE_CLASS_NAME__(){};

public:
	std::string _execOnCondMatch;
	std::string _execEnvCheck;
	std::list<std::string> _execEnvCondition_list;
};

/***********************************/
/*        PARAMETER CONTAINER      */
/***********************************/

#undef __BASE_CLASS_NAME__
#define __BASE_CLASS_NAME__ SUFFIX_CONCAT(parameter,SUFFIX_C)

class __BASE_CLASS_NAME__
{
public:
	//=============//
	// Constructor //
	//============//
	inline __BASE_CLASS_NAME__(){};

	//============//
	// Destructor //
	//============//
	inline virtual
	~__BASE_CLASS_NAME__(){};

public:


public:
	std::list<SUFFIX_CONCAT(execEnv,SUFFIX_C)> execEnv_list;
	std::list<SUFFIX_CONCAT(os,SUFFIX_C)> os_list;
	SUFFIX_CONCAT(evaluation,SUFFIX_C) evaluation;

public:

	inline void set_rule_attributes(std::map<std::string, std::pair<std::string, std::string>> * rule_attrs)
	{
		for(std::list<SUFFIX_CONCAT(os,SUFFIX_C)>::iterator it1 = os_list.begin(); it1 != os_list.end(); ++it1)
		{
			for(std::list<SUFFIX_CONCAT(filter,SUFFIX_C)>::iterator it2 = it1->filter_list.begin(); it2 != it1->filter_list.end(); ++it2)
			{
				it2->filter_parameter._rule_attrs = rule_attrs;

				for(std::list<SUFFIX_CONCAT(filter_parameter,SUFFIX_C)>::iterator it3 = it2->intersect_list.begin(); it3 != it2->intersect_list.end(); ++it3)
				{
					it3->_rule_attrs = rule_attrs;
				}

				for(std::list<SUFFIX_CONCAT(filter_parameter,SUFFIX_C)>::iterator it3 = it2->unite_list.begin(); it3 != it2->unite_list.end(); ++it3)
				{
					it3->_rule_attrs = rule_attrs;
				}
			}
		}
	};
};

#endif /* RULESET_FILE_CONTAINERS_H_ */
