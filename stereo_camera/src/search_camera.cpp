#include "search_camera.h"



using namespace std;



search_camera::search_camera(const char *device_name, int port, int poll_time)
{
	client_ = new broadcast_client(device_name, port, poll_time);
}

search_camera::~search_camera()
{
	delete client_;
}


void search_camera::get_device_nodes(std::vector<std::string> &device_nodes)
{
	std::map<std::string, std::map<std::string, std::string> > client_nodes;
	client_->get_device_nodes(client_nodes);
	
	device_nodes.clear();
	for (auto it = client_nodes.begin(); it != client_nodes.end(); it++)
	{
		std::string ip = it->first;
		device_nodes.push_back(ip);
	}
}

void search_camera::get_device_nodes(std::vector<struct zscam_node> &device_nodes)
{
	std::map<std::string, std::map<std::string, std::string> > client_nodes;
	client_->get_device_nodes(client_nodes);
	
	device_nodes.clear();
	for (auto it = client_nodes.begin(); it != client_nodes.end(); it++)
	{
		struct zscam_node node;
		node.ip = it->first;
		get_header(it->second, "version", node.version);
		get_header(it->second, "company", node.company);
		get_header(it->second, "board", node.board);
		get_header(it->second, "serial_number", node.serial_number);
		device_nodes.push_back(node);
	}	
	
}

int search_camera::get_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value)
{
	typename std::map<std::string, std::string>::iterator it;
	it = headers.find(key);
	if (it == headers.end())
		return -1;
	value = it->second;
	return 0;
}












