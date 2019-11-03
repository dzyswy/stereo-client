#include "stream_client.h"
#include "stream_receiver.h"



using namespace std;



stream_client::stream_client(int debug)
{
	debug_ = debug;
	going = 0;
	reconnect_count_ = 0;
	image_.clear();
	headers_.clear();
	run_thread_ = NULL;
}


stream_client::~stream_client()
{
	disconnect_stream();
	image_.clear();
	headers_.clear();
}


int stream_client::connect_stream(const char *ip, int port, int index)
{ 
	int ret;
	std::unique_lock<std::mutex> lock(lock_);

	if (going)
		return -1;
	
	ret = check_ip(ip);
	if (ret < 0)
		return -1;
	
	ip_ = ip;
	port_ = port;
	index_ = index;
	 
	
	going = 1;
	reconnect_count_ = 0;
	
	
	run_thread_ = new std::thread([this] () {stream_process();});
	return 0;
}


int stream_client::disconnect_stream()
{
	std::unique_lock<std::mutex> lock(lock_);
	going = 0;

	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
		run_thread_ = NULL;
	}
	
	return 0;
}


int stream_client::query_frame(int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (cond_.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
 
	return 0;
}

void stream_client::get_image(std::vector<unsigned char> &image)
{
	std::unique_lock<std::mutex> lock(mux_);
	image = image_;
}

int stream_client::get_header(const std::string key, std::string &value)
{
	std::unique_lock<std::mutex> lock(mux_);
	typename std::map<std::string, std::string>::iterator it;
	it = headers_.find(key);
	if (it == headers_.end())
		return -1;
	value = it->second;
	return 0;
}

int stream_client::get_reconnect_count()
{
	std::unique_lock<std::mutex> lock(mux_);
	return reconnect_count_;
}

void stream_client::stream_process()
{
	int ret;
	cout << "create stream receiver thread\n";
	while(going)
	{
		stream_receiver *impl = new stream_receiver(ip_, port_, index_, debug_);
		impl->run();
		while(going)
		{
			if (debug_)
				cout << "stream_client::stream_process() going: " << going << endl;
			
			ret = impl->query_frame(5);
			if (ret < 0) {
				break;
			}

			{
				std::unique_lock<std::mutex> lock(mux_);		
				impl->get_image(image_);
				headers_ = impl->get_headers();
				cond_.notify_all();
			}
			 
		}	
		reconnect_count_++;
		delete impl; 
		

		
	}
	cout << "leave stream receiver thread\n";
	
}

int stream_client::check_ip(const char *value)
{
	int a = -1, b = -1, c = -1, d = -1;
	if ((sscanf(value, "%d.%d.%d.%d", &a, &b, &c, &d) == 4)
		&& ((a >= 0) && (a <= 255))
		&& ((b >= 0) && (b <= 255))
		&& ((c >= 0) && (c <= 255))
		&& ((d >= 0) && (d <= 255)))
	{
		return 0;
	}	
	cout << "ip: " << string(value) << ". format is wrong, please set again!\n";
	return -1;
}













