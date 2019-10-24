#include "rpc_receiver.h"


using namespace std;





rpc_receiver::rpc_receiver(std::string ip, int port, std::string request)
	: socket_(io_context_)
{
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
			if (!ec)
			{
				std::istream response_stream(&response_);
				std::string header;
				while (std::getline(response_stream, header) && header != "\r")
				{
					int len;
					len = strlen("ret:");
					if (header.substr(0, len) == "ret:") 
					{
						string ret_s = header.substr(len + 1, header.length() - (len + 2));
						ret_ = atoi(ret_s.c_str());
					}
					
					len = strlen("result:");
					if (header.substr(0, len) == "result:") 
					{
						std::unique_lock<std::mutex> lock(mux_);
						result_ = header.substr(len + 1, header.length() - (len + 2));
						cond_.notify_all();
					}
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























