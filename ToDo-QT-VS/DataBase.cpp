#include "DataBase.h"

#include "Util.h"

DBUpdateListNode::DBUpdateListNode(std::string key, std::string value, bool is_string) :
	key(key), value(value), is_string(is_string)
{ }

DBQueryListNode::DBQueryListNode() :
	key(""), value(""), is_string(false), mode(Mode::Empty)
{ }

DBQueryListNode::DBQueryListNode(std::string key, std::string value, bool is_string, Mode mode) :
	key(key), value(value), is_string(is_string), mode(mode)
{ }

bool DBQueryListNode::empty() const
{
	return this->mode == Mode::Empty;
}

std::string DBQueryListNode::getString() const
{
	if (mode == Mode::Empty) { return ""; }
	std::string res = "";
	std::string _mode = "";
	switch (mode)
	{
	case DBQueryListNode::Mode::Greater:
		_mode = " > ";
		break;
	case DBQueryListNode::Mode::Less:
		_mode = " < ";
		break;
	case DBQueryListNode::Mode::Like:
		_mode = " LIKE ";
		break;
	case DBQueryListNode::Mode::NotEqual:
		_mode = " != ";
		break;
	case DBQueryListNode::Mode::Equal:
	default:
		_mode = " = ";
		break;
	}
	res +=
		key + _mode +
		(is_string ? "\'" : "") +
		(mode == DBQueryListNode::Mode::Like ? "%" : "") +
		value +
		(mode == DBQueryListNode::Mode::Like ? "%" : "") +
		(is_string ? "\'" : "");

	return res;
}

DBQueryListGroupNode::DBQueryListGroupNode() :
	mode(Mode::Empty)
{ }

DBQueryListGroupNode::DBQueryListGroupNode(DBQueryListNode listnode) :
	node(listnode), mode(Mode::Node)
{ }

void DBQueryListGroupNode::append(DBQueryListGroupNode node)
{
	if (mode == Mode::Node || mode == Mode::Empty)
	{
		mode = Mode::AndGroup;
		if (!this->node.empty()) append(DBQueryListGroupNode(this->node));
	}

	node_group.emplace_back(node);
}

void DBQueryListGroupNode::append(DBQueryListNode node)
{
	this->append(DBQueryListGroupNode(node));
}

bool DBQueryListGroupNode::setModeNode()
{
	if (node_group.size() > 1) 
	{
		return false; 
	} 

	mode = Mode::Node; 
	return false; 
}

bool DBQueryListGroupNode::setModeAnd()
{
	mode = Mode::AndGroup;
	return true;
}

bool DBQueryListGroupNode::setModeOr()
{
	mode = Mode::OrGroup;
	return true;
}

std::string DBQueryListGroupNode::getString() const
{
	if (mode == Mode::Empty) { return ""; }
	if (mode == Mode::Node) { return node.getString(); }
	std::string res = "";
	std::string _mode = " and ";
	if (mode == Mode::OrGroup) { _mode = " or "; }
	for (int i = 0; i < node_group.size(); ++i)
	{
		if (i) { res += _mode; }
		res += "(" + node_group[i].getString() + ")";
	}

	return res;
}

DBSortListNode::DBSortListNode(std::string key, bool is_asc) :
	key(key), is_asc(is_asc)
{ }

std::string DBSortListNode::getString() const
{
	std::string res = "";
	res += key + " " + (is_asc ? "ASC" : "DESC");
	return res;
}

DBLimit::DBLimit(int offset, int limit) :
	offset(offset), limit(limit)
{ }

std::string DBLimit::getString() const
{
	std::string res = "";
	if (limit != -1 && offset != -1)
	{
		res += std::to_string(offset) + "," + std::to_string(limit);
	}
	return 	res;
}


DataBase::DataBase(std::string db_path)
{
	int nRes = sqlite3_open(db_path.data(), &m_db);
	if (nRes != SQLITE_OK)
	{
		fatalError(std::string("Open database fail: ") + sqlite3_errmsg(m_db));
	}
	m_can_use = true;
}

DBQueryRes DataBase::DBQuery(std::string _sql_from, std::string _sql_what, std::string _sql_where, std::string _sql_sort, std::string _sql_limit)
{
	DBQueryRes result;

	std::string _sql = "SELECT " + _sql_what + " FROM " + _sql_from;

	if (!_sql_where.empty()) { _sql += " WHERE "; }
	if (!_sql_where.empty()) { _sql += _sql_where; }

	if (!_sql_sort.empty()) { _sql += " ORDER BY "; }
	if (!_sql_sort.empty()) { _sql += _sql_sort; }

	if (!_sql_limit.empty()) { _sql += " LIMIT "; }
	if (!_sql_limit.empty()) { _sql += _sql_limit; }

	_sql += ";";

	char* cErrMsg;
	int res = sqlite3_exec(
		m_db, _sql.data(),
		[](void* ptr, int argc, char** argv, char** az_col_name) -> int {
			DBQueryRes* _query_res = static_cast<DBQueryRes*>(ptr);
			for (int i = 0; i < argc; i++)
			{
				(*_query_res)[az_col_name[i]].emplace_back(argv[i]);
			}
			_query_res->rows++;
			_query_res->cols = argc;
			return 0;
		},
		&result, &cErrMsg
	);

	return result;
}

