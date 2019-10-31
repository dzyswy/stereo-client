#ifndef __FTP_CLIENT_H
#define __FTP_CLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream> 
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <ctime>


#define off64_t long
#define fseeko64 fseek
#define fopen64 fopen


class ftplib;

class ftp_client
{
public:
	ftp_client(const char *host, const char *user, const char *passwd);
	~ftp_client();
	int connect_device();
	int disconnect_device();
	int upload(const char *src_name, const char *dst_name, std::function<int(long, long)> func = NULL, std::function<int(int)> over_func = NULL);
	long get_xfered();
	long get_file_size();
	void set_busy(int value);
	int get_busy();
	void set_success(int value);
	int get_success();
	
protected:	
	static int xfer_callback(off64_t xfered, void* arg);
	int check_ip(const char *value);
	
protected:
	std::string host_;
	std::string user_;
	std::string passwd_;
	
	int connect_;
	
	std::string src_name_;
	std::string dst_name_;
	long xfered_;
	long file_size_;
	int busy_;
	int success_;
	
	std::mutex mux_;
	
	std::function<int(long, long)> xfer_func_;
	std::function<int(int)> over_func_;
	ftplib *ftp_;
};




















#endif

