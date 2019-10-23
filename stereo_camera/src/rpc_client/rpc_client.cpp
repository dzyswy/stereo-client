#include "rpc_client.h"
#include "rpc_receiver.h" 




using namespace std;


void rpc_client::set_connect(const char *ip, int port)
{
	ip_ = ip;
	port_ = port;
}
	
int rpc_client::set_value(const std::string para, int value, int timeout)
{
	stringstream os;
	os << "set" << "&";
	os << para << "&"; 
	os << std::to_string(value) << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request);
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	ret = atoi(result.c_str());
	if (ret < 0)
		return -1;
	
	return 0;
}
	
int rpc_client::set_value(const std::string para, float value, int timeout)
{
	stringstream os;
	os << "set" << "&";
	os << para << "&";
	os << std::to_string((double)value) << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request);
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	ret = atoi(result.c_str());
	if (ret < 0)
		return -1;
	
	return 0;
}
	
int rpc_client::set_value(const std::string para, std::string value, int timeout)
{
	stringstream os;
	os << "set" << "&";
	os << para << "&";
	os << value << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request);
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	ret = atoi(result.c_str());
	if (ret < 0)
		return -1;
	
	return 0;
}
	

int rpc_client::get_value(const std::string para, int &value, int timeout)
{
	stringstream os;
	os << "get" << "&";
	os << para << "&";
	os << "null" << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request);
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	value = atoi(result.c_str()); 
	
	return 0;
}
	
int rpc_client::get_value(const std::string para, float &value, int timeout)
{
	stringstream os;
	os << "get" << "&";
	os << para << "&";
	os << "null" << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request);
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	value = atof(result.c_str()); 
	
	return 0;
}
	
int rpc_client::get_value(const std::string para, std::string &value, int timeout)
{
	stringstream os;
	os << "get" << "&";
	os << para << "&";
	os << "null" << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request);
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	value = result; 
	
	return 0;
}

int rpc_client::get_value(const std::string para, std::string &value, std::string &result, int timeout)
{
	stringstream os;
	os << "get" << "&";
	os << para << "&";
	os << value << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request);
	
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	value = result; 
	
	return 0;
}
	
int rpc_client::do_action(const std::string para, int timeout)
{
	stringstream os;
	os << "set" << "&";
	os << para << "&";
	os << "null" << "&";
	
	string request = os.str();
	rpc_receiver client(ip_, port_, request);
	
	string result = "";
	int ret = client.get_result(result, timeout);
	if (ret < 0)
		return -1;
	
	return 0;
}











