﻿#include <iostream>

#include <vector>

#include <algorithm>

#include <fstream>

#define NOMINMAX
#include <Windows.h>

#include "CreditAccount.h"

#include "SavingsAccount.h"

#include "RuntimeError.h"

#include "Login.h"

using namespace std;



struct deleter {

	template <class T> void operator () (T* p) { delete p; }

};

// 恢复历史记录
void restore(string user_info, Date& date, vector<Account*>& accounts);

// 当前月账户统计
void AccountStatistics(Account* account);

int run() {

	Date date(2008, 11, 1);//起始日期

	vector<Account*> accounts;//创建账户数组，元素个数为0

	Login login;

	if (login.Init() == 0)
	{
		cout << "Thanks for your using!" << endl;
		return 0;
	}

	cout << "Welcome to you! Loading..." << endl;

	Sleep(800);

	system("cls");

	string user_info = login.getUserName() + "_commands.txt";

	ofstream text(user_info, ios::app);

	restore(user_info, date, accounts);

	cout << endl;

	for (auto account : accounts)
	{
		AccountStatistics(account);

		account->RepaymentReminder();

		cout << endl;
	}

	cout << "(a)add account (d)deposit (w)withdraw (s)show (c)change day (n)next month (q)query (e)exit" << endl;

	char cmd = 0;

	vector<char> commands{ 'a', 'd', 'w', 's', 'c', 'n', 'q' };

	do {

		//显示日期和总金额

		date.show();

		cout << "\tTotal: " << Account::getTotal() << "\tcommand> ";

		char type;

		int index, day;

		double amount, credit, rate, fee;

		string id, desc;

		Account* account;

		Date date1, date2;

		string command;

		cin >> command;

		if (command.size() == 1)
		{
			cmd = command[0];
		}
		else
		{
			RuntimeError::Error(INPUTERRORCOMMAND);
			cin.ignore(std::numeric_limits<streamsize>::max(), '\n'); // 清空缓冲区输入
			continue;
		}

		// 如果还未停止输入
		if (cmd != 'e')
		{
			vector<char>::iterator c(commands.begin());
			for (; c != commands.end(); c++)
			{
				if (*c == cmd && *c != 's' && *c != 'q')
				{
					// text << cmd << ' ';
					break;
				}
			}
			if (c == commands.end())
			{
				RuntimeError::Error(INPUTNOCOMMAND);
				cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
				break;
			}

		}

		switch (cmd) {

		case 'a'://增加账户

			cin >> type >> id;
			if (type == 's' || type == 'c')
			{

				if (type == 's') {

					cin >> rate;

					if (rate < 1)
					{
						text << cmd << ' ' << type << ' ' << id << ' ' << rate << endl;

						account = new SavingsAccount(date, id, rate);
					}

					else
					{
						RuntimeError::Error(RATEERROR);

						continue;
					}

				}

				else {

					cin >> credit >> rate >> fee;

					text << cmd << ' ' << type << id << ' ' << credit << ' ' << rate << ' ' << fee << endl;

					account = new CreditAccount(date, id, credit, rate, fee);

				}

				accounts.push_back(account);
			}
			else
			{
				RuntimeError::Error(CREATEACCOUNTERROR);
				continue;
			}


			break;

		case 'd'://存入现金

			cin >> index >> amount;

			if ((unsigned)index >= accounts.size())
			{
				RuntimeError::Error(ACCOUNTNOEXIST);
				continue;
			}
			if (amount < 0)
			{
				RuntimeError::Error(DEPOSITERROR);
			}

			getline(cin, desc);

			text << cmd << ' ' << index << ' ' << amount << ' ' << desc << endl;

			accounts[index]->deposit(date, amount, desc);

			break;

		case 'w'://取出现金

			cin >> index >> amount;

			if ((unsigned)index >= accounts.size())
			{
				RuntimeError::Error(ACCOUNTNOEXIST);
				continue;
			}
			if (amount <= 0)
			{
				RuntimeError::Error(WITHDRAWERROR);
				continue;
			}

			getline(cin, desc);

			// 取钱成功
			if (accounts[index]->withdraw(date, amount, desc) == true)
			{
				text << cmd << ' ' << index << ' ' << amount << ' ' << desc << endl;
			}

			break;

		case 's'://查询各账户信息

			for (size_t i = 0; i < accounts.size(); i++) {

				cout << "[" << i << "] ";

				accounts[i]->show();

				cout << endl;

			}

			break;

		case 'c'://改变日期

			cin >> day;

			if (day < date.getDay())
			{
				RuntimeError::Error(CHANGEPREVIOUSDATE);
				continue;
			}
			else if (day > date.getMaxDay())
			{
				RuntimeError::Error(CHANGEDATEINVAILD);
				continue;
			}
			else
			{
				date = Date(date.getYear(), date.getMonth(), day);
				text << cmd << ' ' << day << endl;
			}
			break;

		case 'n'://进入下个月

			text << cmd << endl;
			
			if (date.getMonth() == 12)
			{
				date = Date(date.getYear() + 1, 1, 1);
			}
			else
			{
				date = Date(date.getYear(), date.getMonth() + 1, 1);
			}
			for (vector<Account*>::iterator iter = accounts.begin(); iter != accounts.end(); ++iter)
			{
				(*iter)->settle(date);
			}
			break;

		case 'q'://查询一段时间内的账目

			char query_way;	// 查询方式 t: 按时间排序 a 按交易金额大小排序

			cin >> query_way;

			date1 = Date::read();

			date2 = Date::read();

			if (date1.getYear() == -1 || date2.getYear() == -1)
			{
				RuntimeError::Error(DATEINPUTINVAILD);

				continue;
			}

			if (date2 < date1)
			{
				RuntimeError::Error(DATEORDERERROR);

				continue;
			}

			if (query_way == 't')	// 按时间查询
			{
				Account::QuaryByDate(date1, date2);

				break;
			}
			else if (query_way == 'a')	// 按交易金额从大到小查询
			{
				Account::QueryByAmount(date1, date2);

				break;
			}
			else
			{
				RuntimeError::Error(INPUTNOCOMMAND);

				break;
			}

		}

	} while (cmd != 'e');

	for_each(accounts.begin(), accounts.end(), deleter());

	total = 0;

	return 1;

}

