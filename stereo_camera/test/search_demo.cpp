#include "search_camera.h"
#include <iostream>       
#include <thread>         
#include <chrono>        
#include <errno.h>
#include <signal.h>


using namespace std;


int going = 1;

void signal_handler(int sig)
{
	going = 0;
}


int main(int argc, char *argv[])
{ 

	if(signal(SIGINT, signal_handler) == SIG_ERR) {
        return -1;
    }
	
	int debug = 0;
	if (argc == 2)
		debug = atoi(argv[1]);
	
	search_camera search("zynq_stereo_camera", 45789, 3, debug);
	
	while(going)
	{
		if (!debug)
		{
			system("clear");
		}	
		
		std::vector<struct zscam_node> device_nodes;
		device_nodes.clear();
		search.get_device_nodes(device_nodes);
		printf("%15s %15s %15s %15s %15s\n", "ip", "board", "company", "version", "number");
		for (int i = 0; i < device_nodes.size(); i++)
		{
			struct zscam_node &node = device_nodes[i];
			std::string ip = (node.ip.size() > 0) ? node.ip : string("NULL");//192.168.3.1
			std::string board = (node.board.size() > 0) ? node.board : string("NULL");
			std::string company = (node.company.size() > 0) ? node.company : string("NULL");
			std::string version = (node.version.size() > 0) ? node.version : string("NULL");
			std::string serial_number = (node.serial_number.size() > 0) ? node.serial_number : string("NULL");
			
			printf("%15s %15s %15s %15s %15s\n", ip.c_str(), board.c_str(), company.c_str(), version.c_str(), serial_number.c_str());
			
		}
		fflush(stdout);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}	
	return 0;
}


















