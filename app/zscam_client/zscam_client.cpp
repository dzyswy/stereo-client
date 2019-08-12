#include <iostream>

#include "camera_command.h"
#include "system_command.h"
#include "discovery_receiver.h"

int main(int argc, char *argv[])
{
	camera_command("127.0.0.1", 8080);
	system_command("127.0.0.1", 8081);
	discovery_receiver("stereo_camera", 8082, 5000);
	return 0;
}








