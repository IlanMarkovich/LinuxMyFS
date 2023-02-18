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

	_table->addInode(path_str);
}

std::string MyFs::get_content(std::string path_str) {
	return _table->getInodeContent(path_str);
}

void MyFs::set_content(std::string path_str, std::string content) {
	return _table->changeInodeContent(path_str, content);
}

MyFs::dir_list MyFs::list_dir(std::string path_str) {
	if(path_str != "/")
	{
		throw std::runtime_error("not implemented");
	}

	MyFs::dir_list list;

	// Iterate inodes and convert them to dir_list_entry
	for(Inode inode : _table->getInodes())
	{
		MyFs::dir_list_entry entry = {inode.getName(), false, inode.getSize()};
		list.push_back(entry);
	}

	return list;
}
