#pragma once

#include <string>

using std::string;

class Inode
{
protected:
    // FIELDS

    string _name;
    int _size;

public:
    // C'tor

    Inode();
    Inode(string name, int size);

    // GETTERS

    string getName() const;
    int getSize() const;

    // SETTERS

    void setSize(int size);

    // OPERATOR

    virtual operator string() = 0;
    virtual int operator[](int index) = 0;

    // METHODS

    virtual void add(int index) = 0;
    virtual void remove(int index) = 0;
};
