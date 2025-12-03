#include "Database.h"
#include <iostream>
#include <limits>

void displayMenu()
{
  std::cout << "\n=== Bank Management System ===" << std::endl;
  std::cout << "1. Create Account" << std::endl;
  std::cout << "2. Deposit" << std::endl;
  std::cout << "3. Withdraw" << std::endl;
  std::cout << "4. Check Balance" << std::endl;
  std::cout << "5. Display Account Info" << std::endl;
  std::cout << "6. Delete Account" << std::endl;
  std::cout << "7. Exit" << std::endl;
  std::cout << "=============================" << std::endl;
  std::cout << "Enter your choice: ";
}

int main()
{
  try
  {
    // Подключение к MongoDB
    // Замените строку подключения на свою
    std::string mongo_uri = "mongodb://localhost:27017";
    Database db(mongo_uri, "bank_system", "accounts");

    int choice;
    std::string account_id, name;
    double amount;

    do
    {
      displayMenu();
      std::cin >> choice;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      switch (choice)
      {
      case 1: // Create Account
        std::cout << "Enter Account ID: ";
        std::getline(std::cin, account_id);
        std::cout << "Enter Name: ";
        std::getline(std::cin, name);
        std::cout << "Enter Initial Balance: ";
        std::cin >> amount;

        if (db.createAccount(account_id, name, amount))
        {
          std::cout << "Account created successfully!" << std::endl;
        }
        else
        {
          std::cout << "Failed to create account!" << std::endl;
        }
        break;

      case 2: // Deposit
        std::cout << "Enter Account ID: ";
        std::getline(std::cin, account_id);
        std::cout << "Enter Deposit Amount: ";
        std::cin >> amount;

        if (db.deposit(account_id, amount))
        {
          std::cout << "Deposit successful!" << std::endl;
          std::cout << "New Balance: " << db.getBalance(account_id) << std::endl;
        }
        else
        {
          std::cout << "Deposit failed!" << std::endl;
        }
        break;

      case 3: // Withdraw
        std::cout << "Enter Account ID: ";
        std::getline(std::cin, account_id);
        std::cout << "Enter Withdrawal Amount: ";
        std::cin >> amount;

        if (db.withdraw(account_id, amount))
        {
          std::cout << "Withdrawal successful!" << std::endl;
          std::cout << "New Balance: " << db.getBalance(account_id) << std::endl;
        }
        else
        {
          std::cout << "Withdrawal failed!" << std::endl;
        }
        break;

      case 4: // Check Balance
        std::cout << "Enter Account ID: ";
        std::getline(std::cin, account_id);
        amount = db.getBalance(account_id);
        if (amount >= 0)
        {
          std::cout << "Current Balance: " << amount << std::endl;
        }
        else
        {
          std::cout << "Account not found!" << std::endl;
        }
        break;

      case 5: // Display Account Info
        std::cout << "Enter Account ID: ";
        std::getline(std::cin, account_id);
        db.displayAccountInfo(account_id);
        break;

      case 6: // Delete Account
        std::cout << "Enter Account ID to delete: ";
        std::getline(std::cin, account_id);
        std::cout << "Are you sure? (y/n): ";
        char confirm;
        std::cin >> confirm;
        if (confirm == 'y' || confirm == 'Y')
        {
          if (db.deleteAccount(account_id))
          {
            std::cout << "Account deleted successfully!" << std::endl;
          }
          else
          {
            std::cout << "Failed to delete account!" << std::endl;
          }
        }
        break;

      case 7:
        std::cout << "Thank you for using our banking system!" << std::endl;
        break;

      default:
        std::cout << "Invalid choice! Please try again." << std::endl;
      }

    } while (choice != 7);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}