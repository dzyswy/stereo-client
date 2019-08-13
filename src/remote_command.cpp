#include "remote_command.h"

#include "rpc/client.h"
#include "rpc/rpc_error.h"


remote_command::set_connect(const char *ip, int port)
{
	ip_ = ip;
	port_ = port;
}


int remote_command::set_value(const char *cmd, int value, int timeout)
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


int remote_command::set_value(const char *cmd, float value, int timeout)
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

int remote_command::set_value(const char *cmd, std::string value, int timeout)
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



int remote_command::get_value(const char *cmd, int &value, int timeout)
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

int remote_command::get_value(const char *cmd, float &value, int timeout)
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


int remote_command::get_value(const char *cmd, std::string &value, int timeout)
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

































