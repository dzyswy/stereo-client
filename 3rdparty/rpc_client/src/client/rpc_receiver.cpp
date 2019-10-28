#include "rpc_receiver.h"


using namespace std;





rpc_receiver::rpc_receiver(std::string ip, int port, std::string request, int debug)
	: socket_(io_context_)
{
	ret_ = -1;
	result_ = "";
	debug_ = debug;
	request_s_ = request;
	
	std::ostream request_stream(&request_);
	request_stream << "GET " << request << " HTTP/1.0\r\n";
	request_stream << "Host: " << ip << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";
	
	
	ip::tcp::endpoint ep(address_v4::from_string(ip), port); 
	
	do_connect(ep);
	
	run_thread_ = new std::thread([this] () {io_context_.run();});
}

rpc_receiver::~rpc_receiver()
{
	socket_.close();
	io_context_.stop();
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
	}
}


void rpc_receiver::do_connect(ip::tcp::endpoint &ep)
{
	socket_.async_connect(ep, 
        [this] (asio::error_code ec) 
		{
			if (!ec) 
			{
				do_write_request();
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}	
			
		});
}

void rpc_receiver::do_write_request()
{
	asio::async_write(socket_, request_,
		[this] (asio::error_code ec, std::size_t length) 
		{
			if (!ec)
			{
				do_read_status_line();
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}			
		});
}

void rpc_receiver::do_read_status_line()
{
	asio::async_read_until(socket_, response_, "\r\n",
        [this] (asio::error_code ec, std::size_t n) 
		{
			if (!ec)
			{
				// Check that response is OK.
				std::istream response_stream(&response_);
				std::string http_version;
				response_stream >> http_version;
				unsigned int status_code;
				response_stream >> status_code;
				std::string status_message;
				std::getline(response_stream, status_message);
				if (!response_stream || http_version.substr(0, 5) != "HTTP/")
				{
					std::cout << "Invalid response\n";
					return;
				}
				if (status_code != 200)
				{
					std::cout << "Response returned with status code ";
					std::cout << status_code << "\n";
					return;
				}
				do_read_headers();
				
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}		
		});
}

void rpc_receiver::do_read_headers()
{
	asio::async_read_until(socket_, response_, "\r\n\r\n",
        [this] (asio::error_code ec, std::size_t n) 
		{
			int ret;
			if (!ec)
			{
				std::istream response_stream(&response_);
				std::map<std::string, std::string> headers;
				headers.clear();
				std::string header;
				while (std::getline(response_stream, header) && header != "\r")
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
				
				string ret_s = "";
				get_header(headers, "ret", ret_s);
				string result_s = "";
				get_header(headers, "result", result_s);
				
				if (ret_s.size())
				{
					std::unique_lock<std::mutex> lock(mux_);
					ret_ = atoi(ret_s.c_str());
					result_ = result_s;
					cond_.notify_all();
				}	

				// Write whatever content we already have to output.
				if (response_.size() > 0)
					std::cout << &response_;
				
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}		
		});
}




int rpc_receiver::get_result(std::string &result, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (cond_.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
	
	if (ret_ < 0) {
		if (ret_ == -2) {
			printf("Do not support request: %s\n", request_s_.c_str());
		} else if (ret_ == -3) {
			printf("request: %s format is wrong\n", request_s_.c_str());
		}
		
		return -1;
	}
	
	result = result_;
 
	return 0;
}




void rpc_receiver::set_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value)
{
	typename std::map<std::string, std::string>::iterator it;
	it = headers.find(key);
	if (it == headers.end()) {
		headers.insert(make_pair(key, value));
	} else {
		it->second = value;
	} 
}

int rpc_receiver::get_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value)
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


















