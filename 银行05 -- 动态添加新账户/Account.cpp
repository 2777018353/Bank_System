#include "Account.h"

double total = 0;

void Account::show()
{
	cout << id << "\tBalance: " << balance;
}

multimap<Date, AccountRecord> recordMap;

void Account::query(Date start, Date end)
{
	for (auto record : recordMap)
	{
		if (start < record.first && record.first < end)
		{
			record.second.show();
		}
	}
}

AccountRecord::AccountRecord(Date date, Account* account, double amount, double balance, std::string desc)
{
	this->date = date;
	this->account = account;
	this->amount = amount;
	this->balance = balance;
	this->desc = desc;
}

void AccountRecord::show()
{
	date.show();
	cout << "\t#" << account->getId() << '\t' << amount << '\t' << balance << endl;
}
