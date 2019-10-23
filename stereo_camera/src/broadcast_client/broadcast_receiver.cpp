#include "broadcast_receiver.h"



using namespace std;




broadcast_receiver::broadcast_receiver(const char *device_name, int port, int poll_time) : 
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

broadcast_receiver::~broadcast_receiver()
{
	io_context_.stop();
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
	}	
	clear_device_nodes();
	
}

void broadcast_receiver::get_device_nodes(std::map<std::string, struct bdc_dev_node> &device_nodes)
{
	std::unique_lock<std::mutex> lock(mux_);
	device_nodes = device_nodes_;
}

void broadcast_receiver::do_receive()
{
	socket_.async_receive_from(asio::buffer(data_, max_length), sender_endpoint_,
		[this](std::error_code ec, std::size_t length)
		{
			int ret;
			if (!ec)
			{
				stringstream rs;
				rs.write(data_, length);
				
				std::string header;
				std::map<std::string, std::string> headers;
				headers.clear();
				while (std::getline(rs, header))
				{
				//	cout << header;
					std::string key;
					std::string value;
					
					//Content-type: image/jpeg
					const char *p = strchr(header.c_str(), ':');
					if (p == NULL)
						continue;
					long len_key = (long)p - (long)header.c_str();//exclude :
					long len_val = header.length() - len_key - 2;//exclude :space
					key = header.substr(0, len_key);
					value = header.substr(len_key + 2, len_val);
					
					headers.insert(make_pair(key, value));
				}
				
				string device_name = "";
				ret = get_header(headers, "device", device_name);
				if (ret == 0)
				{
					if (strcmp(device_name.c_str(), device_name_.c_str()) == 0)
					{ 
						string max_age_s = "";
						ret = get_header(headers, "max-age", max_age_s);
						if (ret == 0)
						{
							int max_age = atoi(max_age_s.c_str());
							if (max_age < poll_time_)
								max_age = poll_time_ * 2;
							string dev_ip = sender_endpoint_.address().to_string();
							time_t timeout = time(NULL) + max_age;
							
							set_device_node(dev_ip, timeout, headers);
						}	
					}	
				}	
		

				do_receive();
			}	
		});
}




void broadcast_receiver::do_timer()
{
	timer_.expires_from_now(std::chrono::seconds(poll_time_));
	timer_.async_wait([this](std::error_code ec) {
			if (!ec) 
			{ 
				erase_timeout_device_node();
			}
			do_timer();
		});
}

void broadcast_receiver::erase_timeout_device_node()
{
	std::unique_lock<std::mutex> lock(mux_);
	
	for (auto it = device_nodes_.begin(); it != device_nodes_.end();)
	{
		time_t now_time = time(NULL);
		if (now_time > it->second.timeout)
		{
			it->second.headers.clear();
			device_nodes_.erase(it++);
		}	
		else
			it++;
	}	
}
 
void broadcast_receiver::set_device_node(std::string ip, time_t timeout, std::map<std::string, std::string> &headers)
{
	struct bdc_dev_node value = {0};
	value.timeout = timeout;
	value.headers = headers;
	
	std::unique_lock<std::mutex> lock(mux_);
	typename std::map<std::string, struct bdc_dev_node>::iterator it;
	it = device_nodes_.find(ip);
	if (it == device_nodes_.end()) {
		device_nodes_.insert(make_pair(ip, value));
	} else {
		it->second = value;
	} 
}


int broadcast_receiver::get_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value)
{
	std::unique_lock<std::mutex> lock(mux_);
	typename std::map<std::string, std::string>::iterator it;
	it = headers.find(key);
	if (it == headers.end())
		return -1;
	value = it->second;
	return 0;
}

void broadcast_receiver::clear_device_nodes()
{
	for (auto it = device_nodes_.begin(); it != device_nodes_.end(); ++it)
	{
		it->second.headers.clear();
	}	
	device_nodes_.clear();
}


















