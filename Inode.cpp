#include "Inode.h"

// C'tors

Inode::Inode() : _name(""), _size(0)
{
}

Inode::Inode(string name, int size) : _name(name), _size(size)
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

void Inode::setSize(int size)
{
    _size = size;
}
