#include "FileInode.h"

#include <algorithm>

// C'tors

FileInode::FileInode(string name, int size) : Inode(name, size)
{
}

FileInode::FileInode(string fileInode)
{
    int i = 0;

    // Read name of inode
    for(; fileInode[i] != ','; i++)
    {
        _name += fileInode[i];
    }

    string size;

    for(i++; fileInode[i] != ','; i++)
    {
        size += fileInode[i];
    }

    _size = atoi(size.c_str());
    string num;

    // Get the blocks of the inode
    for(i++; i < fileInode.size(); i++)
    {
        if(fileInode[i] == ',')
        {
            _blocks.push_back(atoi(num.c_str()));
            num = "";
        }
        else
        {
            num += fileInode[i];
        }
    }
}

// GETTERS

vector<int> FileInode::getBlocks() const
{
    return _blocks;
}

// OPERATOR

FileInode::operator string()
{
    string ret = _name + ',';
    ret += std::to_string(_size) + ',';
    
    for(int block : _blocks)
    {
        ret += std::to_string(block) + ',';
    }

    return ret;
}

int FileInode::operator[](int index)
{
    return _blocks[index];
}

// METHODS

void FileInode::add(int index)
{
    _blocks.push_back(index);
}

void FileInode::remove(int index)
{
    if(std::count(_blocks.begin(), _blocks.end(), index) != 0)
    {
        _blocks.erase(std::find(_blocks.begin(), _blocks.end(), index));
    }
}
