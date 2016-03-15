#include "order_data.h"

bool OrderData::create_table(sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::string query = std::string("CREATE TABLE IF NOT EXISTS ") + table_name;
  query += " ";
  query += make_column_type_string();
  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    error_msg += sqlite3_errmsg(db);
    return false;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };

  if (sqlite3_step(stmt) != SQLITE_DONE)
  {
    error_msg = "Can't create sqlite table: ";
    return false;
  }

  return true;
}

bool OrderData::insert(sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::string query = std::string("INSERT INTO ") + table_name;
  query += " ";
  query += make_column_string();
  query += " VALUES";
  query += make_values_string();
  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    return false;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };

  int bind_location = 1;
#define X(type, name) \
  if ((result = sqlite_bind<type>(stmt, bind_location, name)) != SQLITE_OK) \
  { \
    error_msg = std::string("sqlite3_bind_int failed. Error code: ") + std::to_string(result); \
  } \
  bind_location++;
  FIELDS
#undef X

  if ((result = sqlite3_step(stmt)) != SQLITE_DONE)
  {
    error_msg = std::string("sqlite3_step failed. Error code: ") + std::to_string(result);
    return false;
  }

  return true;
}

bool OrderData::update(sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::string query = std::string("UPDATE ") + table_name + " SET "
#define X(type, name) + #name + " = ?,"
  FIELDS
#undef X
  +" WHERE internal_id = " + std::to_string(internal_id);

  int idx = query.find_last_of(", ");
  query.replace(idx - 1, 2, " ");
  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    return false;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };

  int bind_location = 1;
#define X(type, name) \
  if ((result = sqlite_bind<type>(stmt, bind_location, name)) != SQLITE_OK) \
  { \
  error_msg = std::string("sqlite3_bind_int failed. Error code: ") + std::to_string(result); \
  } \
  bind_location++;
  FIELDS
#undef X

  if ((result = sqlite3_step(stmt)) != SQLITE_DONE)
  {
    error_msg = std::string("sqlite3_step failed. Error code: ") + std::to_string(result);
    return false;
  }

  return true;
}

std::vector<OrderData> OrderData::select_all(sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::vector<OrderData> orders;
  std::string query = std::string("SELECT * FROM ") + table_name;
  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    return orders;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };

  while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    OrderData order_data = get_row(stmt);
    orders.push_back(order_data);
  }

  return orders;
}

std::vector<OrderData> OrderData::select_all_by_id(int id, sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::vector<OrderData> orders;
  std::string query = std::string("SELECT * FROM ") + table_name + " WHERE order_id = " + std::to_string(id);
  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    return orders;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };


  while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    OrderData order_data = get_row(stmt);
    orders.push_back(order_data);
  }

  return orders;
}

bool OrderData::select_last_modified_by_id(int order_id, sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::vector<OrderData> orders;
  std::string query = std::string("SELECT * FROM ") + table_name + " WHERE order_id = " + std::to_string(order_id) + " ORDER BY internal_id DESC LIMIT 1";
  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    return false;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };

  if ((result = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    OrderData order_data = get_row(stmt);
    memcpy(this, &order_data, sizeof(OrderData));
  }
  else
  {
    error_msg = std::string("0 results on SELECT. Error code: ") + std::to_string(result);
    return false;
  }

  return true;
}
std::vector<OrderData> OrderData::select_all_in_time_interval(long long from, long long to, sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::vector<OrderData> orders;
  std::string query = std::string("SELECT * FROM " + table_name + " WHERE modify_time > " + std::to_string(from) + " AND modify_time < " + std::to_string(to));
  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    return orders;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };

  while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    OrderData order_data = get_row(stmt);
    orders.push_back(order_data);
  }

  return orders;
}
std::vector<OrderData> OrderData::select_unique_last_modified_in_time_interval(long long from, long long to, sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::vector<OrderData> orders;
  std::string query = std::string("SELECT *, MAX(internal_id) FROM " + table_name + " WHERE modify_time > " + std::to_string(from) + " AND modify_time < " + std::to_string(to) + " GROUP BY order_id");
  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    return orders;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };

  while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    OrderData order_data = get_row(stmt);
    orders.push_back(order_data);
  }

  return orders;
}

std::vector<OrderData> OrderData::select_unique_last_modified_in_time_interval_by_user(long long from, long long to, int login, sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::vector<OrderData> orders;
  std::string query = "SELECT * FROM " + table_name + " WHERE account_id = " + std::to_string(login) + " AND modify_time > " + std::to_string(from) + " AND modify_time < " + std::to_string(to) + " GROUP BY order_id";

  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    return orders;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };

  while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    OrderData order_data = get_row(stmt);
    orders.push_back(order_data);
  }

  return orders;
}

bool OrderData::delete_all_before(long long before, sqlite3* db, std::string table_name, std::string& error_msg)
{
  error_msg = "OK";
  std::vector<OrderData> orders;
  std::string query = std::string("DELETE FROM ") + table_name + " WHERE modify_time < " + std::to_string(before);
  int result = 0;
  sqlite3_stmt* stmt;

  if ((result = sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, NULL)) != SQLITE_OK)
  {
    error_msg = std::string("sqlite3_prepare_v2 failed. Error code: ") + std::to_string(result);
    return false;
  }
  BOOST_SCOPE_EXIT_ALL(stmt)
  {
    sqlite3_finalize(stmt);
  };

  if ((result = sqlite3_step(stmt)) != SQLITE_DONE)
  {
    error_msg = std::string("sqlite3_step failed. Error code: ") + std::to_string(result);
    return false;
  }

  return true;
}

//==============================================
//Private
//==============================================

OrderData OrderData::get_row(sqlite3_stmt* stmt)
{
  OrderData order_data;
  int col = 0;
#define X(type, name) order_data.name = sqlite_get_row_value<type>(stmt, col); \
  col++;
  FIELDS
#undef X
  return order_data;
}