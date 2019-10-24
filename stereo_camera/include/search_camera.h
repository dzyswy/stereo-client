#ifndef __SEARCH_CAMERA_H
#define __SEARCH_CAMERA_H


#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>



class broadcast_client;

struct zscam_node
{
	std::string ip;
	std::string version;
	std::string company;
	std::string board;
	std::string serial_number;
};




class search_camera
{
public:
	search_camera(const char *device_name, int port, int poll_time, int debug = 0);
	~search_camera();
	
	void get_device_nodes(std::vector<std::string> &device_nodes);
	void get_device_nodes(std::vector<struct zscam_node> &device_nodes);

protected:
	int get_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value);

protected:	
	broadcast_client *client_;
	
};














#endif


