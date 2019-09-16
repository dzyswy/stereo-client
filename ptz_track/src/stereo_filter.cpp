#include "stereo_filter.h"


using namespace std;



stereo_filter::stereo_filter(stereo_camera *camera)
{
	camera_ = camera;
	
	min_number_count_ = 2;
	max_number_count_ = 4;
	stable_angle_ = 1;
	stable_distance_ = 50;
	min_stable_count_ = 3;
	
	focus_boxes_.resize(STEREO_FILTER_MAX_FOCUS_BOX_NUM);
	
	clear_filter(); 
}

void stereo_filter::compute(std::vector<struct stereo_detect_box> &detect_boxes, int &number_state, struct stereo_detect_box &focus_box, int &statble_state)
{
	static int frame_count = 0;
	frame_count++;
	
	int detect_count = detect_boxes.size();
	if (detect_count == 0) {
		number_count_[STEREO_FILTER_NO_TARGET]++;
		number_count_[STEREO_FILTER_SINGLE_TARGET] = 0;
		number_count_[STEREO_FILTER_MULTI_TARGET] = 0;
	} else if (detect_count == 1) {
		number_count_[STEREO_FILTER_NO_TARGET] = 0;
		number_count_[STEREO_FILTER_SINGLE_TARGET]++;
		number_count_[STEREO_FILTER_MULTI_TARGET] = 0;
	} else {
		number_count_[STEREO_FILTER_NO_TARGET] = 0;
		number_count_[STEREO_FILTER_SINGLE_TARGET] = 0;
		number_count_[STEREO_FILTER_MULTI_TARGET]++;
	}
	
	
	
	/*
	 * 1.目标状态判断
	 * 	|-根据目标计数的持续状态和当前状态进行切换。
	 *	|-避免状态的频繁切换。
	 * 2.目标位置确定
	 *	|-跟踪上一帧的相同ID目标。
	 * 	|-当没有检测到上一帧ID目标时，选择与上个ID目标直线距离最近的目标。
	 *	|-当前没有新目标出现时，维持上一帧的目标不变。
	 *	|-避免镜头大浮动摆动。
	 * 3.目标位置是否稳定
	 *	|-连续3帧之间的直线距离小于阈值，认为目标位置稳定。
	 *	|-连续3帧之间的x和y轴的变化值都小于阈值，认为目标位置稳定。
	*/

	switch(number_state_)
	{
		case STEREO_FILTER_NO_TARGET:
		{
			if (number_count_[STEREO_FILTER_SINGLE_TARGET] >= min_number_count_) {
				number_state_ = STEREO_FILTER_SINGLE_TARGET;
			} else if (number_count_[STEREO_FILTER_MULTI_TARGET] >= min_number_count_) {
				number_state_ = STEREO_FILTER_MULTI_TARGET;
			}	
		}break;
		case STEREO_FILTER_SINGLE_TARGET:
		{
			if (number_count_[STEREO_FILTER_NO_TARGET] >= max_number_count_) {
				number_state_ = STEREO_FILTER_NO_TARGET;
			} else if (number_count_[STEREO_FILTER_MULTI_TARGET] >= max_number_count_) {
				number_state_ = STEREO_FILTER_MULTI_TARGET;
			}	
		}break;
		case STEREO_FILTER_MULTI_TARGET:
		{
			if (number_count_[STEREO_FILTER_SINGLE_TARGET] >= max_number_count_) {
				number_state_ = STEREO_FILTER_SINGLE_TARGET;
			} else if (number_count_[STEREO_FILTER_NO_TARGET] >= max_number_count_) {
				number_state_ = STEREO_FILTER_NO_TARGET;
			}	
		}break;
		default:
			break;
		
	}
	number_state = number_state_;
	

	//focus_box
	struct stereo_detect_box pre_box = pre_focus_box_;
	int min_dist = 10000000, min_id = -1;
	int pre_index = -1, near_index = -1, min_index = -1;
	for (int i = 0; i < detect_count; i++)
	{
		struct stereo_detect_box &detect_box = detect_boxes[i];
		
		if (pre_box.d > 0)
		{
			if (pre_box.id == detect_box.id) {
				pre_index = i;
				break;
			}
			
			if (detect_box.d > 0)
			{
				float xcm = (pre_box.xcm - detect_box.xcm);
				float ycm = (pre_box.ycm - detect_box.ycm);
				float zcm = (pre_box.zcm - detect_box.zcm);
				float dist = sqrt(xcm * xcm + ycm * ycm + zcm * zcm);
				if (dist < min_dist)
				{
					min_dist = dist;
					near_index = i;
				}	
			}	
		}	
		

		if ((min_id == -1) || (detect_box.id < min_id)) 
		{
			min_id = detect_box.id;
			min_index = i;
		}
	}
	
	int index = -1;
	if (pre_index >= 0) {
		index = pre_index;
	} else {
		if (near_index >= 0) {
			index = near_index;
		} else {
			if (min_index >= 0) {
				index = min_index;
			}
		}
	} 
	
	if (index < 0) {
		memset(&focus_box, 0, sizeof(focus_box));
	} else {
		focus_box = detect_boxes[index];
	}

	focus_boxes_[frame_count % STEREO_FILTER_MAX_FOCUS_BOX_NUM] = focus_box;
	 
	//stable
	
	if (frame_count > STEREO_FILTER_MAX_FOCUS_BOX_NUM)
	{ 
		float x[2][2], y[2][2], z[2][2];
		for (int i = 0; i < STEREO_FILTER_MAX_FOCUS_BOX_NUM; i++)
		{
			x[0][0] = (i == 0) ? (float)focus_boxes_[i].xa : min(x[0][0], (float)focus_boxes_[i].xa);
			x[0][1] = (i == 0) ? (float)focus_boxes_[i].xcm : min(x[0][1], (float)focus_boxes_[i].xcm);
			x[1][0] = (i == 0) ? (float)focus_boxes_[i].xa : max(x[0][0], (float)focus_boxes_[i].xa);
			x[1][1] = (i == 0) ? (float)focus_boxes_[i].xcm : max(x[0][1], (float)focus_boxes_[i].xcm);
			
			y[0][0] = (i == 0) ? (float)focus_boxes_[i].ya : min(y[0][0], (float)focus_boxes_[i].ya);
			y[0][1] = (i == 0) ? (float)focus_boxes_[i].ycm : min(y[0][1], (float)focus_boxes_[i].ycm);
			y[1][0] = (i == 0) ? (float)focus_boxes_[i].ya : max(y[0][0], (float)focus_boxes_[i].ya);
			y[1][1] = (i == 0) ? (float)focus_boxes_[i].ycm : max(x[0][1], (float)focus_boxes_[i].ycm);
			
		//	z[0][0] = (i == 0) ? focus_boxes_[i].r : min(z[0][0], focus_boxes_[i].r);
		//	z[0][1] = (i == 0) ? focus_boxes_[i].zcm : min(z[0][1], focus_boxes_[i].zcm);
		//	z[1][0] = (i == 0) ? focus_boxes_[i].r : max(z[0][0], focus_boxes_[i].r);
		//	z[1][1] = (i == 0) ? focus_boxes_[i].zcm : max(z[0][1], focus_boxes_[i].zcm);
		}	
		 
		float dist_angle = max(fabs(x[0][0] - x[1][0]), fabs(y[0][0] - y[1][0]));
		float dist_space = max(fabs(x[0][1] - x[1][1]), fabs(y[0][1] - y[1][1]));
		if ((dist_angle < stable_angle_) || (dist_space < stable_distance_)) {
			statble_count_++;
		} else {
			statble_count_ = 0;
		}
		statble_state = (statble_count_ >= min_stable_count_) ? 1 : 0;	
	//	cout << "index: " << index << " ,dist_angle: " << dist_angle << " ,dist_space: " << dist_space << " ,statble_count: " << statble_count_ << endl;
		
		
	}	
	
		 
}




void stereo_filter::clear_filter()
{ 
	number_state_ = STEREO_FILTER_NO_TARGET;
	memset(&number_count_, 0, sizeof(number_count_));
	  
	statble_state_ = 0;
	statble_count_ = 0;
	memset(&pre_focus_box_, 0, sizeof(pre_focus_box_));
}














































