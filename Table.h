#pragma once

#include <string>
#include <vector>
#include <set>

#include "blkdev.h"
#include "Inode.h"
#include "FileInode.h"

#define TABLE_SIZE 1024
#define BLOCK_SIZE 128
#define BLOCKS_SIZE (BlockDeviceSimulator::DEVICE_SIZE - TABLE_SIZE) / BLOCK_SIZE

using std::string;
using std::vector;
using std::set;

class Table
{
private:
    // FIELDS
    vector<Inode*> _inodes;
    set<int> _avaliable_blocks;
    BlockDeviceSimulator* _blkdevsim;
    const int _headrSize;

    // METHODS

    void readInodesFromBlockDevice();
    void writeInodesToBlockDevice();
    Inode* operator[](string name);

public:
    // C'tor
    Table(BlockDeviceSimulator* blkdevsim, int headerSize);

    // D'tor
    ~Table();

    // GETTERS

    vector<Inode*> getInodes() const;

    // METHODS

    bool hasName(string name);
    void addInode(string name, bool is_dir);
    string getInodeContent(string name);
    void changeInodeContent(string name, string content);
};
