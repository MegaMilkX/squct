//==============================================
// Supported types for fields are:
// char, int, long, long long,
// unsigned char, unsigned int, unsigned long, unsigned long long, and std::string
//==============================================

#ifndef _SQUCT_H_
#define _SQUCT_H_

struct long_long_autoincr
{
public:
  long_long_autoincr() : _internal(0){}

  long_long_autoincr(long long value)
  {
    _internal = value;
  }

  operator long long() const
  {
    return _internal;
  }

private:
  long long _internal;
};

template<typename T>
std::string sql_type()
{ 
#pragma message ("order_data.h: sql_type() - unsupported type. Using BLOB") 
  return "BLOB"; 
}
template<>
inline std::string sql_type<char>(){ return "INTEGER"; }
template<>
inline std::string sql_type<int>(){ return "INTEGER"; }
template<>
inline std::string sql_type<long>(){ return "INTEGER"; }
template<>
inline std::string sql_type<long long>(){ return "INTEGER"; }
template<>
inline std::string sql_type<unsigned char>(){ return "INTEGER"; }
template<>
inline std::string sql_type<unsigned int>(){ return "INTEGER"; }
//template<>
//inline std::string sql_type<unsigned long>(){ return "INTEGER"; }
//template<>
//inline std::string sql_type<unsigned long long>(){ return "INTEGER"; }
template<>
inline std::string sql_type<float>(){ return "REAL"; }
template<>
inline std::string sql_type<double>(){ return "REAL"; }
template<>
inline std::string sql_type<std::string>(){ return "TEXT"; }
template<>
inline std::string sql_type<long_long_autoincr>(){ return "INTEGER PRIMARY KEY AUTOINCREMENT"; }

template<typename T>
int sqlite_bind(sqlite3_stmt* stmt, int loc, const T& value) 
{ 
#pragma message ("order_data.h: sqlite_bind() - unsupported type. Binding BLOB")
  return sqlite3_bind_blob(stmt, loc, &value, sizeof(T), SQLITE_STATIC);
}
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const char& value) { return sqlite3_bind_int(stmt, loc, value); }
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const int& value) { return sqlite3_bind_int(stmt, loc, value); }
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const long& value) { return sqlite3_bind_int64(stmt, loc, value); }
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const long long& value) { return sqlite3_bind_int64(stmt, loc, value); }
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const unsigned char& value) { return sqlite3_bind_int(stmt, loc, value); }
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const unsigned int& value) { return sqlite3_bind_null(stmt, loc); }
//template<>
//inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const unsigned long& value) { return sqlite3_bind_int(stmt, loc, value); }
//template<>
//inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const unsigned long long& value) { return sqlite3_bind_int64(stmt, loc, value); }
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const float& value) { return sqlite3_bind_double(stmt, loc, value); }
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const double& value) { return sqlite3_bind_double(stmt, loc, value); }
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const std::string& value) { return sqlite3_bind_text(stmt, loc, value.c_str(), -1, 0); }
template<>
inline int sqlite_bind(sqlite3_stmt* stmt, int loc, const long_long_autoincr& value) { return sqlite3_bind_null(stmt, loc); }

template<typename T>
T sqlite_get_row_value(sqlite3_stmt* stmt, int col) 
{
#pragma message ("order_data.h: sqlite_get_row_value() - unsupported type. Reading as BLOB") 
  const void* blob = sqlite3_column_blob(stmt, col);
  T value;
  memcpy(&value, blob , sizeof(T));
  return value;
}
template<>
inline char                   sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_int(stmt, col); }
template<>
inline int                    sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_int(stmt, col); }
template<>
inline long                   sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_int64(stmt, col); }
template<>
inline long long              sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_int64(stmt, col); }
template<>
inline unsigned char          sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_int(stmt, col); }
template<>
inline unsigned int           sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_int(stmt, col); }
//template<>
//inline unsigned long          sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_int(stmt, col); }
//template<>
//inline unsigned long long     sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_int(stmt, col); }
template<>
inline float                  sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_double(stmt, col); }
template<>
inline double                 sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_double(stmt, col); }
template<>
inline std::string            sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, col))); }
template<>
inline long_long_autoincr     sqlite_get_row_value(sqlite3_stmt* stmt, int col) { return sqlite3_column_int64(stmt, col); }

