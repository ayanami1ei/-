#pragma once
#include"SmartPoint.h"
#include<vector>
#include<string>

class detecor
{
public:
	void set_img_path(std::string& path);

	void run();
private:
	class Ipml;
	SmartPoint<Ipml> pipml;
};