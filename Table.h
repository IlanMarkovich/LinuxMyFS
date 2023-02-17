#pragma once

#include <string>
#include <vector>
#include <set>

#include "blkdev.h"
#include "Inode.h"

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
    vector<Inode> _inodes;
    set<int> _avaliable_blocks;
    BlockDeviceSimulator* _blkdevsim;
    const int _headerSize;

    // METHODS

    void readInodesFromBlockDevice();
    void writeInodesToBlockDevice();
    vector<Inode>& getFolderInodes(Inode inode);
    Inode& getInodeFromFolder(string path, vector<Inode>& inodes);

    // OPERATOR

    Inode& operator[](string path);

public:
    // C'tor
    Table(BlockDeviceSimulator* blkdevsim, int headerSize);

    // GETTERS

    vector<Inode> getInodes() const;

    // METHODS

    bool hasName(string name);
    void addInode(string path, bool is_dir);
    string getInodeContent(string name);
    void changeInodeContent(string name, string content);
};
