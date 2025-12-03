#ifndef BANKACCOUNT_H
#define BANKACCOUNT_H

#include <string>

class BankAccount
{
private:
  std::string account_id;
  std::string name;
  double balance;

public:
  BankAccount(const std::string &id, const std::string &owner_name, double initial_balance);

  // Геттеры
  std::string getAccountId() const;
  std::string getName() const;
  double getBalance() const;

  // Операции со счетом
  bool deposit(double amount);
  bool withdraw(double amount);

  void displayInfo() const;
};

#endif