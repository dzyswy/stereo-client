#include "bdc_node.h"



using namespace std;





bdc_nodes::bdc_nodes()
{
	
}

bdc_nodes::~bdc_nodes()
{
	for (auto it = nodes_.begin(); it != nodes_.end(); it++)
	{
		delete it->second;
	}	
	nodes_.clear();
}


void bdc_nodes::add_node(std::string ip, time_t deadline, std::map<std::string, std::string> &headers)
{ 
	std::unique_lock<std::mutex> lock(mux_);
	typename std::map<std::string, bdc_node*>::iterator it;
	it = nodes_.find(ip);
	if (it == nodes_.end()) {
		bdc_node *node = new bdc_node(ip, deadline, headers);
		nodes_.insert(make_pair(ip, node));
	} else {
		it->second->set_ip(ip);
		it->second->set_deadline(deadline);
		it->second->set_headers(headers);
	} 

}

void bdc_nodes::delete_node()
{
	std::unique_lock<std::mutex> lock(mux_);
	for (auto it = nodes_.begin(); it != nodes_.end();)
	{
		time_t now_time = time(NULL);
		time_t deadline = it->second->get_deadline();
		if (now_time > deadline)
		{
			delete it->second;
			nodes_.erase(it++);
		}	
		else
			it++;
	}	
}


void bdc_nodes::get_nodes(std::map<std::string, bdc_node*> &value)
{
	std::unique_lock<std::mutex> lock(mux_);
	value = nodes_;
}



















