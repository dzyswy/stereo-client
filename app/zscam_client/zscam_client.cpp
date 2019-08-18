#include <iostream>

#include "camera_command.h"
#include "system_command.h"
#include "discovery_receiver.h"
#include "stream_receiver.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	int ret;
	if (argc != 4)
	{
		printf("usage: %s ip port stream_id\n", argv[0]);
		return -1;
	}	
	
	char *ip = strdup(argv[1]);
	int port = atoi(argv[2]);
	int stream_id = atoi(argv[3]);
	
	vector<unsigned char> frame_buffer;
	vector<struct stereo_detect_box> detect_boxes;
	struct stereo_gyro_angle gyro_angle;
	
	
	stream_receiver mstream;
	
	mstream.connect_stream(ip, port, stream_id);
	
	int frame_count = 0;
	while(1)
	{
		ret = mstream.query_frame();
		if (ret < 0)
		{
			continue;
		}	
		mstream.get_frame(frame_buffer);
		mstream.get_detect_boxes(detect_boxes);
		mstream.get_gyro_angle(gyro_angle);
		
		if (frame_buffer.size() > 10)
		{
			Mat jpegimage = imdecode(Mat(frame_buffer), CV_LOAD_IMAGE_COLOR);
		
			imshow("jpeg", jpegimage);
			waitKey(20);
		}
		
		
		cout << "frame_count: " << frame_count << endl;
		cout << "frame_size: " << frame_buffer.size() << endl;
		cout << "detect_boxes: " << detect_boxes.size() << " box_w: " << detect_boxes[0].box_w << " xa: " << detect_boxes[1].xa << endl;
		cout << "roll: " << gyro_angle.roll << ", pitch: " << gyro_angle.pitch << endl;
	}	
	
	return 0;
}








