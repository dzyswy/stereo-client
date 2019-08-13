#include "system_command.h"

#include "rpc/client.h"
#include "rpc/rpc_error.h"
#include "rpc/msgpack.hpp"





using namespace std;




int system_command::download_update_system(std::string host, std::string file, std::string user, std::string passwd, int force, int timeout)
{
	try {
		try {
			rpc::client client(ip_, port_);
			client.set_timeout(timeout);
			int result = client.call("set_download_update_system", host, file, user, passwd, force).as<int>();
			if (result < 0)
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
		std::cout << "Error: server is offline, or no support command, or error command format: " << "" << "download_update_system " << host << " " << file << " " << user << " " << passwd << " " << force << " \n";
		return -1;
	}
}







