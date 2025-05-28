#pragma once

#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>

class DBUpdateListNode
{
public:
	DBUpdateListNode(std::string key, std::string value, bool is_string);
	std::string key;
	std::string value;
	bool is_string;
};

class DBQueryListNode
{
public:
	enum class Mode
	{
		Empty,      // 空
		Equal,      // 等于
		NotEqual,   // 不等于
		Less,       // 小于
		Greater,    // 大于
		Like        // 模糊搜索
	};
public:
	DBQueryListNode();
	DBQueryListNode(std::string key, std::string value,
		bool is_string, Mode mode = Mode::Equal);
	bool empty() const;
	std::string getString() const;
public:
	std::string key;
	std::string value;
	bool is_string;
	Mode mode;
};

class DBQueryListGroupNode
{
public:
	enum class Mode
	{
		Empty,     // 空
		Node,      // 普通节点
		OrGroup,   // 或
		AndGroup,  // 与
	};
public:
	DBQueryListGroupNode();
	DBQueryListGroupNode(DBQueryListNode listnode);

	void append(DBQueryListGroupNode node);
	void append(DBQueryListNode node);
	bool empty() const { return mode == Mode::Empty; }

	bool setModeNode(); //设置为node
	bool setModeAnd(); // 设置为And
	bool setModeOr(); // 设置为Or

	std::string getString() const;
private:
	DBQueryListNode node;
	std::vector<DBQueryListGroupNode> node_group;
	Mode mode;

};

class DBSortListNode
{
public:
	DBSortListNode(std::string key, bool is_asc = true);
	std::string getString() const;
public:
	std::string key;
	bool is_asc;
};

class DBLimit
{
public:
	DBLimit(int offset, int limit);
	std::string getString() const;
public:
	int offset;
	int limit;
};

//using DBQueryRes = std::map<std::string, std::vector<std::string>>;
class DBQueryRes
{
public:
	int rows = 0;
	int cols = 0;
	std::vector<std::string>& operator[](const std::string& key)
	{
		return data[key];
	}

	const std::vector<std::string>& operator[](const std::string& key) const
	{
		return data.at(key);
	}
private:
	std::map<std::string, std::vector<std::string>> data;
};
using DBUpdateList = std::vector<DBUpdateListNode>;
using DBQueryList = std::vector<DBQueryListNode>;
using DBSortList = std::vector<DBSortListNode>;

class DataBase
{
public:
	DataBase(std::string db_path);
	DBQueryRes DBQuery(std::string _sql_from, std::string _sql_what, std::string _sql_where = "",
		std::string _sql_sort = "", std::string _sql_limit = "");
	DBQueryRes DBQuery(std::string _sql_from, std::string _sql_what, DBQueryList query_list,
		DBSortList sort_list = {}, DBLimit limit = { -1, -1 });
	DBQueryRes DBQuery(std::string _sql_from, std::string _sql_what, DBQueryListGroupNode query_list,
		DBSortList sort_list = {}, DBLimit limit = { -1, -1 });
	char* DBUpdate(std::string _sql_from, DBUpdateList change_list, std::string _sql_where);
	char* DBInsert(std::string _sql_from, std::string _sql_what, std::string _sql_value);
	char* DBDelete(std::string _sql_from, std::string _sql_where);

	char* DBExecuteSQL(std::string _sql);
	bool tableExists(const std::string tableName);
	bool tableColumnExists(const std::string tableName, const std::string columnName);
private:
	sqlite3* m_db;
	bool m_can_use = false;
};