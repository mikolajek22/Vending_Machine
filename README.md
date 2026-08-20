

## Jak zbudować



GTest install

```
sudo apt install libgtest-dev
```
SQLite
```
sudo apt install libsqlite3-dev
```
Build:
```
cmake -S . -B build
cmake --build build
```

./build/tests



### Notatki dla mnie

Sqlite ma dwa mechanizmy indexowania, parametry w bindach indeksuje się od 1 np:
```
UPDATE transactions SET status = ? WHERE status = ?
sqlite3_bind_int(stmt, 1, STATUS_PENDING);
```

W przypadku indeksowania kolumn:
```
t.uuid = sqlite3_column_text(stmt, 0);
```