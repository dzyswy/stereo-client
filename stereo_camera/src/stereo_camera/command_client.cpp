#include "command_client.h"

#include "rpc/client.h"
#include "rpc/rpc_error.h"
#include "float_pair_vector.h"


using namespace std;

void command_client::set_connect(const char *ip, int port)
{
	ip_ = ip;
	port_ = port;
}


int command_client::set_value(const char *cmd, int value, int timeout)
{
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			return client.call(cmd, value).as<int>();
		} catch (rpc::rpc_error &e) {
			std::cout << std::endl << e.what() << std::endl;
			std::cout << "in function " << e.get_function_name() << ": ";

			using err_t = std::tuple<int, std::string>;
			auto err = e.get_error().as<err_t>();
			std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
			throw;
		}
	} catch (...) {
		std::string cmd_str = cmd;
		std::cout << "Error: server is offline, or no support command, or error command format: " << cmd_str << " " << value << " \n";
		return -1;
	}
}


int command_client::set_value(const char *cmd, float value, int timeout)
{
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			return client.call(cmd, value).as<int>();
		} catch (rpc::rpc_error &e) {
			std::cout << std::endl << e.what() << std::endl;
			std::cout << "in function " << e.get_function_name() << ": ";

			using err_t = std::tuple<int, std::string>;
			auto err = e.get_error().as<err_t>();
			std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
			throw;
		}
	} catch (...) {
		std::string cmd_str = cmd;
		std::cout << "Error: server is offline, or no support command, or error command format: " << cmd_str << " " << value << " \n";
		return -1;
	}
}

int command_client::set_value(const char *cmd, std::string value, int timeout)
{
	try {
		try {
			rpc::client client(ip_, port_);	
			client.set_timeout(timeout);
			return client.call(cmd, value).as<int>();
		} catch (rpc::rpc_error &e) {
			std::cout << std::endl << e.what() << std::endl;
			std::cout << "in function " << e.get_function_name() << ": ";

			using err_t = std::tuple<int, std::string>;
			auto err = e.get_error().as<err_t>();
			std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
			throw;
		}
	} catch (...) {
		std::string cmd_str = cmd;
		std::cout << "Error: server is offline, or no support command, or error command format: " << cmd_str << " " << value << " \n";
		return -1;
	}
}


int command_client::set_poly_mask(std::vector<std::pair<float, float> > &value, int timeout)
{
	struct float_pair_vector pointes;
	pointes.data = value;
	string svalue;
	int ret = pointes.to_string(svalue);
	if (ret < 0)
		return -1;
	return set_value("set_poly_mask", svalue, timeout);
}


int command_client::get_value(const char *cmd, int &value, int timeout)
{
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			value = client.call(cmd).as<int>();
			return 0;
		} catch (rpc::rpc_error &e) {
			std::cout << std::endl << e.what() << std::endl;
			std::cout << "in function " << e.get_function_name() << ": ";

			using err_t = std::tuple<int, std::string>;
			auto err = e.get_error().as<err_t>();
			std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
			throw;
		}
	} catch (...) {
		std::string cmd_str = cmd;
		std::cout << "Error: server is offline, or no support command, or error command format: " << cmd_str << " \n";
		return -1;
	}
}

int command_client::get_value(const char *cmd, float &value, int timeout)
{
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			value = client.call(cmd).as<float>();
			return 0;
		} catch (rpc::rpc_error &e) {
			std::cout << std::endl << e.what() << std::endl;
			std::cout << "in function " << e.get_function_name() << ": ";

			using err_t = std::tuple<int, std::string>;
			auto err = e.get_error().as<err_t>();
			std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
			throw;
		}
	} catch (...) {
		std::string cmd_str = cmd;
		std::cout << "Error: server is offline, or no support command, or error command format: " << cmd_str << " \n";
		return -1;
	}
}


int command_client::get_value(const char *cmd, std::string &value, int timeout)
{
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			value = client.call(cmd).as<std::string>();
			return 0;
		} catch (rpc::rpc_error &e) {
			std::cout << std::endl << e.what() << std::endl;
			std::cout << "in function " << e.get_function_name() << ": ";

			using err_t = std::tuple<int, std::string>;
			auto err = e.get_error().as<err_t>();
			std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
			throw;
		}
	} catch (...) {
		std::string cmd_str = cmd;
		std::cout << "Error: server is offline, or no support command, or error command format: " << cmd_str << " \n";
		return -1;
	}
}


int command_client::get_poly_mask(std::vector<std::pair<float, float> > &value, int timeout)
{
	int ret;
	string str; 
	ret = get_value("get_poly_mask", str, timeout);
	if (ret < 0)
		return -1;
	
	struct float_pair_vector pointes;
	ret = pointes.from_string(str);
	if (ret < 0)
		return -1;
	
	value = pointes.data;
	return 0;
}


int command_client::get_pixel_point(int x, int y, struct stereo_pixel_point &value, int timeout)
{
	string svalue;
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			svalue = client.call("get_pixel_point", x, y).as<std::string>();
		} catch (rpc::rpc_error &e) {
			std::cout << std::endl << e.what() << std::endl;
			std::cout << "in function " << e.get_function_name() << ": ";

			using err_t = std::tuple<int, std::string>;
			auto err = e.get_error().as<err_t>();
			std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
			throw;
		}
	} catch (...) {
		std::string cmd_str = "get_pixel_point";
		std::cout << "Error: server is offline, or no support command, or error command format: " << cmd_str << " \n";
		return -1;
	}
	
	struct stereo_pixel_point pixel_point;
	int ret = pixel_point.from_string(svalue);
	if (ret < 0)
		return -1;
	
	value = pixel_point;
	
	return 0;
}



























