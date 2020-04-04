#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>
#include <asio.hpp>
#include <asio/steady_timer.hpp>



using namespace std;
using namespace asio;


static const size_t BUF_SIZE = 1024;

int udpsend(const char* ip, int port, const char* value)
{
	string ips = ip;
	
	io_service ios;
	ip::udp::socket sock(ios);
	ip::udp::endpoint end_point(ip::address::from_string(ips), port);
	sock.open(end_point.protocol());
	string str = value;
	
	try {
		sock.send_to(buffer(str.c_str(), str.size()), end_point);
	}
	catch (...) {
		printf("udpsend error!\n");
		return -1;
	}

	return 0;
}