#include "stereo_camera.h"



using namespace std;


int main(int argc, char *argv[])
{
	
	int ret;
	if (argc != 5)
	{
		printf("usage: %s ip set|get|do key para\n", argv[0]);
		printf("usage: %s 192.168.3.22 get version null\n", argv[0]);
		return -1;
	}

	string ip = argv[1];
	string action_s = argv[2];
	string key = argv[3];
	string para = argv[4];
	
	int action = 0;
	if (strcmp(action_s.c_str(), "set") == 0)
		action = 0;
	else if (strcmp(action_s.c_str(), "get") == 0)
		action = 1; 
	else if (strcmp(action_s.c_str(), "do") == 0)
		action = 2;
	else {
		printf("only support: set get do\n");
		return -1;
	}
	stereo_camera cam(0);
	cam.open_device(ip.c_str(), 7070);
	
	switch(action)
	{
		case 0:
		{
			ret = cam.set_value(key.c_str(), para, 5);
			if (ret < 0) {
				printf("set error!\n");
				return -1;
			}
		}break;
		
		case 1:
		{
			string result = "";
			ret = cam.get_value(key.c_str(), para, result, 5);
			if (ret < 0) {
				printf("get error!\n");
				return -1;
			}
			cout << result << endl;
		}break;
		
		case 2:
		{
			ret = cam.do_action(key.c_str(), 5);
			if (ret < 0) {
				printf("set error!\n");
				return -1;
			}
			printf("ok\n");
		}break;
		default:
			break;
	}
	

	return 0;
}
















