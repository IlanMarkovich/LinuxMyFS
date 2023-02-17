#include "Inode.h"

#include <algorithm>

// C'tors

Inode::Inode(string str)
{
    int i = 0;

    // Read name of inode
    for(; str[i] != ','; i++)
    {
        _name += str[i];
    }

    _is_dir = str[++i] == 'd';
    string size;

    for(i++; str[i] != ','; i++)
    {
        size += str[i];
    }

    _size = atoi(size.c_str());
    string num;

    // Get the blocks of the inode
    for(i++; i < str.size(); i++)
    {
        if(str[i] == ',')
        {
            _storage.push_back(atoi(num.c_str()));
            num = "";
        }
        else
        {
            num += str[i];
        }
    }
}

Inode::Inode(string name, int size, bool is_dir) : _name(name), _size(size), _is_dir(is_dir)
{
}

// GETTERS

string Inode::getName() const
{
    return _name;
}

int Inode::getSize() const
{
    return _size;
}

bool Inode::isDir() const
{
    return _is_dir;
}

vector<int> Inode::getStorage() const
{
    return _storage;
}

// SETTERS

void Inode::setSize(int size)
{
    _size = size;
}

// OPERATORS

Inode::operator string()
{
    string str = _name + ',';
    str += _is_dir ? 'd' : 'f';
    str += std::to_string(_size) + ',';
    
    for(int element : _storage)
    {
        str += std::to_string(element) + ',';
    }

    return str;
}

int Inode::operator[](int index)
{
    return _storage[index];
}

// METHODS

void Inode::add(int element)
{
    _storage.push_back(element);
}

void Inode::remove(int element)
{
    if(std::count(_storage.begin(), _storage.end(), element) != 0)
    {
        _storage.erase(std::find(_storage.begin(), _storage.end(), element));
    }
}
