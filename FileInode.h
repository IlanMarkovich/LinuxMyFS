#pragma once

#include "Inode.h"
#include <vector>

using std::vector;

class FileInode : public Inode
{
private:
    // FIELDS

    vector<int> _blocks;

public:
    // C'tors

    FileInode(string name, int size);
    FileInode(string fileInode);

    // GETTERS

    vector<int> getBlocks() const;

    // OPERATOR

    virtual operator string() override;
    virtual int operator[](int index) override;

    // METHODS

    virtual void add(int index) override;
    virtual void remove(int index) override;

};
