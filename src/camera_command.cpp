#include "camera_command.h"
#include "rpc/client.h"
#include "rpc/rpc_error.h"
#include "rpc/msgpack.hpp"

using namespace std;

struct point_space_status_rpclib {
	int x;
	int y;
	int d;
	int xcm;
	int ycm;
	int zcm;
	int xtcm;
	int ytcm;
	int ztcm;
	double xa;
	double ya;
	double r;
	MSGPACK_DEFINE_ARRAY(x, y, d, xcm, ycm, zcm, xtcm, ytcm, ztcm, xa, ya, r);
};



camera_command::camera_command(const char *ip, int port) : 
	remote_command(ip, port)
{
	
}	

int camera_command::set_poly_mask(vector<pair<int, int> > &value, int timeout)
{

	try {
		try {
			stringstream os;
			for (int i = 0; i < value.size(); i++)
			{
				os << value[i].first << " " << value[i].second << " ";
			}	 
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			int ret = client.call("set_poly_mask_points", os.str()).as<int>();
			if (ret < 0)
				return -1;
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
		std::cout << "Error: server is offline, or no support command, or error command format: " << "" << "set_poly_mask " << " \n";
		return -1;
	}
}


int camera_command::get_disparity(int x, int y, int &d, int timeout)
{
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			d = client.call("get_disparity", x, y).as<int>();
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
		std::cout << "Error: server is offline, or no support command, or error command format: " << "" << "get_disparity " << x << " " << y << " \n";
		return -1;
	}
}

int camera_command::get_point_space_status(int x, int y, struct point_space_status &status, int timeout)
{
	struct point_space_status_rpclib result;
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			result = client.call("get_point_space_status", x, y).as<struct point_space_status_rpclib>();
			status.x = result.x;
			status.y = result.y;
			status.d = result.d;
			status.xcm = result.xcm;
			status.ycm = result.ycm;
			status.zcm = result.zcm;
			status.xtcm = result.xtcm;
			status.ytcm = result.ytcm;
			status.ztcm = result.ztcm;
			status.xa = result.xa;
			status.ya = result.ya;
			status.r = result.r;
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
		std::cout << "Error: server is offline, or no support command, or error command format: " << "" << "get_point_space_status " << x << " " << y << " \n";
		return -1;
	}
}
/*
int camera_command::get_poly_mask(vector<pair<int, int> > &value)
{
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			string str = client.call("get_poly_mask_points").as<string>();
			vector<string> list_str;
			boost::split(list_str, str, boost::is_any_of(" "));
			int num = list_str.size() / 2;
			if ((num < 4) || (num > 50))
				return -1;
			
			value.clear();
			for (int i = 0; i < num; i++)
			{
				int x = boost::lexical_cast<int>(list_str[2 * i]);
				int y = boost::lexical_cast<int>(list_str[2 * i + 1]);
				printf("%d: %d %d \n", i, x, y);
				pair<int, int> point;
				point.first = x;
				point.second = y;
				value.push_back(point);
			}	
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
		std::cout << "Error: server is offline, or no support command, or error command format: " << "" << "get_poly_mask "  << " \n";
		return -1;
	}
}

*/













