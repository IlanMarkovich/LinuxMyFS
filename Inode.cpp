#include "Inode.h"

#include <algorithm>
#include <stdexcept>

// C'tors

Inode::Inode(string name, int size) : _name(name), _size(size)
{
}

Inode::Inode(string input)
{
    int i = 0;

    // Read name of inode
    for(; input[i] != ','; i++)
    {
        _name += input[i];
    }

    string size;

    for(i++; input[i] != ','; i++)
    {
        size += input[i];
    }

    _size = atoi(size.c_str());
    string num;

    // Get the blocks of the inode
    for(i++; i < input.size(); i++)
    {
        if(input[i] == ',')
        {
            _blocks.push_back(atoi(num.c_str()));
            num = "";
        }
        else
        {
            num += input[i];
        }
    }
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

vector<int> Inode::getBlocks() const
{
    return _blocks;
}

// SETTERS

void Inode::setSize(int size)
{
    _size = size;
}

// OPERATORS

// This method convert the inode to string
// in the following format:
// <name>,<size>,<block>,<block>,....<block>,
Inode::operator string()
{
    string str = _name + ",";
    str += std::to_string(_size) + ",";

    for(int block : _blocks)
    {
        str += std::to_string(block) + ",";
    }

    return str;
}

int Inode::operator[](int index)
{
    return _blocks[index];
}

// METHODS

void Inode::add(int block)
{
    _blocks.push_back(block);
}

void Inode::remove(int block)
{
    try
    {
        _blocks.erase(std::find(_blocks.begin(), _blocks.end(), block));
    }
    catch(std::exception& e)
    {
        throw std::runtime_error("Tried to remove non-existent block from inode");
    }
}
