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
	
	clear_filter(); 
}

void stereo_filter::compute(std::vector<struct stereo_detect_box> &detect_boxes, int &number_state, struct stereo_detect_box &focus_box, int &statble_state)
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
	pre_focus_box_ = focus_box;
 
	 
	//stable
	float dist_angle = -1;
	if (pre_box.d)
	{
		float dx = fabs(pre_box.xa - focus_box.xa);
		float dy = fabs(pre_box.ya - focus_box.ya);
		dist_angle = max(dx, dy);
	}	
	
	float dist_space = -1;
	if (pre_box.d)
	{
		float dx = (pre_box.xcm - focus_box.xcm);
		float dy = (pre_box.ycm - focus_box.ycm);
		float dz = (pre_box.zcm - focus_box.zcm);
		
		dist_space = sqrt(dx * dx + dy * dy + dz * dz);
	}	
	
	if (((dist_angle > 0) && (dist_angle < stable_angle_)) || ((dist_space > 0)) && (dist_space < stable_distance_)) {
		statble_count_++;
	} else {
		statble_count_ = 0;
	}
	statble_state = (statble_count_ >= min_stable_count_) ? 1 : 0;	
			
}




void stereo_filter::clear_filter()
{ 
	number_state_ = STEREO_FILTER_NO_TARGET;
	memset(&number_count_, 0, sizeof(number_count_));
	  
	statble_state_ = 0;
	statble_count_ = 0;
	memset(&pre_focus_box_, 0, sizeof(pre_focus_box_));
}














































