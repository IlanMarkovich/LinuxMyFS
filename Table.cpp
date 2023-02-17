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

Table::~Table()
{
    for(Inode* inode : _inodes)
    {
        delete inode;
    }
}

// GETTERS

vector<Inode*> Table::getInodes() const
{
    return _inodes;
}

// PRIVATE METHODS

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
            FileInode* inode = new FileInode(currInode);
            _inodes.push_back(inode);

            // Remove the blocks of the inode from the currently avaliable
            // blocks in the set
            for(int block : inode->getBlocks())
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
    // <name>,<size>,<block>,<block>,....<block>,~
    for(Inode* inode : _inodes)
    {
        table += (string)(*inode) + '~';
    }

    _blkdevsim->write(_headrSize, TABLE_SIZE, table.c_str());
}

Inode* Table::operator[](string name)
{
    for(Inode* inode : _inodes)
    {
        if(inode->getName() == name)
        {
            return inode;
        }
    }

    throw std::runtime_error("Failed to access file");
}

// PUBLIC METHODS

bool Table::hasName(string name)
{
    for(Inode* inode : _inodes)
    {
        if(inode->getName() == name)
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

    FileInode* inode = new FileInode(name, 0);

    // Add a block from the block device to the inode
    int block = *(_avaliable_blocks.begin());
    _avaliable_blocks.erase(_avaliable_blocks.begin());
    inode->add(block);

    // Add the inode to the vector and write the vector to the block device
    _inodes.push_back(inode);
    writeInodesToBlockDevice();
}

string Table::getInodeContent(string name)
{
    FileInode inode = *(FileInode*)(*this)[name];
    string content;

    for(int block : inode.getBlocks())
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
    FileInode& inode = *(FileInode*)(*this)[name];
    int contentIter = 0;

    // If there are too many blocks for the content
    // Make some of the blocks avaliable for other memory
    if((inode.getBlocks().size() - 1) * BLOCK_SIZE > content.size())
    {
        // Saves the blocks that are needed for the storage of the content
        vector<int> neededBlocks;

        // Writes the blocks to the block device
        for(int i = 0; contentIter < content.size(); i++)
        {
            _blkdevsim->write(_headrSize + TABLE_SIZE + BLOCK_SIZE * inode[i], BLOCK_SIZE, content.c_str() + contentIter);
            contentIter += BLOCK_SIZE;
            neededBlocks.push_back(inode[i]);
        }

        for(int block : inode.getBlocks())
        {
            // If the block is not a needed block
            if(std::find(neededBlocks.begin(), neededBlocks.end(), block) == neededBlocks.end())
            {
                // Removes the block from the inode and make the block avaliable for other inodes to use
                inode.remove(block);
                _avaliable_blocks.insert(block);
            }
        }
    }
    else 
    {
        // If there are too few blocks for the content
        // the content requires more avaliable memory blocks from the block device
        if(inode.getBlocks().size() * BLOCK_SIZE < content.size())
        {
            // Calculates the number of needed blocks for the storage of the content
            int neededBlocks = content.size() / BLOCK_SIZE;
            neededBlocks = (content.size() % BLOCK_SIZE == 0) ? neededBlocks : neededBlocks + 1;

            // If there are too few avaliable blocks
            if(_avaliable_blocks.size() < neededBlocks)
            {
                throw std::runtime_error("Not enough memory avaliable");
            }

            int blocksSize = inode.getBlocks().size();

            // Take the avaliable blocks to the inode
            for(int i = 0; i < neededBlocks - blocksSize; i++)
            {
                inode.add(*(_avaliable_blocks.begin()));
                _avaliable_blocks.erase(_avaliable_blocks.begin());
            }
        }

        // Write the blocks to the block device
        for(int block : inode.getBlocks())
        {
            _blkdevsim->write(_headrSize + TABLE_SIZE + BLOCK_SIZE * block, BLOCK_SIZE, content.c_str() + contentIter);
            contentIter += BLOCK_SIZE;
        }
    }

    // Update inode's size
    inode.setSize(content.size());

    // Updates the block device's table
    writeInodesToBlockDevice();
}
