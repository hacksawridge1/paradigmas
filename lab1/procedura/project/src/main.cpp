#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <iostream>
#include <string>
#include <limits>
#include <chrono>

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

// Глобальные переменные для соединения с БД
mongocxx::instance *mongo_instance = nullptr;
mongocxx::client *mongo_client = nullptr;
mongocxx::database *mongo_db = nullptr;
mongocxx::collection *mongo_collection = nullptr;

// Функции для работы с базой данных
void initialize_database()
{
  std::cout << "Initializing MongoDB connection..." << std::endl;

  try
  {
    // Создаем инстанс MongoDB
    mongo_instance = new mongocxx::instance();

    // Подключаемся к MongoDB
    std::string mongo_uri = "mongodb://localhost:27017";
    mongo_client = new mongocxx::client{mongocxx::uri{mongo_uri}};

    // Выбираем базу данных и коллекцию
    mongo_db = new mongocxx::database((*mongo_client)["bank_system"]);
    mongo_collection = new mongocxx::collection((*mongo_db)["accounts"]);

    std::cout << "Connected to MongoDB successfully!" << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Failed to connect to MongoDB: " << e.what() << std::endl;
    exit(1);
  }
}

void cleanup_database()
{
  std::cout << "Cleaning up database connection..." << std::endl;

  delete mongo_collection;
  delete mongo_db;
  delete mongo_client;
  delete mongo_instance;
}

bool account_exists(const std::string &account_id)
{
  auto filter = document{} << "account_id" << account_id << finalize;
  auto result = mongo_collection->find_one(filter.view());
  return result ? true : false;
}

double get_account_balance(const std::string &account_id)
{
  auto filter = document{} << "account_id" << account_id << finalize;
  auto result = mongo_collection->find_one(filter.view());

  if (result)
  {
    auto view = result->view();
    return view["balance"].get_double().value;
  }
  return -1.0;
}

bool create_account()
{
  std::string account_id, name;
  double initial_balance;

  std::cout << "\n=== Create New Account ===" << std::endl;
  std::cout << "Enter Account ID: ";
  std::cin.ignore();
  std::getline(std::cin, account_id);

  if (account_exists(account_id))
  {
    std::cout << "Account already exists!" << std::endl;
    return false;
  }

  std::cout << "Enter Account Holder Name: ";
  std::getline(std::cin, name);

  std::cout << "Enter Initial Balance: ";
  std::cin >> initial_balance;

  if (initial_balance < 0)
  {
    std::cout << "Initial balance cannot be negative!" << std::endl;
    return false;
  }

  // Создаем документ для нового счета
  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc = builder
                                 << "account_id" << account_id
                                 << "name" << name
                                 << "balance" << initial_balance
                                 << "transactions" << bsoncxx::builder::stream::open_array
                                 << bsoncxx::builder::stream::open_document
                                 << "type" << "initial_deposit"
                                 << "amount" << initial_balance
                                 << "timestamp" << bsoncxx::types::b_date{std::chrono::system_clock::now()}
                                 << bsoncxx::builder::stream::close_document
                                 << bsoncxx::builder::stream::close_array
                                 << bsoncxx::builder::stream::finalize;

  auto result = mongo_collection->insert_one(doc.view());

  if (result)
  {
    std::cout << "Account created successfully!" << std::endl;
    return true;
  }
  else
  {
    std::cout << "Failed to create account!" << std::endl;
    return false;
  }
}

bool deposit_money()
{
  std::string account_id;
  double amount;

  std::cout << "\n=== Deposit Money ===" << std::endl;
  std::cout << "Enter Account ID: ";
  std::cin.ignore();
  std::getline(std::cin, account_id);

  if (!account_exists(account_id))
  {
    std::cout << "Account not found!" << std::endl;
    return false;
  }

  std::cout << "Enter Deposit Amount: ";
  std::cin >> amount;

  if (amount <= 0)
  {
    std::cout << "Deposit amount must be positive!" << std::endl;
    return false;
  }

  // Обновляем баланс
  auto filter = document{} << "account_id" << account_id << finalize;
  auto update = document{}
                << "$inc" << open_document << "balance" << amount << close_document
                << "$push" << open_document
                << "transactions" << open_document
                << "type" << "deposit"
                << "amount" << amount
                << "timestamp" << bsoncxx::types::b_date{std::chrono::system_clock::now()}
                << close_document
                << close_document
                << finalize;

  auto result = mongo_collection->update_one(filter.view(), update.view());

  if (result && result->modified_count() > 0)
  {
    std::cout << "Deposit successful!" << std::endl;
    std::cout << "New Balance: " << get_account_balance(account_id) << std::endl;
    return true;
  }
  else
  {
    std::cout << "Deposit failed!" << std::endl;
    return false;
  }
}

