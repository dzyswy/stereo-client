#include "rpc_client.h"
#include "rpc_receiver.h" 




using namespace std;

rpc_client::rpc_client(int debug)
{
	debug_ = debug;
}


void rpc_client::set_connect(const char *ip, int port)
{
	ip_ = ip;
	port_ = port;
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
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	return 0;
}











