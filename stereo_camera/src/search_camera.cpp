#include "search_camera.h"
#include "broadcast_client.h"


using namespace std;



search_camera::search_camera(const char *device_name, int port, int poll_time, int debug)
{
	client_ = new broadcast_client(device_name, port, poll_time, debug);
}

search_camera::~search_camera()
{
	delete client_;
}


void search_camera::get_device_nodes(std::vector<std::string> &device_nodes)
{
	std::map<std::string, bdc_node*> client_nodes;
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
	std::map<std::string, bdc_node*> client_nodes;
	client_->get_device_nodes(client_nodes);
 
	device_nodes.clear();
	for (auto it = client_nodes.begin(); it != client_nodes.end(); it++)
	{
		struct zscam_node node;
		std::map<std::string, std::string> headers = it->second->get_headers();
		node.ip = it->first;
		node.board = headers["board"];
		node.company = headers["company"];
		node.version = headers["version"];
		node.serial_number = headers["serial_number"];
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












