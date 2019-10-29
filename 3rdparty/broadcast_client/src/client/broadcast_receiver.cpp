#include "broadcast_receiver.h"



using namespace std;




broadcast_receiver::broadcast_receiver(const char *device_name, int port, int poll_time, bdc_nodes& nodes, int debug) : 
	socket_(io_context_),
	timer_(io_context_), 
	device_nodes_(nodes)
{
	debug_ = debug;
	device_name_ = device_name;
	poll_time_ = poll_time;
	
	if (debug_) {
		cout << "broadcast_receiver: " << device_name_ << " " << port << " " << poll_time << endl;
	}
	
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
				 
				std::map<std::string, std::string> headers;
				headers.clear();
				std::string header;
				while (std::getline(rs, header))
				{
					if (header == "\r")
						break;
					
					std::string key;
					std::string value;
					
					//Content-type: image/jpeg
					const char *p = strchr(header.c_str(), ':');
					if (p == NULL)
						continue;
					long len_key = (long)p - (long)header.c_str();//exclude :
					long len_val = header.length() - len_key - 3;//exclude :space \r
					key = header.substr(0, len_key);
					value = header.substr(len_key + 2, len_val);
					
					set_header(headers, key, value);
					
					if (debug_) {
						cout << header << endl;
						cout << "key: " << key << endl;
						cout << "value: " << value << endl;
					}
				}
				
				string device_name = headers["device"];
				int max_age = atoi(headers["max-age"].c_str());
				if (max_age < poll_time_)
					max_age = poll_time_ * 2;
				
				
				if (strcmp(device_name.c_str(), device_name_.c_str()) == 0)
				{
					string dev_ip = sender_endpoint_.address().to_string();
					time_t deadline = time(NULL) + max_age; 
					device_nodes_.add_node(dev_ip, deadline, headers);
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
				device_nodes_.delete_node();
			}
			do_timer();
		});
}


void broadcast_receiver::set_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value)
{
	typename std::map<std::string, std::string>::iterator it;
	it = headers.find(key);
	if (it == headers.end()) {
		headers.insert(make_pair(key, value));
	} else {
		it->second = value;
	} 
}

int broadcast_receiver::get_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value)
{ 
	typename std::map<std::string, std::string>::iterator it;
	it = headers.find(key);
	if (it == headers.end()) {
		if (debug_) {
			cout << "broadcast_receiver::get_header no key: " << key << endl;
		}
		return -1;
	}
 
	value = it->second;
	if (debug_) {
		cout << "broadcast_receiver::get_header key=" << key << ", value=" << value << endl;
	}
	
	return 0;
}



















