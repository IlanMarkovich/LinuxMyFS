#include "myfs.h"
#include <string.h>
#include <iostream>
#include <math.h>
#include <sstream>

const char *MyFs::MYFS_MAGIC = "MYFS";

MyFs::MyFs(BlockDeviceSimulator *blkdevsim_) : blkdevsim(blkdevsim_) 
{
	struct myfs_header header;
	blkdevsim->read(0, sizeof(header), (char *)&header);

	if (strncmp(header.magic, MYFS_MAGIC, sizeof(header.magic)) != 0 ||
	    (header.version != CURR_VERSION)) {
		std::cout << "Did not find myfs instance on blkdev" << std::endl;
		std::cout << "Creating..." << std::endl;
		format();
		std::cout << "Finished!" << std::endl;
	}

	_table = new Table(blkdevsim, sizeof(header));
}

MyFs::~MyFs()
{
	delete _table;
}

void MyFs::format() {

	// put the header in place
	struct myfs_header header;
	strncpy(header.magic, MYFS_MAGIC, sizeof(header.magic));
	header.version = CURR_VERSION;
	blkdevsim->write(0, sizeof(header), (const char*)&header);
}

void MyFs::create_file(std::string path_str, bool directory) {
	if(directory)
	{
		throw std::runtime_error("not implemented");
	}

	// Check if the file already exists
	if(_table->hasName(path_str))
	{
		throw std::runtime_error("Name already exists");
	}

	_table->addInode(path_str, directory);
}

std::string MyFs::get_content(std::string path_str) {
	// Check if the file doesn't exists
	if(!_table->hasName(path_str))
	{
		throw std::runtime_error("Couldn't find file!");
	}

	return _table->getInodeContent(path_str);
}

void MyFs::set_content(std::string path_str, std::string content) {
	// Check if the file doesn't exists
	if(!_table->hasName(path_str))
	{
		throw std::runtime_error("Couldn't find file!");
	}

	return _table->changeInodeContent(path_str, content);
}

MyFs::dir_list MyFs::list_dir(std::string path_str) {
	if(path_str != "/")
	{
		throw std::runtime_error("not implemented");
	}

	vector<inode> inodes = _table->getInodes();
	MyFs::dir_list list;

	for(inode node : inodes)
	{
		MyFs::dir_list_entry entry = {node.name, node.is_dir, 0};
		list.push_back(entry);
	}

	return list;
}
