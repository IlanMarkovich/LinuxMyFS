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
    const int _headrSize;

    // METHODS

    /// @brief Reads the inodes from the block device's table to the _inodes vector
    void readInodesFromBlockDevice();

    /// @brief Writes inodes from the _inodes vector to the block device's table
    void writeInodesToBlockDevice();

    /// @brief Gets a reference to an inodes from the _inodes vector
    /// @param name The name of the requested inodes
    Inode& operator[](string name);

public:
    // C'tor
    Table(BlockDeviceSimulator* blkdevsim, int headerSize);

    // GETTERS

    vector<Inode> getInodes() const;

    // METHODS

    /// @brief Adds an inodes to the block device's table and the _inodes vecotr
    /// @param name The name of the new inode
    void addInode(string name);

    /// @brief Gets the content of an inode
    /// @param name The name of the requested inode
    string getInodeContent(string name);

    /// @brief Change the content of an existing inode
    /// @param name The name of the requested inode
    /// @param content The new content that will be inserted in the inode's memroy
    void changeInodeContent(string name, string content);
};
