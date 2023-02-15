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

void Table::addInode(string name, bool is_dir, char* content)
{
    inode node = {name, is_dir, vector<int>()};
    int numBlocks = sizeof(content) / BLOCK_SIZE;
    int contentIter = 0;

    for(int i = 0; i < numBlocks; i++)
    {
        // Pop the first avaliable block from the set
        int block = *(_avaliable_blocks.begin());
        _avaliable_blocks.erase(_avaliable_blocks.begin());

        // Add the block to the blocks of this inode
        node.blocks.push_back(block);

        // Write the content to the block device
        _blkdevsim->write(_startBlockSize + BLOCK_SIZE * block, BLOCK_SIZE, content + contentIter);
        contentIter += BLOCK_SIZE;
    }
}
