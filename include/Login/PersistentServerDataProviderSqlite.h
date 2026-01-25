#include "Login/PersistentServerDataProvider.h"
#include "Login/LoginTypes.h"
#include "Login/Sqlite3Cpp.h"
#include <string>

static constexpr char const *sql_create_key =
    "CREATE TABLE IF NOT EXISTS kv_store ("
    "key INTEGER PRIMARY KEY, "
    "id INTEGER NOT NULL);";

static constexpr const char *sql_create_player =
    "CREATE TABLE IF NOT EXISTS player_info ("
    "id   INTEGER PRIMARY KEY,"
    "name TEXT NOT NULL,"
    "class INTEGER NOT NULL);";

class PersistentServerDataProviderSqlite : public PersistentServerDataProvider{
private:
    SQLiteDB db;
public:
    virtual USER_ID_TYPE getUserId(USER_LOGIN_KEY_TYPE loginKey)override{
        SQLiteStmt stmt(db.get(), "SELECT id FROM kv_store WHERE key = ?;");
        stmt.bind(1, loginKey);
        if (stmt.step()) {
            return stmt.columnInt64(0);
        }
        throw SQLiteException("Login key not found");};
    virtual std::string getUserName(USER_ID_TYPE userId) override{
        SQLiteStmt stmt(db.get(), "SELECT name FROM player_info WHERE id = ?;");
        stmt.bind(1, userId);
        if (stmt.step()) {
            return stmt.columnText(0);
        }
        throw SQLiteException("User ID not found");
    };
    virtual void renameUser(USER_ID_TYPE userId, const std::string& newUserName) override{
        SQLiteStmt stmt(db.get(),"UPDATE player_info SET name = ? WHERE id = ?;");
        stmt.bind(1, newUserName);
        stmt.bind(2, userId);
        stmt.step();  // SQLITE_DONE
        }

    virtual USER_LOGIN_KEY_TYPE createUser()override{return createUser("unnamed");};
    virtual USER_LOGIN_KEY_TYPE createUser(const std::string& userName) override{
        //TODO autogenerate key and id
        USER_LOGIN_KEY_TYPE key = 21308412;
        USER_ID_TYPE id  = 43210987;
        {
            SQLiteStmt stmt(db.get(), "INSERT INTO kv_store (key, id) VALUES (?, ?);");
            stmt.bind(1, int64_t(key));
            stmt.bind(2, int64_t(id));
            stmt.step();
        }
        {
            SQLiteStmt stmt(db.get(),"INSERT OR REPLACE INTO player_info (id, name, class) VALUES (?, ?, ?);");
            stmt.bind(1, id);
            stmt.bind(2, userName);
            stmt.bind(3, static_cast<int>(0));
            stmt.step();
        }
        return key;
    }

    PersistentServerDataProviderSqlite(const std::string fileName): db(fileName){
        db.execute(sql_create_key);
        db.execute(sql_create_player);
    };
};