void restore(string user_info, Date& date, vector<Account*>& accounts)
{
	ifstream text(user_info);

	char cmd;

	while (text.peek() != EOF)
	{
		char type;

		int index, day;

		double amount, credit, rate, fee;

		string id, desc;

		Account* account;

		Date date1, date2;



		text >> cmd;

		switch (cmd) {

		case 'a'://增加账户

			text >> type >> id;

			if (type == 's') {

				text >> rate;

				account = new SavingsAccount(date, id, rate);

			}

			else {

				text >> credit >> rate >> fee;

				account = new CreditAccount(date, id, credit, rate, fee);

			}

			accounts.push_back(account);

			break;

		case 'd'://存入现金

			text >> index >> amount;

			getline(text, desc);

			accounts[index]->deposit(date, amount, desc);

			break;

		case 'w'://取出现金

			text >> index >> amount;

			getline(text, desc);

			accounts[index]->withdraw(date, amount, desc);

			break;

		case 's'://查询各账户信息

			for (size_t i = 0; i < accounts.size(); i++) {

				cout << "[" << i << "] ";

				accounts[i]->show();

				cout << endl;

			}

			break;

		case 'c'://改变日期

			text >> day;

			date = Date(date.getYear(), date.getMonth(), day);

			break;

		case 'n'://进入下个月

			if (date.getMonth() == 12)

				date = Date(date.getYear() + 1, 1, 1);

			else

				date = Date(date.getYear(), date.getMonth() + 1, 1);

			for (vector<Account*>::iterator iter = accounts.begin(); iter != accounts.end(); ++iter)

				(*iter)->settle(date);

			break;

		case 'q'://查询一段时间内的账目

			date1 = Date::read();

			date2 = Date::read();

			Account::QuaryByDate(date1, date2);

			break;

		}

	}
}

void AccountStatistics(Account* account)
{
	multimap<Date, AccountRecord> temp;

	// 如果记录不为空, 在该账号中找到该账户所有的操作
	if (recordMap.size() != 0)
	{
		for (auto record : recordMap)
		{
			if (record.second.getAccount() == account)
			{
				temp.insert(record);
			}
		}
		multimap<Date, AccountRecord>::iterator iter(--temp.end());//最后一次操作
		Date lastDate = iter->second.getDate();

		int year = lastDate.getYear();
		int month = lastDate.getMonth();

		double income = 0;
		double expenditure = 0;

		for (auto record : temp)
		{
			if (Date(year, month, 0) < record.second.getDate() || Date(year, month, 32) < record.second.getDate())
			{
				double amount = record.second.getAmount();
				if (amount >= 0)
				{
					income += amount;
				}
				else
				{
					expenditure -= amount;
				}
			}
		}

		cout << account->getId() << "'s statistics for this month: " << '\n' << "income: " << income << "\texpenditure: " << expenditure << endl;
	}
}

int main()
{
	while (run() == 1)
	{
		cout << "Hope to see you again!" << endl;
		Sleep(800);
		system("cls");
	}
}