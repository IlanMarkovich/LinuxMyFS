#include "Table.h"

// C'tor

Table::Table(BlockDeviceSimulator *blkdevsim, int headerSize) : _blkdevsim(blkdevsim), _startBlockSize(TABLE_SIZE + headerSize)
{
    // Adds all blocks to the avaliable_blocks set at initialize
    for(int i = 0; i < BLOCKS_SIZE; i++)
    {
        _avaliable_blocks.insert(i);
    }
}

void Table::addInode(string name, bool is_dir)
{
    inode node = {name, is_dir, vector<int>()};
    _inodes.push_back(node);
}
