#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <iostream>
#include <string>
#include <limits>

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

int main()
{
  // === ГЛОБАЛЬНОЕ СОСТОЯНИЕ (в пределах main) ===
  mongocxx::instance mongo_instance{};
  mongocxx::client mongo_client{mongocxx::uri{"mongodb://localhost:27017"}};
  mongocxx::database mongo_db = mongo_client["bank_system"];
  mongocxx::collection mongo_collection = mongo_db["accounts"];

  int choice = 0;
  bool running = true;

  std::cout << "Starting Bank Management System..." << std::endl;

  // === ОСНОВНОЙ ЦИКЛ ПРОГРАММЫ ===
  while (running)
  {
    // Отображение меню
    std::cout << "\n==================================" << std::endl;
    std::cout << "     BANK MANAGEMENT SYSTEM       " << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "1. Create New Account" << std::endl;
    std::cout << "2. Deposit Money" << std::endl;
    std::cout << "3. Withdraw Money" << std::endl;
    std::cout << "4. Check Balance" << std::endl;
    std::cout << "5. Display Account Information" << std::endl;
    std::cout << "6. Delete Account" << std::endl;
    std::cout << "7. Exit" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Enter your choice (1-7): ";
    std::cin >> choice;

    // === БЛОК 1: СОЗДАНИЕ СЧЕТА ===
    if (choice == 1)
    {
      std::string account_id, name;
      double initial_balance;

      std::cout << "\n=== Create New Account ===" << std::endl;
      std::cout << "Enter Account ID: ";
      std::cin.ignore();
      std::getline(std::cin, account_id);

      // Проверка существования счета
      auto check_filter = document{} << "account_id" << account_id << finalize;
      auto check_result = mongo_collection.find_one(check_filter.view());

      if (check_result)
      {
        std::cout << "Account already exists!" << std::endl;
      }
      else
      {
        std::cout << "Enter Account Holder Name: ";
        std::getline(std::cin, name);

        std::cout << "Enter Initial Balance: ";
        std::cin >> initial_balance;

        if (initial_balance >= 0)
        {
          // Создание документа
          auto builder = bsoncxx::builder::stream::document{};
          bsoncxx::document::value doc = builder
                                         << "account_id" << account_id
                                         << "name" << name
                                         << "balance" << initial_balance
                                         << bsoncxx::builder::stream::finalize;

          auto result = mongo_collection.insert_one(doc.view());

          if (result)
          {
            std::cout << "Account created successfully!" << std::endl;
          }
          else
          {
            std::cout << "Failed to create account!" << std::endl;
          }
        }
        else
        {
          std::cout << "Initial balance cannot be negative!" << std::endl;
        }
      }
    }

    // === БЛОК 2: ПОПОЛНЕНИЕ СЧЕТА ===
    else if (choice == 2)
    {
      std::string account_id;
      double amount;

      std::cout << "\n=== Deposit Money ===" << std::endl;
      std::cout << "Enter Account ID: ";
      std::cin.ignore();
      std::getline(std::cin, account_id);

      // Проверка существования счета
      auto check_filter = document{} << "account_id" << account_id << finalize;
      auto check_result = mongo_collection.find_one(check_filter.view());

      if (!check_result)
      {
        std::cout << "Account not found!" << std::endl;
      }
      else
      {
        std::cout << "Enter Deposit Amount: ";
        std::cin >> amount;

        if (amount > 0)
        {
          // Обновление баланса
          auto filter = document{} << "account_id" << account_id << finalize;
          auto update = document{}
                        << "$inc" << open_document << "balance" << amount << close_document
                        << bsoncxx::builder::stream::finalize;

          auto result = mongo_collection.update_one(filter.view(), update.view());

          if (result && result->modified_count() > 0)
          {
            // Получение нового баланса
            auto get_filter = document{} << "account_id" << account_id << finalize;
            auto get_result = mongo_collection.find_one(get_filter.view());
            if (get_result)
            {
              auto view = get_result->view();
              double new_balance = view["balance"].get_double().value;
              std::cout << "Deposit successful! New Balance: " << new_balance << std::endl;
            }
          }
          else
          {
            std::cout << "Deposit failed!" << std::endl;
          }
        }
        else
        {
          std::cout << "Deposit amount must be positive!" << std::endl;
        }
      }
    }

    // === БЛОК 3: СНЯТИЕ СРЕДСТВ ===
    else if (choice == 3)
    {
      std::string account_id;
      double amount;

      std::cout << "\n=== Withdraw Money ===" << std::endl;
      std::cout << "Enter Account ID: ";
      std::cin.ignore();
      std::getline(std::cin, account_id);

      // Проверка существования счета
      auto check_filter = document{} << "account_id" << account_id << finalize;
      auto check_result = mongo_collection.find_one(check_filter.view());

      if (!check_result)
      {
        std::cout << "Account not found!" << std::endl;
      }
      else
      {
        std::cout << "Enter Withdrawal Amount: ";
        std::cin >> amount;

        if (amount > 0)
        {
          // Проверка достаточности средств
          auto balance_filter = document{} << "account_id" << account_id << finalize;
          auto balance_result = mongo_collection.find_one(balance_filter.view());
          auto view = balance_result->view();
          double current_balance = view["balance"].get_double().value;

          if (current_balance >= amount)
          {
            // Обновление баланса
            auto filter = document{} << "account_id" << account_id << finalize;
            auto update = document{}
                          << "$inc" << open_document << "balance" << -amount << close_document
                          << bsoncxx::builder::stream::finalize;

            auto result = mongo_collection.update_one(filter.view(), update.view());

            if (result && result->modified_count() > 0)
            {
              double new_balance = current_balance - amount;
              std::cout << "Withdrawal successful! New Balance: " << new_balance << std::endl;
            }
            else
            {
              std::cout << "Withdrawal failed!" << std::endl;
            }
          }
          else
          {
            std::cout << "Insufficient funds! Current balance: " << current_balance << std::endl;
          }
        }
        else
        {
          std::cout << "Withdrawal amount must be positive!" << std::endl;
        }
      }
    }

    // === БЛОК 4: ПРОВЕРКА БАЛАНСА ===
    else if (choice == 4)
    {
      std::string account_id;

      std::cout << "\n=== Check Balance ===" << std::endl;
      std::cout << "Enter Account ID: ";
      std::cin.ignore();
      std::getline(std::cin, account_id);

      // Поиск счета
      auto filter = document{} << "account_id" << account_id << finalize;
      auto result = mongo_collection.find_one(filter.view());

      if (result)
      {
        auto view = result->view();
        double balance = view["balance"].get_double().value;
        std::cout << "Current Balance: " << balance << std::endl;
      }
      else
      {
        std::cout << "Account not found!" << std::endl;
      }
    }

    // === БЛОК 5: ИНФОРМАЦИЯ О СЧЕТЕ ===
    else if (choice == 5)
    {
      std::string account_id;

      std::cout << "\n=== Account Information ===" << std::endl;
      std::cout << "Enter Account ID: ";
      std::cin.ignore();
      std::getline(std::cin, account_id);

      // Поиск счета
      auto filter = document{} << "account_id" << account_id << finalize;
      auto result = mongo_collection.find_one(filter.view());

      if (result)
      {
        auto view = result->view();
        std::cout << "\n=== Account Details ===" << std::endl;
        std::cout << "Account ID: " << view["account_id"].get_string().value << std::endl;
        std::cout << "Name: " << view["name"].get_string().value << std::endl;
        std::cout << "Balance: " << view["balance"].get_double().value << std::endl;
      }
      else
      {
        std::cout << "Account not found!" << std::endl;
      }
    }

    // === БЛОК 6: УДАЛЕНИЕ СЧЕТА ===
    else if (choice == 6)
    {
      std::string account_id;
      char confirm;

      std::cout << "\n=== Delete Account ===" << std::endl;
      std::cout << "Enter Account ID: ";
      std::cin.ignore();
      std::getline(std::cin, account_id);

      // Проверка существования счета
      auto check_filter = document{} << "account_id" << account_id << finalize;
      auto check_result = mongo_collection.find_one(check_filter.view());

      if (!check_result)
      {
        std::cout << "Account not found!" << std::endl;
      }
      else
      {
        std::cout << "Are you sure you want to delete account '" << account_id << "'? (y/n): ";
        std::cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
          auto filter = document{} << "account_id" << account_id << finalize;
          auto result = mongo_collection.delete_one(filter.view());

          if (result && result->deleted_count() > 0)
          {
            std::cout << "Account deleted successfully!" << std::endl;
          }
          else
          {
            std::cout << "Failed to delete account!" << std::endl;
          }
        }
        else
        {
          std::cout << "Account deletion cancelled." << std::endl;
        }
      }
    }

    // === БЛОК 7: ВЫХОД ===
    else if (choice == 7)
    {
      std::cout << "\nThank you for using our banking system!" << std::endl;
      std::cout << "Goodbye!" << std::endl;
      running = false;
    }

    // === БЛОК 8: НЕПРАВИЛЬНЫЙ ВЫБОР ===
    else
    {
      std::cout << "Invalid choice! Please enter a number between 1 and 7." << std::endl;
    }

    // Пауза для продолжения
    if (choice != 7)
    {
      std::cout << "\nPress Enter to continue...";
      std::cin.ignore();
      std::cin.get();
    }
  }

  return 0;
}