#include "Table.h"

#include <stdexcept>
#include <algorithm>

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
    for(i++; i < str.size(); i++)
    {
        if(str[i] == ',')
        {
            block.push_back(atoi(num.c_str()));
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
    
    // Iterate the table as string, and convert it to the vector of inodes
    for(char c : table)
    {
        if(c == '~')
        {
            inode node = stringToInode(currInode);
            _inodes.push_back(node);

            // Remove the blocks of the inode from the currently avaliable
            // blocks in the set
            for(int block : node.blocks)
            {
                _avaliable_blocks.erase(_avaliable_blocks.find(block));
            }

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

inode &Table::operator[](string name)
{
    for(inode& node : _inodes)
    {
        if(node.name == name)
        {
            return node;
        }
    }

    throw std::runtime_error("Failed to access file");
}

// PUBLIC METHODS

bool Table::hasName(string name)
{
    for(inode node : _inodes)
    {
        if(node.name == name)
        {
            return true;
        }
    }

    return false;
}

void Table::addInode(string name, bool is_dir)
{
    if(_avaliable_blocks.empty())
    {
        throw std::runtime_error("Not enough memory avaliable");
    }

    inode node = {name, is_dir, vector<int>()};

    // Add a block from the block device to the inode
    int block = *(_avaliable_blocks.begin());
    _avaliable_blocks.erase(_avaliable_blocks.begin());
    node.blocks.push_back(block);

    // Add the inode to the vector and write the vector to the block device
    _inodes.push_back(node);
    writeInodesToBlockDevice();
}

string Table::getInodeContent(string name)
{
    inode node = (*this)[name];
    string content;

    for(int block : node.blocks)
    {
        char* block_content = new char[BLOCK_SIZE];

        // _headerSize + TABLE_SIZE for the address that the memory blocks are starting
        // and the calculation of the block address is BLOCK_SIZE * block
        _blkdevsim->read(_headrSize + TABLE_SIZE + BLOCK_SIZE * block, BLOCK_SIZE, block_content);
        content += block_content;

        delete[] block_content;
    }

    return content;
}

void Table::changeInodeContent(string name, string content)
{
    inode& node = (*this)[name];
    int contentIter = 0;

    // If there are too many blocks for the content
    // Make some of the blocks avaliable for other memory
    if((node.blocks.size() - 1) * BLOCK_SIZE > content.size())
    {
        // Saves the blocks that are needed for the storage of the content
        vector<int> neededBlocks;

        // Writes the blocks to the block device
        for(int i = 0; contentIter < content.size(); i++)
        {
            _blkdevsim->write(_headrSize + TABLE_SIZE + BLOCK_SIZE * node.blocks[i], BLOCK_SIZE, content.c_str() + contentIter);
            contentIter += BLOCK_SIZE;
            neededBlocks.push_back(node.blocks[i]);
        }

        for(int block : node.blocks)
        {
            // If the block is not a needed block
            if(std::find(neededBlocks.begin(), neededBlocks.end(), block) == neededBlocks.end())
            {
                // Makes the block avaliable for othre inodes to use
                node.blocks.erase(std::find(node.blocks.begin(), node.blocks.end(), block));
                _avaliable_blocks.insert(block);
            }
        }
    }
    else 
    {
        // If there are too few blocks for the content
        // the content requires more avaliable memory block from the block device
        if(node.blocks.size() * BLOCK_SIZE < content.size())
        {
            // Calculates the number of needed blocks for the storage of the content
            int neededBlocks = content.size() / BLOCK_SIZE;
            neededBlocks = (content.size() % BLOCK_SIZE == 0) ? neededBlocks : neededBlocks + 1;

            // If there are too few avaliable blocks
            if(_avaliable_blocks.size() < neededBlocks)
            {
                throw std::runtime_error("Not enough memory avaliable");
            }

            int blocksSize = node.blocks.size();

            // Take the avaliable blocks to the inode
            for(int i = 0; i < neededBlocks - blocksSize; i++)
            {
                node.blocks.push_back(*(_avaliable_blocks.begin()));
                _avaliable_blocks.erase(_avaliable_blocks.begin());
            }
        }

        // Write the blocks to the block device
        for(int block : node.blocks)
        {
            _blkdevsim->write(_headrSize + TABLE_SIZE + BLOCK_SIZE * block, BLOCK_SIZE, content.c_str() + contentIter);
            contentIter += BLOCK_SIZE;
        }
    }

    // Updates the block device's table
    writeInodesToBlockDevice();
}
