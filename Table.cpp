#include "Table.h"

// C'tor

Table::Table(BlockDeviceSimulator *blkdevsim, int headerSize) : _blkdevsim(blkdevsim), _headrSize(headerSize)
{
    // Adds all blocks to the avaliable_blocks set at initialize
    for(int i = 0; i < TABLE_BLOCKS_SIZE; i++)
    {
        _avaliable_blocks.insert(i);
    }
}

// PRIVATE METHODS

char *Table::inodeToCharArr(inode node)
{
    string ret = node.name + ',';
    ret += (node.is_dir ? 'd' : 'f');
    
    for(int block : node.blocks)
    {
        ret += std::to_string(block) + ',';
    }

    return ret.c_str();
}

inode Table::charArrToInode(char *inode_arr)
{
    string str = inode_arr;
    string name;
    int i = 0;

    // Read name of inode
    for(; str[i] != ','; i++)
    {
        name += str[i];
    }

    // Get if the inode represents a directory or a file
    bool dir = str[++i] == 'd';
    vector<int> block;
    string num;

    // Get the blocks of the inode
    for(; i < str.size(); i++)
    {
        if(str[i] == ',')
        {
            block.push_back(atoi(str[i]));
            num = "";
        }
        else
        {
            num += str[i];
        }
    }

    return (inode){name, dir, block};
}

void Table::readInodesFromBlockDevice()
{

}

// PUBLIC METHODS

void Table::addInode(string name, bool is_dir)
{
    inode node = {name, is_dir, vector<int>()};

    // Add a block of the block device to the inode
    int block = *(_avaliable_blocks.begin());
    _avaliable_blocks.erase(_avaliable_blocks.begin());
    node.blocks.push_back(block);

    _inodes.push_back(node);
}
