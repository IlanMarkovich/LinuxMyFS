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
    vector<int> _blocks;

public:
    // C'tors

    Inode(string name, int size);
    Inode(string input);

    // GETTERS

    string getName() const;
    int getSize() const;
    vector<int> getBlocks() const;

    // SETTERS

    void setSize(int size);

    // OPERATORS

    operator string();
    int operator[](int index);

    // METHODS

    void add(int block);
    void remove(int block);
};