struct Prop
{
  Prop() : type_index(typeid(int)){};
  Prop(std::type_index type_index, size_t offset, size_t size) :
    type_index(type_index), offset(offset), sz(size) {}
  std::type_index type_index;
  size_t offset;
  size_t sz;
};

struct OrderData
{
  //Never leave spaces after '\' symbol
#define FIELDS \
  X(long_long_autoincr, internal_id) \
  X(int, cmd) \
  X(int, state) \
  X(int, rec_type) \
  X(bool, is_trade) \
  X(bool, is_cancelled) \
  X(int, pending_type) \
  X(int, order_id) \
  X(std::string, symbol) \
  X(int, margin_mode) \
  X(int, profit_mode) \
  X(double, request_price) \
  X(double, request_volume) \
  X(long long, request_time) \
  X(double, exec_price) \
  X(double, exec_volume) \
  X(long long, exec_time) \
  X(double, req_close_price) \
  X(double, req_close_volume) \
  X(long long, req_close_time) \
  X(double, close_price) \
  X(double, close_volume) \
  X(long long, close_time) \
  X(long long, modify_time) \
  X(long long, cancel_time) \
  X(int, gtc) \
  X(double, sl) \
  X(double, tp) \
  X(double, swap) \
  X(double, commission) \
  X(double, profit) \
  X(double, contract_size) \
  X(int, account_id) \
  X(std::string, account_group) \
  X(std::string, account_name) \
  X(double, account_balance) \
  X(std::string, comment) \
  X(long long, expiration_time)

#define X(type, name) type name;
  FIELDS
#undef X

  bool create_table(sqlite3* db, std::string table_name, std::string& error_msg);
  bool insert(sqlite3* db, std::string table_name, std::string& error_msg);
  bool update(sqlite3* db, std::string table_name, std::string& error_msg);
  std::vector<OrderData> select_all(sqlite3* db, std::string table_name, std::string& error_msg);
  std::vector<OrderData> select_all_by_id(int id, sqlite3* db, std::string table_name, std::string& error_msg);
  bool select_last_modified_by_id(int order_id, sqlite3* db, std::string table_name, std::string& error_msg);
  std::vector<OrderData> select_all_in_time_interval(long long from, long long to, sqlite3* db, std::string table_name, std::string& error_msg);
  std::vector<OrderData> select_unique_last_modified_in_time_interval(long long from, long long to, sqlite3* db, std::string table_name, std::string& error_msg);
  std::vector<OrderData> select_unique_last_modified_in_time_interval_by_user(long long from, long long to, int login, sqlite3* db, std::string table_name, std::string& error_msg);

  bool delete_all_before(long long before, sqlite3* db, std::string table_namel, std::string& error_msg);

private:
  std::string make_column_string()
  {
    std::string str;
    str += "(";
#define X(type, name) str += std::string(#name) + ", ";
    FIELDS
#undef X
    int idx = str.find_last_of(", ");
    str.replace(idx - 1, 2, ")");
    return str;
  }

  std::string make_column_type_string()
  {
    std::string str;
    str += "(";
#define X(type, name) str += std::string(#name) + " " + sql_type<type>() + ", ";
    FIELDS
#undef X
      int idx = str.find_last_of(", ");
    str.replace(idx - 1, 2, ")");
    return str;
  }

  std::string make_values_string()
  {
    std::string str;
    str += "(";
#define X(type, name) str += std::string("?") + ", ";
    FIELDS
#undef X
      int idx = str.find_last_of(", ");
    str.replace(idx - 1, 2, ")");
    return str;
  }

  OrderData get_row(sqlite3_stmt* stmt);
};

#endif