bool withdraw_money()
{
  std::string account_id;
  double amount;

  std::cout << "\n=== Withdraw Money ===" << std::endl;
  std::cout << "Enter Account ID: ";
  std::cin.ignore();
  std::getline(std::cin, account_id);

  if (!account_exists(account_id))
  {
    std::cout << "Account not found!" << std::endl;
    return false;
  }

  std::cout << "Enter Withdrawal Amount: ";
  std::cin >> amount;

  if (amount <= 0)
  {
    std::cout << "Withdrawal amount must be positive!" << std::endl;
    return false;
  }

  double current_balance = get_account_balance(account_id);
  if (current_balance < amount)
  {
    std::cout << "Insufficient funds! Current balance: " << current_balance << std::endl;
    return false;
  }

  // Обновляем баланс
  auto filter = document{} << "account_id" << account_id << finalize;
  auto update = document{}
                << "$inc" << open_document << "balance" << -amount << close_document
                << "$push" << open_document
                << "transactions" << open_document
                << "type" << "withdrawal"
                << "amount" << amount
                << "timestamp" << bsoncxx::types::b_date{std::chrono::system_clock::now()}
                << close_document
                << close_document
                << finalize;

  auto result = mongo_collection->update_one(filter.view(), update.view());

  if (result && result->modified_count() > 0)
  {
    std::cout << "Withdrawal successful!" << std::endl;
    std::cout << "New Balance: " << get_account_balance(account_id) << std::endl;
    return true;
  }
  else
  {
    std::cout << "Withdrawal failed!" << std::endl;
    return false;
  }
}

void check_balance()
{
  std::string account_id;

  std::cout << "\n=== Check Balance ===" << std::endl;
  std::cout << "Enter Account ID: ";
  std::cin.ignore();
  std::getline(std::cin, account_id);

  if (!account_exists(account_id))
  {
    std::cout << "Account not found!" << std::endl;
    return;
  }

  double balance = get_account_balance(account_id);
  std::cout << "Current Balance: " << balance << std::endl;
}

void display_account_info()
{
  std::string account_id;

  std::cout << "\n=== Account Information ===" << std::endl;
  std::cout << "Enter Account ID: ";
  std::cin.ignore();
  std::getline(std::cin, account_id);

  auto filter = document{} << "account_id" << account_id << finalize;
  auto result = mongo_collection->find_one(filter.view());

  if (!result)
  {
    std::cout << "Account not found!" << std::endl;
    return;
  }

  auto view = result->view();
  std::cout << "\n=== Account Details ===" << std::endl;
  std::cout << "Account ID: " << view["account_id"].get_string().value << std::endl;
  std::cout << "Name: " << view["name"].get_string().value << std::endl;
  std::cout << "Balance: " << view["balance"].get_double().value << std::endl;

  // Отображаем историю транзакций
  std::cout << "\n=== Transaction History ===" << std::endl;
  auto transactions_val = view["transactions"];
  if (transactions_val)
  {
    auto transactions = transactions_val.get_array().value;
    int count = 0;

    for (auto it = transactions.begin(); it != transactions.end(); ++it)
    {
      count++;
      auto transaction_doc = (*it).get_document();
      auto transaction_view = transaction_doc.view();

      std::cout << count << ". "
                << transaction_view["type"].get_string().value
                << ": " << transaction_view["amount"].get_double().value << std::endl;
    }
  }
}

bool delete_account()
{
  std::string account_id;
  char confirm;

  std::cout << "\n=== Delete Account ===" << std::endl;
  std::cout << "Enter Account ID: ";
  std::cin.ignore();
  std::getline(std::cin, account_id);

  if (!account_exists(account_id))
  {
    std::cout << "Account not found!" << std::endl;
    return false;
  }

  std::cout << "Are you sure you want to delete account '" << account_id << "'? (y/n): ";
  std::cin >> confirm;

  if (confirm == 'y' || confirm == 'Y')
  {
    auto filter = document{} << "account_id" << account_id << finalize;
    auto result = mongo_collection->delete_one(filter.view());

    if (result && result->deleted_count() > 0)
    {
      std::cout << "Account deleted successfully!" << std::endl;
      return true;
    }
    else
    {
      std::cout << "Failed to delete account!" << std::endl;
      return false;
    }
  }
  else
  {
    std::cout << "Account deletion cancelled." << std::endl;
    return false;
  }
}

