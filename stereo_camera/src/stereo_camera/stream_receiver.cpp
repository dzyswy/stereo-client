#include "stream_receiver.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>
#include <asio.hpp>






#define BOUNDARYSTRING "--BOUNDARYSTRING\r\n"


using namespace std;
using namespace asio;
using namespace asio::ip;


class stream_receiver_impl
{
public:	
	stream_receiver_impl(std::string ip, int port, int index, int debug = 0);
	~stream_receiver_impl();
	
	int query_frame(int timeout = 5);
	void get_frame(std::vector<unsigned char> &image);
	void get_detect_boxes(std::vector<struct stereo_detect_box> &detect_boxes);
	void get_gyro_angle(struct stereo_gyro_angle &gyro_angle);
	
protected:
	void do_connect(ip::tcp::endpoint &ep);
	void do_write();
	void do_read();
	void do_boundary();
	void do_headers();
	void do_content();
	
protected:
	asio::io_context io_context_;
	tcp::socket socket_;
	asio::streambuf request_;
	asio::streambuf response_;
	
	int debug_;

	std::mutex mux_;
	std::condition_variable cond_;
	int frame_size_;
	vector<unsigned char> frame_buffer_;
	struct stereo_detect_boxes detect_boxes_;
	struct stereo_gyro_angle gyro_angle_;
	int frame_count_;
	int first_frame_;
	
	std::thread *run_thread_;	
	
};


stream_receiver_impl::stream_receiver_impl(std::string ip, int port, int index, int debug) : 
	socket_(io_context_), 
	frame_size_(0), 
	debug_(debug)
{
	frame_buffer_.clear();
	detect_boxes_.detect_boxes.clear();
	memset(&gyro_angle_, 0, sizeof(gyro_angle_));
	frame_count_ = 0;
	first_frame_ = 1;

	std::ostream request_stream(&request_);
	request_stream << "GET ";
	request_stream << "/" << index << " ";
    request_stream << "HTTP/1.1\r\n\r\n";
	
	
	ip::tcp::endpoint ep(address_v4::from_string(ip), port); 
	do_connect(ep);
	
	run_thread_ = new std::thread([this] () {io_context_.run();});
}

stream_receiver_impl::~stream_receiver_impl()
{
	socket_.close();
	io_context_.stop();
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
	}
}

int stream_receiver_impl::query_frame(int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (cond_.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
 
	return 0;
}


void stream_receiver_impl::get_frame(std::vector<unsigned char> &image)
{
	std::unique_lock<std::mutex> lock(mux_);
	image = frame_buffer_;
}

void stream_receiver_impl::get_detect_boxes(std::vector<struct stereo_detect_box> &detect_boxes)
{
	std::unique_lock<std::mutex> lock(mux_);
	detect_boxes = detect_boxes_.detect_boxes;
}

void stream_receiver_impl::get_gyro_angle(struct stereo_gyro_angle &gyro_angle)
{
	std::unique_lock<std::mutex> lock(mux_);
	gyro_angle = gyro_angle_;
}

void stream_receiver_impl::do_connect(ip::tcp::endpoint &ep)
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

void stream_receiver_impl::do_write()
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

void stream_receiver_impl::do_read()
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
				
				first_frame_ = 1;				
				do_boundary();
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}		
		});
}

