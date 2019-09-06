#include "zscam_client.h"
#include <QtWidgets/QApplication>


#include "stereo_camera.h"


int going = 1;


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	vector<unsigned char> frame_buffer;
	vector<struct stereo_detect_box> detect_boxes;
	struct stereo_gyro_angle gyro_angle;
	
	stereo_camera camera("zynq_stereo_camera", 45789, 5000);
	zscam_client w(&camera);
	
	
	auto func_frame = [&] () {
			int ret;
			static int frame_count = 0;
			
			while(going)
			{
				if (!camera.is_opened()) {
					std::this_thread::sleep_for (std::chrono::seconds(5));
					continue;
				}
				
				ret = camera.query_frame(5);
				if (ret < 0) {
					std::this_thread::sleep_for (std::chrono::seconds(1));
					continue;
				}
				
				camera.get_frame(frame_buffer);
				camera.get_detect_boxes(detect_boxes);
				camera.get_gyro_angle(gyro_angle);
				
				w.set_frame(frame_buffer);
				w.set_detect_boxes(detect_boxes);
				w.set_gyro_angle(gyro_angle);
				w.post_frame();
			
				cout << "func_frame frame_count: " << frame_count << endl;
				frame_count++;
			}	
			 
		};
	std::thread t(func_frame);
	
	w.show();
	a.exec();
	going = 0;
	t.join();
	
	return 0;
}