void transfer_money()
{
  std::string from_account, to_account;
  double amount;

  std::cout << "\n=== Transfer Money ===" << std::endl;
  std::cout << "Enter Source Account ID: ";
  std::cin.ignore();
  std::getline(std::cin, from_account);

  std::cout << "Enter Destination Account ID: ";
  std::getline(std::cin, to_account);

  if (!account_exists(from_account))
  {
    std::cout << "Source account not found!" << std::endl;
    return;
  }

  if (!account_exists(to_account))
  {
    std::cout << "Destination account not found!" << std::endl;
    return;
  }

  if (from_account == to_account)
  {
    std::cout << "Cannot transfer to the same account!" << std::endl;
    return;
  }

  std::cout << "Enter Transfer Amount: ";
  std::cin >> amount;

  if (amount <= 0)
  {
    std::cout << "Transfer amount must be positive!" << std::endl;
    return;
  }

  double from_balance = get_account_balance(from_account);
  if (from_balance < amount)
  {
    std::cout << "Insufficient funds! Current balance: " << from_balance << std::endl;
    return;
  }

  // Снимаем деньги с исходного счета
  auto from_filter = document{} << "account_id" << from_account << finalize;
  auto from_update = document{}
                     << "$inc" << open_document << "balance" << -amount << close_document
                     << "$push" << open_document
                     << "transactions" << open_document
                     << "type" << "transfer_out"
                     << "amount" << amount
                     << "to_account" << to_account
                     << "timestamp" << bsoncxx::types::b_date{std::chrono::system_clock::now()}
                     << close_document
                     << close_document
                     << finalize;

  // Добавляем деньги на целевой счет
  auto to_filter = document{} << "account_id" << to_account << finalize;
  auto to_update = document{}
                   << "$inc" << open_document << "balance" << amount << close_document
                   << "$push" << open_document
                   << "transactions" << open_document
                   << "type" << "transfer_in"
                   << "amount" << amount
                   << "from_account" << from_account
                   << "timestamp" << bsoncxx::types::b_date{std::chrono::system_clock::now()}
                   << close_document
                   << close_document
                   << finalize;

  // Выполняем оба обновления
  auto from_result = mongo_collection->update_one(from_filter.view(), from_update.view());
  auto to_result = mongo_collection->update_one(to_filter.view(), to_update.view());

  if (from_result && from_result->modified_count() > 0 &&
      to_result && to_result->modified_count() > 0)
  {
    std::cout << "Transfer successful!" << std::endl;
    std::cout << "New balance of " << from_account << ": "
              << get_account_balance(from_account) << std::endl;
  }
  else
  {
    std::cout << "Transfer failed!" << std::endl;
  }
}

void display_menu()
{
  std::cout << "\n==================================" << std::endl;
  std::cout << "     BANK MANAGEMENT SYSTEM       " << std::endl;
  std::cout << "==================================" << std::endl;
  std::cout << "1. Create New Account" << std::endl;
  std::cout << "2. Deposit Money" << std::endl;
  std::cout << "3. Withdraw Money" << std::endl;
  std::cout << "4. Check Balance" << std::endl;
  std::cout << "5. Display Account Information" << std::endl;
  std::cout << "6. Delete Account" << std::endl;
  std::cout << "7. Transfer Money" << std::endl;
  std::cout << "8. Exit" << std::endl;
  std::cout << "==================================" << std::endl;
  std::cout << "Enter your choice (1-8): ";
}

int main()
{
  std::cout << "Starting Bank Management System..." << std::endl;

  // Инициализируем подключение к БД
  initialize_database();

  int choice;
  bool running = true;

  while (running)
  {
    display_menu();
    std::cin >> choice;

    switch (choice)
    {
    case 1:
      create_account();
      break;

    case 2:
      deposit_money();
      break;

    case 3:
      withdraw_money();
      break;

    case 4:
      check_balance();
      break;

    case 5:
      display_account_info();
      break;

    case 6:
      delete_account();
      break;

    case 7:
      transfer_money();
      break;

    case 8:
      std::cout << "\nThank you for using our banking system!" << std::endl;
      std::cout << "Goodbye!" << std::endl;
      running = false;
      break;

    default:
      std::cout << "Invalid choice! Please enter a number between 1 and 8." << std::endl;
    }

    if (choice != 8)
    {
      std::cout << "\nPress Enter to continue...";
      std::cin.ignore();
      std::cin.get();
    }
  }

  // Очищаем ресурсы
  cleanup_database();

  return 0;
}