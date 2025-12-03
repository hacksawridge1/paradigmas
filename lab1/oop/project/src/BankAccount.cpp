#include "BankAccount.h"
#include <iostream>

BankAccount::BankAccount(const std::string &id, const std::string &owner_name, double initial_balance)
    : account_id(id), name(owner_name), balance(initial_balance)
{
  if (initial_balance < 0)
  {
    throw std::invalid_argument("Initial balance cannot be negative");
  }
}

std::string BankAccount::getAccountId() const
{
  return account_id;
}

std::string BankAccount::getName() const
{
  return name;
}

double BankAccount::getBalance() const
{
  return balance;
}

bool BankAccount::deposit(double amount)
{
  if (amount <= 0)
  {
    std::cout << "Deposit amount must be positive!" << std::endl;
    return false;
  }
  balance += amount;
  return true;
}

bool BankAccount::withdraw(double amount)
{
  if (amount <= 0)
  {
    std::cout << "Withdrawal amount must be positive!" << std::endl;
    return false;
  }

  if (balance >= amount)
  {
    balance -= amount;
    return true;
  }
  else
  {
    std::cout << "Insufficient funds! Current balance: " << balance << std::endl;
    return false;
  }
}

void BankAccount::displayInfo() const
{
  std::cout << "\n=== Account Information ===" << std::endl;
  std::cout << "Account ID: " << account_id << std::endl;
  std::cout << "Name: " << name << std::endl;
  std::cout << "Balance: " << balance << std::endl;
}