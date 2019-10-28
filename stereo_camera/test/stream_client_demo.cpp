#include "stereo_camera.h"
#include "media_record.h"
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
	int ret;
	if ((argc != 3) && (argc != 4)) 
	{
		printf("usage: %s ip channel\n", argv[0]);
		printf("usage: %s ip channel picture_%%05d.jpg\n", argv[0]);
		return -1;
	}	
	
	string ip = argv[1];
	int channel = atoi(argv[2]);
	string pic_format = "";
	int save_pic = 0;
	if (argc == 4) {
		pic_format = argv[3];
		if (channel < 2)
			save_pic = 1;
	}
		
	
	if(signal(SIGINT, signal_handler) == SIG_ERR) {
        return -1;
    }
	
	media_record record;
	
	stereo_camera cam;
	ret = cam.open_device(ip.c_str(), 7070, 9090, channel);
	if (ret < 0)
	{
		printf("Failed to open camera: %s\n", ip.c_str());
		return -1;
	}	
	
	int frame_count = 0;
	char filename[2048];
	while(going)
	{
		ret = cam.query_frame(5);
		if (ret < 0) { 
			printf("reconnect: %d\n", cam.get_reconnect_count()); 
			continue;
		}
			
		
		std::vector<unsigned char> image;
		int image_count = 0;
		std::vector<struct stereo_detect_box> detect_boxes;
		struct stereo_gyro_angle gyro_angle;
		
		cam.get_image(image);
		cam.get_frame_count(image_count);
		
		if ((save_pic) && (image.size() > 1))
		{
			memset(filename, 0, sizeof(filename));
			sprintf(filename, pic_format.c_str(), frame_count);
			record.save_picture(filename, (unsigned char *)&image[0], image.size());
			printf("save %s\n", filename);
		}	
		
		printf("image%d_%d: %d\n", frame_count, image_count, image.size());
		
		ret = cam.get_detect_boxes(detect_boxes);
		if (ret == 0)
		{
			printf("detect_boxes-------->\n");
			for (int i = 0; i < detect_boxes.size(); i++)
			{
				struct stereo_detect_box &detect_box = detect_boxes[i];
				printf("%d: graph:(%d %d %d %d) disp:(%d %d %d) camera:(%d %d %d) room:(%d %d %d) ball:(%f %f %f)\n", 
					i, detect_box.box_x, detect_box.box_y, detect_box.box_w, detect_box.box_h, 
					detect_box.x, detect_box.y, detect_box.d, 
					detect_box.xcm, detect_box.ycm, detect_box.zcm, 
					detect_box.xtcm, detect_box.ytcm, detect_box.ztcm, 
					detect_box.xa, detect_box.ya, detect_box.r);
			}	
			
		}	
		
		ret = cam.get_gyro_angle(gyro_angle);
		if (ret == 0)
		{
			printf("pitch: %f, roll: %f\n", gyro_angle.pitch, gyro_angle.roll);
		}	
		
		printf("\n\n");
		frame_count++;
	}	
	
	return 0;
}