void stream_receiver_impl::do_boundary()
{
	asio::async_read_until(socket_, response_, "--BOUNDARYSTRING\r\n",
		[this] (asio::error_code ec, std::size_t n) 
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
				
				if (first_frame_) {
					first_frame_ = 0;
				} else {
					std::unique_lock<std::mutex> lock(mux_);
					cond_.notify_all(); 
				}	
				
				if ((frame_size_ > 1024) && (response_.size() > (frame_size_ * 2)))
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

void stream_receiver_impl::do_headers()
{
	
	
	asio::async_read_until(socket_, response_, "\r\n\r\n",
		[this] (asio::error_code ec, std::size_t n) 
		{
			int ret;
			string frame_size_s;
			string frame_count_s;
			string detect_boxes_s;
			string gyro_angle_s;
			
			frame_size_s.clear();
			frame_count_s.clear();
			detect_boxes_s.clear();
			gyro_angle_s.clear();
			
			if (!ec)
			{
				if (debug_) {
					cout << "do_headers new read:" << n << ", total: " << response_.size() << endl;
				}
				 
				std::istream rs(&response_);
				std::string header;
				while (std::getline(rs, header))
				{
				//	cout << header;
					int len;
					if (header.substr(0, 15) == "Content-Length:") 
					{
						frame_size_s = header.substr(16, header.length() - 17);
						if (debug_) {
							cout << "get Content-Length: " << frame_size_s << endl;
						} 
					}
					
					len = strlen("Content-frame-count:");
					if (header.substr(0, len) == "Content-frame-count:") 
					{
						frame_count_s = header.substr(len + 1, header.length() - (len + 2));
						if (debug_) {
							cout << "get Content-frame-count: " << frame_count_s << endl;
						} 	
					}
					
					len = strlen("Content-detect-boxes:");
					if (header.substr(0, len) == "Content-detect-boxes:") 
					{
						detect_boxes_s = header.substr(len + 1, header.length() - (len + 2));
						if (debug_) {
							cout << "get Content-detect-boxes: " << detect_boxes_s << endl;
						} 	
					}
					
					len = strlen("Content-gyro-angle:");
					if (header.substr(0, len) == "Content-gyro-angle:") {
						gyro_angle_s = header.substr(len + 1, header.length() - (len + 2));
						if (debug_) {
							cout << "get Content-gyro-angle: " << gyro_angle_s << endl;
						} 	
					}
					if (header == "\r")
						break;
				}	
				
				{
					std::unique_lock<std::mutex> lock(mux_);
					
					if (frame_size_s.empty()) {
						frame_size_ = 0;
					} else {
						frame_size_ =  atoi(frame_size_s.c_str());
					}
					
					if (frame_count_s.empty()) {
						frame_count_ = 0;
					} else {
						frame_count_ = atoi(frame_count_s.c_str()); 
					}
					
					if (detect_boxes_s.empty()) {
						detect_boxes_.detect_boxes.clear();
					} else {
						struct stereo_detect_boxes detect_boxes;
						ret = detect_boxes.from_string(detect_boxes_s);
						if (ret < 0) {
							detect_boxes_.detect_boxes.clear();
						} else {
							detect_boxes_ = detect_boxes;
						} 
					}
					
					if (gyro_angle_s.empty()) {
						memset(&gyro_angle_, 0, sizeof(gyro_angle_));
					} else {
						struct stereo_gyro_angle gyro_angle;
						ret = gyro_angle.from_string(gyro_angle_s);
						if (ret < 0) {
							memset(&gyro_angle_, 0, sizeof(gyro_angle_));
						} else {
							gyro_angle_ = gyro_angle;
						}
					}  
				}
				
				if (frame_size_)
					do_content();
				else
					do_boundary();
			}
			else
			{
				 std::cout << "Error: " << ec.message() << "\n";
			}		
		});
}

void stream_receiver_impl::do_content()
{
	asio::async_read(socket_, response_, asio::transfer_at_least(frame_size_),
		[this] (asio::error_code ec, std::size_t n) 
		{
			if (!ec)
			{
				if (debug_) {
					cout << "do_content new read:" << n << ", total: " << response_.size() << ", frame_size: " << frame_size_ << endl;
				}
				
				if (response_.size() >= frame_size_)
				{
					std::unique_lock<std::mutex> lock(mux_);
					frame_buffer_.resize(frame_size_);
					response_.sgetn((char *)&frame_buffer_[0], frame_size_);
				}	
				do_boundary();
			}	
		});
}







stream_receiver::stream_receiver(int debug)
{
	debug_ = debug;
	going = 0;
	reconnect_count_ = 0;
	frame_buffer_.clear();
	detect_boxes_.detect_boxes.clear();
	memset(&gyro_angle_, 0, sizeof(gyro_angle_));
	run_thread_ = NULL;
}


stream_receiver::~stream_receiver()
{
	disconnect_stream();
}


int stream_receiver::connect_stream(const char *ip, int port, int index)
{ 
	{
		std::unique_lock<std::mutex> lock(mux_);
	
		if (going)
			return -1;
		
		ip_ = ip;
		port_ = port;
		index_ = index;
		
		going = 1;
		reconnect_count_ = 0;
	}
	
	run_thread_ = new std::thread([this] () {stream_process();});
	return 0;
}


int stream_receiver::disconnect_stream()
{
	{
		std::unique_lock<std::mutex> lock(mux_);
		going = 0;
	}
	
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
		run_thread_ = NULL;
	}
	
	return 0;
}


int stream_receiver::query_frame(int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (cond_.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
 
	return 0;
}

void stream_receiver::get_frame(std::vector<unsigned char> &image)
{
	std::unique_lock<std::mutex> lock(mux_);
	image = frame_buffer_;
}

void stream_receiver::get_detect_boxes(std::vector<struct stereo_detect_box> &detect_boxes)
{
	std::unique_lock<std::mutex> lock(mux_);
	detect_boxes = detect_boxes_.detect_boxes;
}

void stream_receiver::get_gyro_angle(struct stereo_gyro_angle &gyro_angle)
{
	std::unique_lock<std::mutex> lock(mux_);
	gyro_angle = gyro_angle_;
}

int stream_receiver::get_reconnect_count()
{
	std::unique_lock<std::mutex> lock(mux_);
	return reconnect_count_;
}

void stream_receiver::stream_process()
{
	int ret;
	cout << "create stream receiver thread\n";
	while(1)
	{
		stream_receiver_impl *impl = new stream_receiver_impl(ip_, port_, index_, debug_);
		
		while(1)
		{
			ret = impl->query_frame(5);
			if (ret < 0) {
				break;
			}

			{
				std::unique_lock<std::mutex> lock(mux_);		
				impl->get_frame(frame_buffer_);
				impl->get_detect_boxes(detect_boxes_.detect_boxes);
				impl->get_gyro_angle(gyro_angle_);
				cond_.notify_all();
				if (!going)
					break;	
			}
		
		}	
		reconnect_count_++;
		delete impl;
		
		{
			std::unique_lock<std::mutex> lock(mux_);
			if (!going)
				break;
		}
		
	}
	cout << "leave stream receiver thread\n";
	
}




































