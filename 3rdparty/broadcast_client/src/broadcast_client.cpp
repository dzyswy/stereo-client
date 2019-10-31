#include "broadcast_client.h"
#include "broadcast_receiver.h"



using namespace std;







broadcast_client::broadcast_client(const char *device_name, int port, int poll_time, int debug)
{
	impl_ = new broadcast_receiver(device_name, port, poll_time, nodes_, debug);
	
}

broadcast_client::~broadcast_client()
{
	stop();
	delete impl_;
}

void broadcast_client::run()
{
	impl_->run();
}

void broadcast_client::stop()
{
	impl_->stop();
}

void broadcast_client::get_device_nodes(std::map<std::string, bdc_node*> &device_nodes)
{
	nodes_.get_nodes(device_nodes);
}











