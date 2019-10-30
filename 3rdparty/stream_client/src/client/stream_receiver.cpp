#include "stream_receiver.h"
#include <string>
using namespace std;

stream_receiver::stream_receiver(std::string ip, int port, int index, int debug) : 
	socket_(io_context_),  
	debug_(debug)
{
	image_.clear();
	headers_.clear();

	std::ostream request_stream(&request_);
	request_stream << "GET ";
	request_stream << "/" << index << " ";
    request_stream << "HTTP/1.1\r\n\r\n";
	
	
	ip::tcp::endpoint ep(address_v4::from_string(ip), port); 
	do_connect(ep);
	
	run_thread_ = new std::thread([this] () {io_context_.run();});
}

stream_receiver::~stream_receiver()
{
	socket_.close();
	io_context_.stop();
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
	}
	
	image_.clear();
	headers_.clear();
}

int stream_receiver::query_frame(int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (cond_.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
 
	return 0;
}


void stream_receiver::get_image(std::vector<unsigned char> &image)
{
	std::unique_lock<std::mutex> lock(mux_);
	image = image_;
}

int stream_receiver::get_header(const std::string key, std::string &value)
{
	std::unique_lock<std::mutex> lock(mux_);
	typename std::map<std::string, std::string>::iterator it;
	it = headers_.find(key);
	if (it == headers_.end())
		return -1;
	value = it->second;
	return 0;
}

std::map<std::string, std::string> stream_receiver::get_headers()
{
	std::unique_lock<std::mutex> lock(mux_);
	return headers_;
}


void stream_receiver::do_connect(ip::tcp::endpoint &ep)
{
	socket_.async_connect(ep, 
		[this] (asio::error_code ec) 
		{
			if (!ec) 
			{
				do_write();
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}	
			
		});
}

void stream_receiver::do_write()
{
	asio::async_write(socket_, request_,
		[this] (asio::error_code ec, std::size_t length) 
		{
			if (!ec)
			{
				do_read();
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}			
		});
}

void stream_receiver::do_read()
{
	asio::async_read_until(socket_, response_, "\r\n\r\n",
		[this] (asio::error_code ec, std::size_t n) 
		{
			if (!ec)
			{
				std::istream rs(&response_);
				std::string header;
				while(std::getline(rs, header))
				{
				//	cout << header;
					if (header == "\r")
						break;
				}					
				do_boundary(0);
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}		
		});
}

void stream_receiver::do_boundary(int flag, int frame_size)
{
	asio::async_read_until(socket_, response_, "--BOUNDARYSTRING\r\n",
		[this, flag, frame_size] (asio::error_code ec, std::size_t n) 
		{
			if (!ec)
			{
				if (debug_) {
					cout << "do_boundary new read:" << n << ", total: " << response_.size() << endl;
				}
					 
				std::istream rs(&response_);
				std::string header;
				while(std::getline(rs, header))
				{
				//	cout << header;
					if (header == "--BOUNDARYSTRING\r")
						break;
				}	
				
				if (flag) 
				{ 
					std::unique_lock<std::mutex> lock(mux_);
					cond_.notify_all(); 
				}	
				
				if ((frame_size > 1024) && (response_.size() > (frame_size * 2)))
				{
					response_.consume(response_.size());
				}	
				do_headers();
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}	
		});
}

void stream_receiver::do_headers()
{ 
	asio::async_read_until(socket_, response_, "\r\n\r\n",
		[this] (asio::error_code ec, std::size_t n) 
		{
			int ret;
			
			if (!ec)
			{
				if (debug_) {
					cout << "do_headers new read:" << n << ", total: " << response_.size() << endl;
				}
				 
				std::istream rs(&response_);
				std::string header;
				std::map<std::string, std::string> headers;
				headers.clear();
				while (std::getline(rs, header))
				{
				//	cout << header;
					if (header == "\r")
						break;
					
					istringstream iss(header);
					std::string key;
					std::string value;
					
					string temp = "";
					std::getline(iss, temp, ':');
					if (debug_)
					{
						cout << "header: " << header << endl;
						cout << "temp: " << temp << endl;
					}
					if (temp.size() == 0)
						continue;
					key = temp;
					
					if ((header.size() - temp.size()) < 2)
						continue;
					value = header.substr(temp.size() + 2, header.size() - temp.size() - 2);
					
					headers.insert(make_pair(key, value));
					
					if (debug_)
					{ 
						cout << "key=" << key << ", value=" << value << endl;
					}	
				}	
				
				{
					std::unique_lock<std::mutex> lock(mux_);
					headers_ = headers; 
				}
				
				int frame_size = 0;
				std::string frame_size_s = "";
				ret = get_header("Content-Length", frame_size_s);
				if (ret == 0)
				{
					frame_size = atoi(frame_size_s.c_str());
				}	
				
				if (frame_size)
					do_content(frame_size);
				else
					do_boundary(1);
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}		
		});
}

void stream_receiver::do_content(int frame_size)
{
	asio::async_read(socket_, response_, asio::transfer_at_least(frame_size),
		[this, frame_size] (asio::error_code ec, std::size_t n) 
		{
			if (!ec)
			{
				if (debug_) {
					cout << "do_content new read:" << n << ", total: " << response_.size() << ", frame_size: " << frame_size << endl;
				}
				
				if (response_.size() >= frame_size)
				{
					std::unique_lock<std::mutex> lock(mux_);
					image_.resize(frame_size);
					response_.sgetn((char *)&image_[0], frame_size);
				}	
				do_boundary(1, frame_size);
			}	
		});
}











































