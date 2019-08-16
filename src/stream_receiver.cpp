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

#include "json_detect_boxes.h"
#include "json_gyro_angle.h"




#define BOUNDARYSTRING "--BOUNDARYSTRING\r\n"


using namespace std;
using namespace asio;
using namespace asio::ip;


class stream_receiver_impl : public std::enable_shared_from_this<stream_receiver_impl>
{
public:	
	stream_receiver_impl(const stream_receiver_impl&) = delete;
	stream_receiver_impl& operator=(const stream_receiver_impl&) = delete;

	explicit stream_receiver_impl(std::string ip, int port, int stream_id);
	~stream_receiver_impl();
	
	int query_frame(int timeout = 5);
	void get_frame(std::vector<unsigned char> &image);
	void get_detect_boxes(vector<struct stereo_detect_box> &detect_boxes);
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
	

	std::mutex mux_;
	std::condition_variable cond_;
	int frame_size_;
	vector<unsigned char> frame_buffer_;
	vector<struct stereo_detect_box> detect_boxes_;
	struct stereo_gyro_angle gyro_angle_;
	
	std::thread *run_thread_;	
	
};


stream_receiver_impl::stream_receiver_impl(std::string ip, int port, int stream_id) : 
	socket_(io_context_), 
	frame_size_(0)
{

	std::ostream request_stream(&request_);
	request_stream << "GET ";
	request_stream << "/" << stream_id << " ";
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

void stream_receiver_impl::get_detect_boxes(vector<struct stereo_detect_box> &detect_boxes)
{
	std::unique_lock<std::mutex> lock(mux_);
	detect_boxes = detect_boxes_;
}

void stream_receiver_impl::get_gyro_angle(struct stereo_gyro_angle &gyro_angle)
{
	std::unique_lock<std::mutex> lock(mux_);
	gyro_angle = gyro_angle_;
}

void stream_receiver_impl::do_connect(ip::tcp::endpoint &ep)
{
	auto self(shared_from_this());
	socket_.async_connect(ep, 
		[this, self] (std::error_code ec) 
		{
			if (!ec) 
			{
				cout << "do_connect\n";
			//	do_write();
			}
			else
			{
				 printf( "do_connect %d.\n", ec);
			}	
			
		});
}

void stream_receiver_impl::do_write()
{
	auto self(shared_from_this());
	asio::async_write(socket_, request_,
		[this, self] (std::error_code ec, std::size_t length) 
		{
			if (!ec)
			{
				cout << "do_write\n";
				do_read();
			}	
		});
}

void stream_receiver_impl::do_read()
{
	auto self(shared_from_this());
	asio::async_read_until(socket_, response_, "\r\n\r\n",
		[this, self] (std::error_code ec, std::size_t n) 
		{
			if (!ec)
			{
				std::istream rs(&response_);
				std::string header;
				while (std::getline(rs, header) && header != "\r");
				cout << "do_read\n";
				do_boundary();
			}	
		});
}

void stream_receiver_impl::do_boundary()
{
	auto self(shared_from_this());
	asio::async_read_until(socket_, response_, "--BOUNDARYSTRING\r\n",
		[this, self] (std::error_code ec, std::size_t n) 
		{
			if (!ec)
			{
				std::istream rs(&response_);
				std::string header;
				while (std::getline(rs, header) && header != "--BOUNDARYSTRING\r");
				cout << "do_boundary\n";
				do_headers();
			}	
		});
}

void stream_receiver_impl::do_headers()
{
	
	auto self(shared_from_this());
	asio::async_read_until(socket_, response_, "\r\n\r\n",
		[this, self] (std::error_code ec, std::size_t n) 
		{
			int ret;
			if (!ec)
			{
				std::istream rs(&response_);
				std::string header;
				while (std::getline(rs, header) && header != "\r")
				{
					int len;
					if (header.substr(0, 15) == "Content-Length:") 
					{
						string frame_size_s = header.substr(16, header.length() - 17);
						int frame_size = atoi(frame_size_s.c_str());
						{
							std::unique_lock<std::mutex> lock(mux_);
							frame_size_ = (frame_size <= 0) ? 1:frame_size;
						}
					}
					/* 
					len = strlen("Content-detect-boxes:");
					if (header.substr(0, len) == "Content-detect-boxes:") 
					{
						string detect_boxes_s = header.substr(len + 1, header.length() - (len + 2));
						vector<struct stereo_detect_box> detect_boxes;
						json_detect_boxes detect_boxes_j(detect_boxes);
						ret = detect_boxes_j.from_string(detect_boxes_s);
						if (ret == 0)
						{
							std::unique_lock<std::mutex> lock(mux_);
							detect_boxes_ = detect_boxes;
						}	
					}
					
					len = strlen("Content-gyro-status:");
					if (header.substr(0, len) == "Content-gyro-status:") {
						string gyro_angle_s = header.substr(len + 1, header.length() - (len + 2));
						struct stereo_gyro_angle gyro_angle;
						json_gyro_angle gyro_angle_j(gyro_angle);
						ret = gyro_angle_j.from_string(gyro_angle_s);
						if (ret == 0)
						{
							std::unique_lock<std::mutex> lock(mux_);
							gyro_angle_ = gyro_angle;
						}	
					}*/
				}	
				cout << "do_headers\n";
				do_content();
			}	
		});
}

void stream_receiver_impl::do_content()
{
	auto self(shared_from_this());
	asio::async_read(socket_, response_, asio::transfer_at_least(frame_size_ + 2),
		[this, self] (std::error_code ec, std::size_t n) 
		{
			if (!ec)
			{
				if (n >= (frame_size_ + 2))
				{
					std::unique_lock<std::mutex> lock(mux_);
					frame_buffer_.resize(frame_size_);
					response_.sgetn((char *)&frame_buffer_[0], frame_size_);
					char c = response_.sgetc();
					if (c != '\r')
					{
						frame_buffer_.resize(0);
					}	
					cond_.notify_all();
				}	
				cout << "do_content\n";
				do_boundary();
			}	
		});
}







stream_receiver::stream_receiver()
{
	going = 0;
	reconnect_count_ = 0;
}


stream_receiver::~stream_receiver()
{
	disconnect_stream();
}


int stream_receiver::connect_stream(const char *ip, int port, int stream_id)
{
	std::unique_lock<std::mutex> lock(mux_);
	
	if (going)
		return -1;
	
	ip_ = ip;
	port_ = port;
	stream_id_ = stream_id;
	
	going = 1;
	run_thread_ = new std::thread([this] () {stream_process();});
}


int stream_receiver::disconnect_stream()
{
	std::unique_lock<std::mutex> lock(mux_);
	
	going = 0;
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

void stream_receiver::get_detect_boxes(vector<struct stereo_detect_box> &detect_boxes)
{
	std::unique_lock<std::mutex> lock(mux_);
	detect_boxes = detect_boxes_;
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
	while(1)
	{
		{
			std::unique_lock<std::mutex> lock(mux_);
			if (!going)
				break;
		}
		
		stream_receiver_impl stream(ip_, port_, stream_id_);
		while(1)
		{/*
			ret = stream.query_frame(5);
			if (ret < 0)
			{
				reconnect_count_++;
				break;
			}

			{
				std::unique_lock<std::mutex> lock(mux_);		
				stream.get_frame(frame_buffer_);
				stream.get_detect_boxes(detect_boxes_);
				stream.get_gyro_angle(gyro_angle_);
				cond_.notify_all();	
			}*/
			std::this_thread::sleep_for(std::chrono::seconds(10));
		
		}	
		
	}
	
}




































