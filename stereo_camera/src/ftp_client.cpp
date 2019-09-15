#include "ftp_client.h"

#include "ftplib.h"



using namespace std;


void split_path(const char *path, std::string &drive, std::string &dir, std::string &fname, std::string &ext);


ftp_client::ftp_client(const char *host, const char *user, const char *passwd)
{
	host_ = host;
	user_ = user;
	passwd_ = passwd;
	
	connect_ = 0;
	busy_ = 0;
	success_ = 0;
	xfered_ = 0;
	file_size_ = 0;
	
	ftp_ = new ftplib();
	
	ftp_->SetCallbackArg(this);
	ftp_->SetCallbackBytes(1024); // issue a xfer callback every kb
	ftp_->SetCallbackXferFunction(ftp_client::xfer_callback);
}

ftp_client::~ftp_client()
{
	disconnect_device();
	delete ftp_;
}


int ftp_client::connect_device()
{
	int ret;
	std::unique_lock<std::mutex> lock(mux_);
	if (connect_)
		return -1;
	
	xfered_ = 0;
	file_size_ = 0;
	
	ret = ftp_->Connect(host_.c_str());
	if (!ret) {
		cout << "ftp connect error, host:" << host_ << endl;
		return -1;
	}
	
	if ((passwd_.empty()) || (passwd_ == string("null")))
		passwd_ = "";
	
	ret = ftp_->Login(user_.c_str(), passwd_.c_str());
	if (!ret) {
		cout << "ftp login error, user:" << user_ << " passwd: " << passwd_ << endl;
		return -1;
	}
	connect_ = 1;
	busy_ = 0;
	success_ = 0;
	
	return 0;
}


int ftp_client::disconnect_device()
{
	std::unique_lock<std::mutex> lock(mux_);
	
	ftp_->Quit();
	connect_ = 0;
	return 0;
}

int ftp_client::upload(const char *src_name, const char *dst_name, std::function<int(long, long)> xfer_func, std::function<int(int)> over_func)
{
	int ret;
	std::unique_lock<std::mutex> lock(mux_);
	if (!connect_)
		return -1;
	
	if (busy_)
		return -1;
	
	success_ = 0;
	
	src_name_ = src_name;
	dst_name_ = dst_name;
	xfer_func_ = xfer_func;
	over_func_ = over_func;
	
	std::thread t([&] () {
			set_busy(1);
			while(1)
			{
				FILE *fp = fopen(src_name_.c_str(), "rb");
				if (!fp) {
					cout << "ftp open " << src_name_ << " error!\n";
					break;
				}
				fseek(fp, 0L, SEEK_END);
				file_size_ = ftell(fp);
				fclose(fp);
				
				 
				string dst_drive, dst_dir, dst_fname, dst_ext;
				split_path(dst_name_.c_str(), dst_drive, dst_dir, dst_fname, dst_ext);
			//	cout << dst_drive << endl;
			//	cout << dst_dir << endl;
			//	cout << dst_fname << endl;
			//	cout << dst_ext << endl;
				
				
				if (!dst_dir.empty()) {
					ret = ftp_->Chdir(dst_dir.c_str());
					if (!ret) {
						cout << "ftp chdir error, dir:" << dst_dir << endl;;
						break;
					}
				}
				
				string dst_file = dst_fname;
				dst_file += dst_ext;
				

				ret = ftp_->Put(src_name_.c_str(), dst_file.c_str(), ftplib::image);
				if (!ret) {
					cout << "ftp put error, from " << src_name_ << " to " << dst_file << endl;
					break;
				}
				
				set_success(1);
				set_busy(0);
				return 0;
			}	
			set_success(0);
			set_busy(0);
			
			return -1;
		});
	t.detach();
	
	return 0;
}

long ftp_client::get_xfered()
{
	std::unique_lock<std::mutex> lock(mux_);
	return xfered_;
}

long ftp_client::get_file_size()
{
	std::unique_lock<std::mutex> lock(mux_);
	return file_size_;
}

void ftp_client::set_success(int value)
{
	std::unique_lock<std::mutex> lock(mux_);
	success_ = value;
	if (over_func_)
		over_func_(success_);
}

int ftp_client::get_success()
{
	std::unique_lock<std::mutex> lock(mux_);
	return success_;
}

void ftp_client::set_busy(int value)
{
	std::unique_lock<std::mutex> lock(mux_);
	busy_ = value; 
}

int ftp_client::get_busy()
{
	std::unique_lock<std::mutex> lock(mux_);
	return busy_;
}
 
int ftp_client::xfer_callback(off64_t xfered, void* arg)
{
	ftp_client *client = (ftp_client *)arg;
	
	int ret;
	client->xfered_ = xfered;
	if (client->xfer_func_)
	{
		ret = client->xfer_func_(client->xfered_, client->file_size_);
		if (ret < 0)
			return 0;
	}	
	return 1;
}























#if WIN32
void split_path(const char *path, std::string &drive, std::string &dir, std::string &fname, std::string &ext)
{
	char _drive[_MAX_DRIVE];  
    char _dir[_MAX_DIR];  
    char _fname[_MAX_FNAME];
    char _ext[_MAX_EXT];
	
	_splitpath(path, _drive, _dir, _fname, _ext);
	drive = string(_drive);
	dir = string(_dir);
	fname = string(_fname);
	ext = string(_ext);
}


#else




void _split_whole_name(const char *whole_name, char *fname, char *ext)
{
	char *p_ext;
 
	p_ext = rindex(whole_name, '.');
	if (NULL != p_ext)
	{
		strcpy(ext, p_ext);
		snprintf(fname, p_ext - whole_name + 1, "%s", whole_name);
	}
	else
	{
		ext[0] = '\0';
		strcpy(fname, whole_name);
	}
}

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext)
{
	char *p_whole_name;
 
	drive[0] = '\0';
	if (NULL == path)
	{
		dir[0] = '\0';
		fname[0] = '\0';
		ext[0] = '\0';
		return;
	}
 
	if ('/' == path[strlen(path)])
	{
		strcpy(dir, path);
		fname[0] = '\0';
		ext[0] = '\0';
		return;
	}
 
	p_whole_name = rindex(path, '/');
	if (NULL != p_whole_name)
	{
		p_whole_name++;
		_split_whole_name(p_whole_name, fname, ext);
 
		snprintf(dir, p_whole_name - path, "%s", path);
	}
	else
	{
		_split_whole_name(path, fname, ext);
		dir[0] = '\0';
	}
}
 
void split_path(const char *path, std::string &drive, std::string &dir, std::string &fname, std::string &ext)
{
	int len = strlen(path);
	char *_drive = new char [len];
	char *_dir = new char [len];
	char *_fname = new char [len];
	char *_ext = new char [len];
	
	_splitpath(path, _drive, _dir, _fname, _ext);
	
	drive = string(_drive);
	dir = string(_dir);
	fname = string(_fname);
	ext = string(_ext);
	
	delete _drive;
	delete _dir;
	delete _fname;
	delete _ext;
	
}


#endif












