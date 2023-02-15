#pragma once

#include <string>
#include <vector>
#include <set>

#include "blkdev.h"

#define TABLE_SIZE 1024
#define BLOCK_SIZE 128
#define BLOCKS_SIZE (BlockDeviceSimulator::DEVICE_SIZE - TABLE_SIZE) / BLOCK_SIZE

using std::string;
using std::vector;
using std::set;

struct inode
{
    string name;
    bool is_dir;
    vector<int> blocks;
};

class Table
{
private:
    // FIELDS
    set<int> _avaliable_blocks;
    BlockDeviceSimulator* _blkdevsim;
    const int _startBlockSize;

public:
    // C'tor
    Table(BlockDeviceSimulator* blkdevsim, int headerSize);

    // METHODS

    void addInode(string name, bool is_dir, char* content);
};
