#include "Table.h"

#include <stdexcept>
#include <algorithm>

// C'tor

Table::Table(BlockDeviceSimulator *blkdevsim, int headerSize) : _blkdevsim(blkdevsim), _headerSize(headerSize)
{
    // Adds all blocks to the avaliable_blocks set at initialize
    for(int i = 0; i < BLOCKS_SIZE; i++)
    {
        _avaliable_blocks.insert(i);
    }

    // Initializes the '_inodes' vector with the table written in the block device
    readInodesFromBlockDevice();
}

// GETTERS

vector<Inode> Table::getInodes() const
{
    return _inodes;
}

// PRIVATE METHODS

<<<<<<< HEAD
=======
string Table::inodeToString(inode node)
{
    string ret = node.name + ',';
    ret += std::to_string(node.size) + ',';
    
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

    string size;

    for(i++; str[i] != ','; i++)
    {
        size += str[i];
    }

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

    return (inode){name, atoi(size.c_str()), block};
}

>>>>>>> parent of 5aa2606 (Revert changes)
void Table::readInodesFromBlockDevice()
{
    // Get the table information from the block device to 'table'
    char* tableArr = new char[TABLE_SIZE];
    _blkdevsim->read(_headerSize, TABLE_SIZE, tableArr);
    string table = tableArr;
    delete[] tableArr;

    string currInode;
    
    // Iterate the table as string, and convert it to the vector of inodes
    for(char c : table)
    {
        if(c == '~')
        {
            Inode inode = currInode;
            _inodes.push_back(inode);

            // Remove the blocks of the inode from the currently avaliable
            // blocks in the set
            for(int block : inode.getStorage())
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
    // <name>,<dir><size>,<block>,<block>,....<block>,~
    for(Inode inode : _inodes)
    {
        table += (string)inode + '~';
    }

    _blkdevsim->write(_headerSize, TABLE_SIZE, table.c_str());
}

vector<Inode>& Table::getFolderInodes(Inode inode)
{
    vector<Inode> folderInodes;

    for(int inodeId : inode.getStorage())
    {
        folderInodes.push_back(_inodes[inodeId]);
    }

    return folderInodes;
}

Inode &Table::getInodeFromFolder(string path, vector<Inode>& inodes)
{
    // Trim '/'
    path = path.substr(1, path.size());

    if(std::count(path.begin(), path.end(), '/') != 0)
    {
        // Get the current folder
        string folder;
        std::copy(path.begin(), std::find(path.begin(), path.end(), '/'), folder.begin());

        for(Inode inode : inodes)
        {
            if(inode.isDir() && inode.getName() == folder)
            {
                return getInodeFromFolder(path.substr(folder.size(), path.size()), getFolderInodes(inode));
            }
        }
    }
    else
    {
        for(Inode& inode : inodes)
        {
            if(!inode.isDir() && inode.getName() == path)
            {
                return inode;
            }
        }
    }

    throw std::runtime_error("Failed to access file");
}

Inode &Table::operator[](string path)
{
    // If this is a direct file in the home folder
    // Access it here
    if(path[0] != '/')
    {
        for(Inode inode : _inodes)
        {
            if(inode.getName() == path)
            {
                return inode;
            }
        }
    }

    return getInodeFromFolder(path, _inodes); 
}

// PUBLIC METHODS

bool Table::hasName(string name)
{
    for(Inode inode : _inodes)
    {
        if(inode.getName() == name)
        {
            return true;
        }
    }

    return false;
}

void Table::addInode(string path, bool is_dir)
{
    if(_avaliable_blocks.empty())
    {
        throw std::runtime_error("Not enough memory avaliable");
    }

<<<<<<< HEAD
    Inode inode(path, 0, is_dir);
=======
    inode node = {name, 0, vector<int>()};
>>>>>>> parent of 5aa2606 (Revert changes)

    // Add a block from the block device to the inode
    int block = *(_avaliable_blocks.begin());
    _avaliable_blocks.erase(_avaliable_blocks.begin());
    inode.add(block);

    // Find the last slash in the string
    string slash = "/";
    auto slash_iter = std::find_end(path.begin(), path.end(), slash.begin(), slash.end());

    // Remove the file/directory name from the path
    std::copy(path.begin(), slash_iter, path.begin());

    Inode direcory_inode = (*this)[path];
    direcory_inode.add(inode.)

    // Add the inode to the vector and write the vector to the block device
    _inodes.push_back(inode);
    writeInodesToBlockDevice();
}

string Table::getInodeContent(string name)
{
    Inode inode = (*this)[name];
    string content;

    for(int block : inode.getStorage())
    {
        char* block_content = new char[BLOCK_SIZE];

        // _headerSize + TABLE_SIZE for the address that the memory blocks are starting
        // and the calculation of the block address is BLOCK_SIZE * block
        _blkdevsim->read(_headerSize + TABLE_SIZE + BLOCK_SIZE * block, BLOCK_SIZE, block_content);
        content += block_content;

        delete[] block_content;
    }

    return content;
}

void Table::changeInodeContent(string name, string content)
{
    Inode& inode = (*this)[name];
    int contentIter = 0;

    // If there are too many blocks for the content
    // Make some of the blocks avaliable for other memory
    if((inode.getStorage().size() - 1) * BLOCK_SIZE > content.size())
    {
        // Saves the blocks that are needed for the storage of the content
        vector<int> neededBlocks;

        // Writes the blocks to the block device
        for(int i = 0; contentIter < content.size(); i++)
        {
            _blkdevsim->write(_headerSize + TABLE_SIZE + BLOCK_SIZE * inode[i], BLOCK_SIZE, content.c_str() + contentIter);
            contentIter += BLOCK_SIZE;
            neededBlocks.push_back(inode[i]);
        }

        for(int block : inode.getStorage())
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
        if(inode.getStorage().size() * BLOCK_SIZE < content.size())
        {
            // Calculates the number of needed blocks for the storage of the content
            int neededBlocks = content.size() / BLOCK_SIZE;
            neededBlocks = (content.size() % BLOCK_SIZE == 0) ? neededBlocks : neededBlocks + 1;

            // If there are too few avaliable blocks
            if(_avaliable_blocks.size() < neededBlocks)
            {
                throw std::runtime_error("Not enough memory avaliable");
            }

            int blocksSize = inode.getStorage().size();

            // Take the avaliable blocks to the inode
            for(int i = 0; i < neededBlocks - blocksSize; i++)
            {
                inode.add(*(_avaliable_blocks.begin()));
                _avaliable_blocks.erase(_avaliable_blocks.begin());
            }
        }

        // Write the blocks to the block device
        for(int block : inode.getStorage())
        {
            _blkdevsim->write(_headerSize + TABLE_SIZE + BLOCK_SIZE * block, BLOCK_SIZE, content.c_str() + contentIter);
            contentIter += BLOCK_SIZE;
        }
    }

    // Update inode's size
    inode.setSize(content.size());

    // Updates the block device's table
    writeInodesToBlockDevice();
}
