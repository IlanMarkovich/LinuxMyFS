#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

class Inode
{
private:
    // FIELDS

    string _name;
    int _size;
    vector<int> _storage;
    bool _is_dir;

public:
    // C'tor

    Inode(string str);
    Inode(string name, int size, bool is_dir);

    // GETTERS

    string getName() const;
    int getSize() const;
    bool isDir() const;
    vector<int> getStorage() const;

    // SETTERS

    void setSize(int size);

    // OPERATOR

    operator string();
    int operator[](int index);

    // METHODS

    void add(int element);
    void remove(int element);
};
