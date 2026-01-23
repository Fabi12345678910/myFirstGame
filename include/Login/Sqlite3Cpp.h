#pragma once
#include "sqlite3.h"
#include <stdexcept>
struct SQLiteException : public std::runtime_error {
    explicit SQLiteException(const std::string& msg) : std::runtime_error(msg) {}
};

class SQLiteDB {
public:
    explicit SQLiteDB(const std::string& filename) {
        if (sqlite3_open(filename.c_str(), &db_) != SQLITE_OK) {
            throw SQLiteException("Cannot open database: " + std::string(sqlite3_errmsg(db_)));
        }
    }

    ~SQLiteDB() {
        if (db_) sqlite3_close(db_);
    }

    // Execute simple SQL (CREATE TABLE, etc.)
    void execute(const std::string& sql) {
        char* errmsg = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
            std::string err = errmsg ? errmsg : "Unknown error";
            sqlite3_free(errmsg);
            throw SQLiteException(err);
        }
    }

    sqlite3* get() const { return db_; }

private:
    sqlite3* db_{nullptr};
};

class SQLiteStmt {
public:
    SQLiteStmt(sqlite3* db, const std::string& sql) {
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_, nullptr) != SQLITE_OK) {
            throw SQLiteException("Failed to prepare statement");
        }
    }

    ~SQLiteStmt() {
        if (stmt_) sqlite3_finalize(stmt_);
    }

    // Bind int64_t
    void bind(int index, int64_t value) {
        if (sqlite3_bind_int64(stmt_, index, value) != SQLITE_OK)
            throw SQLiteException("Failed to bind int64_t");
    }

    // Bind int (enum)
    void bind(int index, int value) {
        if (sqlite3_bind_int(stmt_, index, value) != SQLITE_OK)
            throw SQLiteException("Failed to bind int");
    }

    // Bind string
    void bind(int index, const std::string& value) {
        if (sqlite3_bind_text(stmt_, index, value.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
            throw SQLiteException("Failed to bind string");
    }

    bool step() {
        int rc = sqlite3_step(stmt_);
        if (rc == SQLITE_ROW) return true;
        if (rc == SQLITE_DONE) return false;
        throw SQLiteException("Failed to step statement");
    }

    int64_t columnInt64(int col) const {
        return sqlite3_column_int64(stmt_, col);
    }

    int columnInt(int col) const {
        return sqlite3_column_int(stmt_, col);
    }

    std::string columnText(int col) const {
        const char* txt = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, col));
        return txt ? txt : "";
    }

private:
    sqlite3_stmt* stmt_{nullptr};
};
