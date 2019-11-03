#include "rpc_client.h"
#include "rpc_receiver.h" 




using namespace std;

rpc_client::rpc_client(int debug)
{
	debug_ = debug;
	ip_ = "127.0.0.1";
}


int rpc_client::set_connect(const char *ip, int port)
{
	int ret;
	ret = check_ip(ip);
	if (ret < 0)
		return -1;
	ip_ = ip;
	port_ = port;
	return 0;
}
	
int rpc_client::set_value(const std::string key, int value, int timeout)
{ 
	return set_value(key, to_string(value), timeout);
}
	
int rpc_client::set_value(const std::string key, float value, int timeout)
{
	return set_value(key, to_string(value), timeout);
}
	
int rpc_client::set_value(const std::string key, std::string value, int timeout)
{
	stringstream os;
	os << "set" << "&";
	os << key << "&";
	os << value << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request, debug_);
	client.run();
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	ret = atoi(result.c_str());
	if (ret < 0)
		return -1;
	
	return 0;
}
	

int rpc_client::get_value(const std::string key, int &value, int timeout)
{
	std::string result = "";
	int ret = get_value(key, result, timeout);
	if (ret < 0)
		return -1;

	value = atoi(result.c_str()); 
	
	return 0;
}
	
int rpc_client::get_value(const std::string key, float &value, int timeout)
{
	std::string result = "";
	int ret = get_value(key, result, timeout);
	if (ret < 0)
		return -1;
	
	value = atof(result.c_str()); 
	
	return 0;
}
	
int rpc_client::get_value(const std::string key, std::string &value, int timeout)
{
	return get_value(key, "null", value, timeout);
}

int rpc_client::get_value(const std::string key, const std::string &para, std::string &value, int timeout)
{
	stringstream os;
	os << "get" << "&";
	os << key << "&";
	os << para << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request, debug_);
	client.run();
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	value = result; 
	
	return 0;
}
	
int rpc_client::do_action(const std::string key, int timeout)
{
	stringstream os;
	os << "get" << "&";
	os << key << "&";
	os << "null" << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request, debug_);
	client.run();
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	return 0;
}




int rpc_client::check_ip(const char *value)
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






