#include "Table.h"

// C'tor

Table::Table(BlockDeviceSimulator *blkdevsim, int headerSize) : _blkdevsim(blkdevsim), _headrSize(headerSize)
{
    // Adds all blocks to the avaliable_blocks set at initialize
    for(int i = 0; i < BLOCKS_SIZE; i++)
    {
        _avaliable_blocks.insert(i);
    }

    // Initializes the '_inodes' vector with the table written in the block device
    readInodesFromBlockDevice();
}

// PRIVATE METHODS

string Table::inodeToString(inode node)
{
    string ret = node.name + ',';
    ret += (node.is_dir ? 'd' : 'f');
    
    for(int block : node.blocks)
    {
        ret += std::to_string(block) + ',';
    }

    return ret;
}

inode Table::stringToInode(string str)
{
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
            block.push_back(atoi(&str[i]));
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
    // Get the table information from the block device to 'table'
    char* tableArr = new char[TABLE_SIZE];
    _blkdevsim->read(_headrSize, TABLE_SIZE, tableArr);
    string table = tableArr;
    delete[] tableArr;

    string currInode;
    
    // Iterates the table as string, and converts it to the vector
    // of inodes
    for(char c : table)
    {
        if(c == '~')
        {
            inode node = stringToInode(currInode);
            _inodes.push_back(node);
            currInode = "";
        }
        else
        {
            currInode += c;
        }
    }
}

void Table::writeInodesToBlockDevice()
{
    string table;

    // Writes the inodes as string in the following format:
    // <name>,<dir><block>,<block>,....<block>,~
    for(inode node : _inodes)
    {
        table += inodeToString(node) + '~';
    }

    _blkdevsim->write(_headrSize, TABLE_SIZE, table.c_str());
}

// PUBLIC METHODS

void Table::addInode(string name, bool is_dir)
{
    inode node = {name, is_dir, vector<int>()};

    // Add a block from the block device to the inode
    int block = *(_avaliable_blocks.begin());
    _avaliable_blocks.erase(_avaliable_blocks.begin());
    node.blocks.push_back(block);

    // Add the inode to the vector and write the vector to the block device
    _inodes.push_back(node);
    writeInodesToBlockDevice();
}
