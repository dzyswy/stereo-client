#include "discovery_receiver.h"
#include <asio.hpp>
#include <asio/steady_timer.hpp>


using namespace std;
using namespace asio;
using namespace asio::ip;

class discovery_receiver_impl
{
public:
	discovery_receiver_impl(const char *device_name, int port, int poll_time);
	~discovery_receiver_impl();
	void get_device_nodes(std::vector<std::string> &device_nodes);

protected:	
	void do_receive();
	void add_node(std::string dev_ip, int max_age);
	void do_timer();
	

protected:	
	asio::io_context io_context_;
	asio::ip::udp::socket socket_;
	asio::ip::udp::endpoint sender_endpoint_;
	asio::steady_timer timer_;
	
	enum { max_length = 1024 };
	char data_[max_length];
	
	std::string device_name_;
	int poll_time_;
	
	std::mutex mux_;
    std::list<struct device_node> device_nodes_;
	
	std::thread *run_thread_;	
};

discovery_receiver_impl::discovery_receiver_impl(const char *device_name, int port, int poll_time) : 
	socket_(io_context_),
	timer_(io_context_)
{
	device_name_ = device_name;
	poll_time_ = poll_time;
	

	udp::endpoint listen_endpoint(address_v4::from_string("0.0.0.0"), port);
    socket_.open(listen_endpoint.protocol());
    socket_.set_option(udp::socket::reuse_address(true));
    socket_.bind(listen_endpoint);
	
	do_receive();
	do_timer();
	
	run_thread_ = new std::thread([this] () {io_context_.run();});

}

discovery_receiver_impl::~discovery_receiver_impl()
{
	io_context_.stop();
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
	}	
}

void discovery_receiver_impl::get_device_nodes(std::vector<std::string> &device_nodes)
{
	std::unique_lock<std::mutex> lock(mux_);
	device_nodes.clear();
	for (list<struct device_node>::iterator it = device_nodes_.begin(); it != device_nodes_.end(); ++it)
	{
		device_nodes.push_back(it->ip);
	}
}

void discovery_receiver_impl::do_receive()
{
	socket_.async_receive_from(asio::buffer(data_, max_length), sender_endpoint_,
		[this](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				stringstream rs;
				rs.write(data_, length);
				
				string dev_msg, age_msg;
				getline(rs, dev_msg);
				getline(rs, age_msg);
				if (dev_msg.size() && (dev_msg.substr(0, 7) == "device:") && (dev_msg.substr(8, dev_msg.length()-9) == device_name_))
				{
					if (age_msg.size() && (age_msg.substr(0, 8) == "max-age:"))
					{
						string str_age = age_msg.substr(9, age_msg.length()-10);
						int max_age = atoi(str_age.c_str());
						string dev_ip = sender_endpoint_.address().to_string();
						add_node(dev_ip, max_age);
					}	
				}	

				do_receive();
			}	
		});
}




void discovery_receiver_impl::add_node(string dev_ip, int max_age)
{
	int flag = 0;
	list<struct device_node> ::iterator it;
	for (it = device_nodes_.begin(); it != device_nodes_.end(); it++)
	{
		struct device_node &node = *it;
		if (node.ip == dev_ip)
		{
			node.timeout = time(NULL) + max_age;
			flag = 1;
		}
	}
	
	if (!flag) {
		struct device_node new_node;
		new_node.ip = dev_ip;
		new_node.timeout = time(NULL) + max_age;
		 
		{
			std::unique_lock<std::mutex> lock(mux_);
			device_nodes_.push_back(new_node);
		}
	}
}

void discovery_receiver_impl::do_timer()
{
	timer_.expires_from_now(std::chrono::milliseconds(poll_time_));
	timer_.async_wait([this](std::error_code ec) {
			if (!ec) 
			{
				list<struct device_node> ::iterator it = device_nodes_.begin();
				while (it != device_nodes_.end())
				{
					std::list<struct device_node>::iterator iter = it++;
					struct device_node node = *iter;
					time_t now_time = time(NULL);
					if ((now_time - node.timeout) > 1)
					{
						{
							std::unique_lock<std::mutex> lock(mux_);
							device_nodes_.erase(iter);
						}
					}
				}
			}
			do_timer();
		});
}
 











