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
#include <sys/time.h>
#include <asio.hpp>


#define BOUNDARYSTRING "--BOUNDARYSTRING\r\n"


using namespace std;
using namespace asio;
using namespace asio::ip;


class stream_receiver_impl
{
public:	
	stream_receiver_impl(std::string ip, int port, int stream_number);
	~stream_receiver_impl();
	int query_frame(std::vector<unsigned char> &image, int timeout = 5);
	
	
protected:
	void do_connect(ip::tcp::endpoint &ep);
	void do_write_stream_request();
	void do_read_stream_response();
	void do_read_boundary();
	
protected:
	asio::io_context io_context_;
	tcp::socket socket_;
	asio::streambuf request_;
	asio::streambuf response_;
	
	int sof_;

	std::mutex mux_;
	std::condition_variable cond_;
	vector<unsigned char> frame_buffer_;
	
	std::thread *run_thread_;	
	
};


stream_receiver_impl::stream_receiver_impl(std::string ip, int port, int stream_number) : 
	socket_(io_context_), 
	sof_(0)
{
	cout << "stream_receiver: " << ip << "@" << port << endl;
	
	std::ostream request_stream(&request_);
	request_stream << "GET ";
	request_stream << "/stream_" << stream_number << " ";
    request_stream << "HTTP/1.1\r\n\r\n";
	
	
	ip::tcp::endpoint ep(address_v4::from_string(ip), port); 
	do_connect(ep);
	
	run_thread_ = new std::thread([this] () {io_context_.run();});
}

stream_receiver_impl::~stream_receiver_impl()
{
	io_context_.stop();
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
	}
}

int stream_receiver_impl::query_frame(std::vector<unsigned char> &image, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	
	if (cond_.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
	
	image = frame_buffer_;
	return 0;
}


void stream_receiver_impl::do_connect(ip::tcp::endpoint &ep)
{
	socket_.async_connect(ep, 
		[this] (std::error_code ec) 
		{
			if (!ec) 
			{
				do_write_stream_request();
			}	
		});
}

void stream_receiver_impl::do_write_stream_request()
{
	asio::async_write(socket_, request_,
			[this] (std::error_code ec, std::size_t length) 
			{
				if (!ec)
				{
					do_read_stream_response();
				}	
			});
}

void stream_receiver_impl::do_read_stream_response()
{
	asio::async_read_until(socket_, response_, BOUNDARYSTRING,
			[this] (std::error_code ec, std::size_t n) 
			{
				if (!ec)
				{
					std::istream rs(&response_);
					std::string header;
					while (std::getline(rs, header) && header != "--BOUNDARYSTRING\r");
			 
					sof_ = 0;
					do_read_boundary();
				}	
			});
}


void stream_receiver_impl::do_read_boundary()
{
	asio::async_read_until(socket_, response_, BOUNDARYSTRING,
			[this] (std::error_code ec, std::size_t n)
			{
				if (!ec)
				{
					std::istream rs(&response_);
					std::string header;
					int frame_size = 0;
					int slen = 0;
			 
					while (std::getline(rs, header))
					{
						if (header.substr(0, 15) == "Content-Length:") {
							string str_size = header.substr(16, header.length() - 17);
							frame_size = atoi(str_size.c_str());
							sof_ = 1;
						//	cout << "frame_size: " << frame_size << endl;
						}
						
						/*
						slen = strlen("Content-detect-boxes:");
						if (header.substr(0, slen) == "Content-detect-boxes:") {
							string detect_boxes_str = header.substr(slen + 1, header.length() - (slen + 2));
						//	cout << "detect_boxes_str: " << detect_boxes_str << endl;
							detect_boxes_.clear();
							ret = deserialize_detect_boxes(detect_boxes_str, detect_boxes_);
							if (ret < 0) {
								cout << "fail to deserialize detect boxes\n";
								detect_boxes_.clear();
							}
						}
						
						slen = strlen("Content-gyro-status:");
						if (header.substr(0, slen) == "Content-gyro-status:") {
							string gyro_angle_str = header.substr(slen + 1, header.length() - (slen + 2));
						//	cout << "gyro_angle_str: " << gyro_angle_str << endl;
							ret = deserialize_gyro_status(gyro_angle_str, gyro_angle_);
							if (ret < 0) {
								cout << "fail to deserialize_gyro_status\n";
							}
						}
						*/
						if (header == "\r") {
							break;
						}
							
					}	
					
					if (sof_ && (response_.size() > frame_size))
					{
						{
							std::unique_lock<std::mutex> lock(mux_);
							frame_buffer_.resize(frame_size);
							response_.sgetn((char *)&frame_buffer_[0], frame_size);
							sof_ = 0;	
						}
						cond_.notify_all();
					}	
					
					
					while (std::getline(rs, header))
					{
						if (header == "--BOUNDARYSTRING\r")
							break;
					}	
					
					do_read_boundary();
				}	
			});
}




stream_receiver::stream_receiver(std::string ip, int port, int stream_number)
{
	impl_ = new stream_receiver_impl(ip, port, stream_number);
}

stream_receiver::~stream_receiver()
{
	delete impl_;
}

int stream_receiver::query_frame(std::vector<unsigned char> &image, int timeout)
{
	return impl_->query_frame(image, timeout);
}









