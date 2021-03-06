#pragma once
#include <iostream>
using namespace std;

class Date			// 建立日期类
{
private:
	int year;		// 年
	int month;		// 月
	int day;		// 日
	int totalDays;	// 总天数

public:
	// 构造函数
	Date() :year(0), month(0), day(0) { totalDays = 0; };
	Date(int year, int month, int day) :year(year), month(month), day(day)
	{
		totalDays = 0;
	}

	// 重载 < 运算符
	bool operator < (const Date& date) const;

	// get 方法
	int getYear() const { return year; }
	int getMonth() const { return month; }
	int getDay() const { return day; }
	int getMaxDay() const;

	// 检测输入是否正确
	static bool ifDate(const Date& date);
	// 判断是否是闰年
	static bool isLeapYear(int year) {return (year % 4 == 0 && year % 100 != 0 || year % 400 == 0);}
	// 格式化输出
	void show() const { cout << year << '-' << month << '-' << day; }
	// 计算与目标日期之间的天数
	int distance(Date date) const;
	// 读入日期并初始化
	static Date read();
};

