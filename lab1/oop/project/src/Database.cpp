#include "Database.h"

Database::Database(const std::string &uri, const std::string &db_name, const std::string &collection_name)
    : client{mongocxx::uri{uri}}, db(client[db_name]), accounts(db[collection_name])
{
  std::cout << "Connected to MongoDB successfully!" << std::endl;
}

bool Database::createAccount(const std::string &account_id, const std::string &name, double initial_balance)
{
  if (accountExists(account_id))
  {
    std::cout << "Account already exists!" << std::endl;
    return false;
  }

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

  auto result = accounts.insert_one(doc.view());
  return result ? true : false;
}

bool Database::deposit(const std::string &account_id, double amount)
{
  if (!accountExists(account_id))
  {
    std::cout << "Account not found!" << std::endl;
    return false;
  }

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

  auto result = accounts.update_one(filter.view(), update.view());
  return result->modified_count() > 0;
}

bool Database::withdraw(const std::string &account_id, double amount)
{
  if (!accountExists(account_id))
  {
    std::cout << "Account not found!" << std::endl;
    return false;
  }

  if (amount <= 0)
  {
    std::cout << "Withdrawal amount must be positive!" << std::endl;
    return false;
  }

  // Проверяем достаточно ли средств
  double current_balance = getBalance(account_id);
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

  auto result = accounts.update_one(filter.view(), update.view());
  return result->modified_count() > 0;
}

double Database::getBalance(const std::string &account_id)
{
  auto filter = document{} << "account_id" << account_id << finalize;
  auto result = accounts.find_one(filter.view());

  if (result)
  {
    auto view = result->view();
    return view["balance"].get_double().value;
  }
  return -1;
}

bool Database::accountExists(const std::string &account_id)
{
  auto filter = document{} << "account_id" << account_id << finalize;
  auto result = accounts.find_one(filter.view());
  return result ? true : false;
}

void Database::displayAccountInfo(const std::string &account_id)
{
  auto filter = document{} << "account_id" << account_id << finalize;
  auto result = accounts.find_one(filter.view());

  if (result)
  {
    auto view = result->view();
    std::cout << "\n=== Account Information ===" << std::endl;
    std::cout << "Account ID: " << view["account_id"].get_string().value << std::endl;
    std::cout << "Name: " << view["name"].get_string().value << std::endl;
    std::cout << "Balance: " << view["balance"].get_double().value << std::endl;

    // Отображаем последние 5 транзакций
    std::cout << "\nRecent Transactions:" << std::endl;

    // Правильный доступ к массиву транзакций
    if (view["transactions"] && view["transactions"].type() == bsoncxx::type::k_array)
    {
      auto transactions_array = view["transactions"].get_array().value;

      int count = 0;
      for (auto &&transaction_element : transactions_array)
      {
        if (count++ >= 5)
          break;

        if (transaction_element.type() == bsoncxx::type::k_document)
        {
          auto transaction_doc = transaction_element.get_document().value;

          // Получаем поля транзакции
          auto type_element = transaction_doc["type"];
          auto amount_element = transaction_doc["amount"];
          auto timestamp_element = transaction_doc["timestamp"];

          if (type_element && type_element.type() == bsoncxx::type::k_string &&
              amount_element && amount_element.type() == bsoncxx::type::k_double)
          {

            std::cout << "- " << type_element.get_string().value
                      << ": " << amount_element.get_double().value;

            // Добавляем timestamp, если он есть
            if (timestamp_element && timestamp_element.type() == bsoncxx::type::k_date)
            {
              auto time = timestamp_element.get_date().value;
              auto time_t = std::chrono::system_clock::to_time_t(
                  std::chrono::system_clock::time_point(time));
              std::cout << " (at " << std::ctime(&time_t) << ")";
            }
            std::cout << std::endl;
          }
        }
      }
    }
  }
  else
  {
    std::cout << "Account not found!" << std::endl;
  }
}

bool Database::deleteAccount(const std::string &account_id)
{
  auto filter = document{} << "account_id" << account_id << finalize;
  auto result = accounts.delete_one(filter.view());
  return result->deleted_count() > 0;
}