discovery_receiver::discovery_receiver(const char *device_name, int port, int poll_time)
{
	impl_ = new discovery_receiver_impl(device_name, port, poll_time);
}

discovery_receiver::~discovery_receiver()
{
	delete impl_;
}

void discovery_receiver::get_device_nodes(std::vector<std::string> &device_nodes)
{
	impl_->get_device_nodes(device_nodes);
}

/*
discovery_receiver::discovery_receiver(const char *device_name, int port, int poll_time)
{
	device_name_ = device_name;
	poll_time_ = poll_time;
	
	
	io_context_ = new io_context;
	socket_ = new udp::socket(*io_context_);
	timer_ = new asio::steady_timer(*io_context_);
	
	
	udp::endpoint listen_endpoint(address_v4::from_string("0.0.0.0"), port);
    socket_->open(listen_endpoint.protocol());
    socket_->set_option(udp::socket::reuse_address(true));
    socket_->bind(listen_endpoint);
	
	sender_endpoint_ = new udp::endpoint;
	
	do_receive();
	do_timer();
	
	run_thread_ = new std::thread(std::bind(&asio::io_service::run, io_context_), this);
}

discovery_receiver::~discovery_receiver()
{
	io_context_->stop();
	if (run_thread_)
	{
		run_thread_.join();
		delete run_thread_;
	}	
}

void discovery_receiver::do_receive()
{
	socket_->async_receive_from(asio::buffer(data_, max_length), *sender_endpoint_,
		[this](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				stringstream rs;
				rs.write(data_, length);
				
				string dev_msg, age_msg;
				getline(rs, dev_msg);
				getline(rs, age_msg);
				if (dev_msg.size() && (dev_msg.substr(0, 7) == "device:") && (dev_msg.substr(8, dev_msg.length()-9) == device_name_))
				{
					if (age_msg.size() && (age_msg.substr(0, 8) == "max-age:"))
					{
						string str_age = age_msg.substr(9, age_msg.length()-10);
						int max_age = atoi(str_age.c_str());
						string dev_ip = sender_endpoint_.address().to_string();
						add_node(dev_ip, max_age);
					}	
				}	

				do_receive();
			}	
		});
}




void discovery_receiver::add_node(string dev_ip, int max_age)
{
	int flag = 0;
	list<struct device_node> ::iterator it;
	for (it = device_nodes_.begin(); it != device_nodes_.end(); it++)
	{
		struct device_node &node = *it;
		if (node.ip == dev_ip)
		{
			node.timeout = time(NULL) + max_age;
			flag = 1;
		}
	}
	
	if (!flag) {
		struct device_node new_node;
		new_node.ip = dev_ip;
		new_node.timeout = time(NULL) + max_age;
		 
		{
			std::unique_lock<std::mutex> lock(mux_);
			device_nodes_.push_back(new_node);
		}
	}
}

void discovery_receiver::do_timer()
{
	timer_->expires_from_now(std::chrono::milliseconds(poll_time_));
	timer_->async_wait([this](std::error_code ec) {
			if (!ec) 
			{
				list<struct device_node> ::iterator it = device_nodes_.begin();
				while (it != device_nodes_.end())
				{
					std::list<struct device_node>::iterator iter = it++;
					struct device_node node = *iter;
					time_t now_time = time(NULL);
					if ((now_time - node.timeout) > 1)
					{
						{
							std::unique_lock<std::mutex> lock(mux_);
							device_nodes_.erase(iter);
						}
					}
				}
			}
			do_timer();
		});
}




void discovery_receiver::get_device_nodes(std::vector<std::string> &device_nodes)
{
	std::unique_lock<std::mutex> lock(mux_);
	device_nodes.clear();
	for (list<struct device_node>::iterator it = device_nodes_.begin(); it != device_nodes_.end(); ++it)
	{
		device_nodes.push_back(it->ip);
	}
}
*/












