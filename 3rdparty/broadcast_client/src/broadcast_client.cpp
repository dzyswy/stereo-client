#include "broadcast_client.h"
#include "broadcast_receiver.h"



using namespace std;







broadcast_client::broadcast_client(const char *device_name, int port, int poll_time, int debug)
{
	impl_ = new broadcast_receiver(device_name, port, poll_time, debug);
}

broadcast_client::~broadcast_client()
{
	delete impl_;
}

void broadcast_client::get_device_nodes(std::map<std::string, std::map<std::string, std::string> > &device_nodes)
{
	std::map<std::string, struct bdc_dev_node> bdc_nodes;
	bdc_nodes.clear();
	impl_->get_device_nodes(bdc_nodes);
	
	std::map<std::string, std::map<std::string, std::string> > nodes;
	for (auto it = bdc_nodes.begin(); it != bdc_nodes.end(); ++it)
	{
		string ip = it->first;
		std::map<std::string, std::string> &headers = it->second.headers;
		
		nodes.insert(make_pair(ip, headers)); 
	}	
	device_nodes = nodes;
}