DBQueryRes DataBase::DBQuery(std::string _sql_from, std::string _sql_what, DBQueryList query_list, DBSortList sort_list, DBLimit limit)
{
	std::string _sql_where = "";
	std::string _sql_sort = "";
	std::string _sql_limit = "";

	for (int i = 0; i < query_list.size(); ++i)
	{
		if (i) { _sql_where += " and "; }
		_sql_where += query_list[i].getString();
	}

	for (int i = 0; i < sort_list.size(); ++i)
	{
		if (i)_sql_sort += ",";
		_sql_sort += sort_list[i].getString();
	}

	_sql_limit += limit.getString();

	return DBQuery(_sql_from, _sql_what, _sql_where, _sql_sort, _sql_limit);
}

DBQueryRes DataBase::DBQuery(std::string _sql_from, std::string _sql_what, DBQueryListGroupNode query_list, DBSortList sort_list, DBLimit limit)
{
	std::string _sql_where = "";
	std::string _sql_sort = "";
	std::string _sql_limit = "";

	_sql_where = query_list.getString();
	for (int i = 0; i < sort_list.size(); ++i)
	{
		if (i)_sql_sort += ",";
		_sql_sort += sort_list[i].getString();
	}
	_sql_limit = limit.getString();

	return DBQuery(_sql_from, _sql_what, _sql_where, _sql_sort, _sql_limit);
}

char* DataBase::DBUpdate(std::string _sql_from, DBUpdateList change_list, std::string _sql_where)
{
	if (change_list.empty()) { return nullptr; }
	std::string _sql = "UPDATE " + _sql_from + " SET ";
	for (int i = 0; i < change_list.size(); ++i)
	{
		if (i) { _sql += ','; }
		bool is_string = change_list[i].is_string;
		_sql += change_list[i].key + '=' + (is_string ? "'" : "") + change_list[i].value + (is_string ? "'" : "");
	}

	_sql += " WHERE " + _sql_where + ";";

	char* cErrMsg;
	int nRes = sqlite3_exec(m_db, _sql.data(), 0, 0, &cErrMsg);

	return cErrMsg;
}

char* DataBase::DBInsert(std::string _sql_from, std::string _sql_what, std::string _sql_value)
{
	std::string _sql = "INSERT INTO " + _sql_from + "(" + _sql_what + ") VALUES(" + _sql_value + ");";

	char* cErrMsg;
	int nRes = sqlite3_exec(m_db, _sql.data(), 0, 0, &cErrMsg);
	return cErrMsg;
}

char* DataBase::DBDelete(std::string _sql_from, std::string _sql_where)
{
	std::string _sql = "DELETE FROM " + _sql_from + " WHERE " + _sql_where + ";";

	char* cErrMsg;
	int nRes = sqlite3_exec(m_db, _sql.data(), 0, 0, &cErrMsg);

	return cErrMsg;
}

char* DataBase::DBExecuteSQL(std::string _sql)
{
	char* cErrMsg;
	int nRes = sqlite3_exec(m_db, _sql.data(), 0, 0, &cErrMsg);

	return cErrMsg;
}

bool DataBase::tableExists(const std::string table_name)
{
	const std::string query =
		"SELECT name FROM sqlite_master WHERE type='table' AND name='" + table_name + "';";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr);

	if (rc != SQLITE_OK) { return false; }

	bool exists = (sqlite3_step(stmt) == SQLITE_ROW); // 如果查询返回行，说明表存在
	sqlite3_finalize(stmt);
	return exists;
}

bool DataBase::tableColumnExists(const std::string tableName, const std::string columnName)
{
	// 先检查表是否存在
	if (!tableExists(tableName)) 
	{
		return false;
	}

	// 准备查询表结构的SQL
	const std::string query =
		"SELECT 1 FROM pragma_table_info(?) WHERE name=? LIMIT 1;";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		return false;
	}

	// 绑定参数
	sqlite3_bind_text(stmt, 1, tableName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, columnName.c_str(), -1, SQLITE_STATIC);

	char* expanded_sql = sqlite3_expanded_sql(stmt);
	if (expanded_sql) {
		qDebug() << "实际执行的SQL: " << expanded_sql;
		sqlite3_free(expanded_sql); // 必须释放内存
	}

	// 执行查询
	bool exists = (sqlite3_step(stmt) == SQLITE_ROW);

	// 释放资源
	sqlite3_finalize(stmt);

	return exists;
}
