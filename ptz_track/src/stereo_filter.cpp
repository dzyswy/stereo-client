#include "stereo_filter.h"


using namespace std;

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

stereo_filter::stereo_filter(stereo_camera *camera)
{
	camera_ = camera;
	
	min_number_count_ = 2;
	max_number_count_ = 4;
	stable_distance_ = 20;
	min_stable_count_ = 3;
	 
	clear_filter(); 
}

void stereo_filter::compute(std::vector<struct stereo_detect_box> &detect_boxes, int &number_state, struct stereo_detect_box &focus_box, int &stable_state)
{ 
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
				float dx = fabs(pre_box.x - detect_box.x) / 960;
				float dy = fabs(pre_box.y - detect_box.y) / 540;
				float dz = fabs(pre_box.d - detect_box.d) / 256;
				
				float dist = sqrt(dx * dx + dy * dy + dz * dz);
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
	pre_focus_box_ = focus_box;
	
	
	focus_boxes_[frame_count_ % STEREO_FILTER_MAX_FOCUS_BOX_NUM] = focus_box;
	
	//stable
	if (frame_count_ > STEREO_FILTER_MAX_FOCUS_BOX_NUM)
	{ 
		float x[2], y[2], z[2];
		for (int i = 0; i < STEREO_FILTER_MAX_FOCUS_BOX_NUM; i++)
		{
			float xa = (float)focus_boxes_[i].xa;
			float ya = (float)focus_boxes_[i].ya;
			float r = (float)focus_boxes_[i].r;
			
			x[0] = (i == 0) ? xa : min(x[0], xa);
			x[1] = (i == 0) ? xa : max(x[1], xa);
			
			y[0] = (i == 0) ? ya : min(y[0], ya);
			y[1] = (i == 0) ? ya : max(y[1], ya);
			
		//	z[0] = (i == 0) ? r : min(z[0], r);
		//	z[1] = (i == 0) ? r : max(z[1], r);
		}	
		
		float dx = x[0] - x[1];
		float dy = y[0] - y[1];
		float agl = sqrt(dx * dx + dy * dy);
		float dist = agl * 3.1415926 * focus_box.r / 180;
		 
		if (dist < stable_distance_)
		{
			stable_count_++;
		}	
		else
		{
			stable_count_ = 0;
		}
		
	}	
	
	stable_state = (stable_count_ >= min_stable_count_) ? 1 : 0;	
	
	
	frame_count_++;	 
}




void stereo_filter::clear_filter()
{ 
	frame_count_ = 0;

	number_state_ = STEREO_FILTER_NO_TARGET;
	memset(&number_count_, 0, sizeof(number_count_));
 
	memset(&pre_focus_box_, 0, sizeof(pre_focus_box_));
	stable_count_ = 0;
	
	memset(&focus_boxes_[0], 0, sizeof(focus_boxes_));
}














































