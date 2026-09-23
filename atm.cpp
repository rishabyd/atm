#include "atm.hpp"
#include <array>
#include <cstdint>
#include <format>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// constants
constexpr auto accountsFile{"accounts.txt"};
constexpr auto transactionsFile{"transactions.txt"};

// parsers
Account parseToAccount(const std::string &line) {
  std::istringstream stream{line};
  std::array<std::string, 3> fields{};
  for (std::string &field : fields) {
    std::getline(stream, field, '|');
  }
  return Account{std::stoll(fields[0]), fields[1], std::stoll(fields[2])};
}

AccountWithPin parseToAccountWithPin(const std::string &line) {
  std::istringstream stream{line};
  std::array<std::string, 4> fields{};
  for (std::string &field : fields) {
    std::getline(stream, field, '|');
  }
  return {std::stoll(fields[0]), fields[1], std::stoll(fields[2]),
          std::stoll(fields[3])};
}

Transaction parseToTransaction(const std::string &line) {
  std::istringstream stream{line};
  std::array<std::string, 4> fields{};
  for (std::string &field : fields) {
    std::getline(stream, field, '|');
  }
  return Transaction{std::stoll(fields[0]),
                     fields[1] == "d" ? Type::deposit : Type::withdraw,
                     std::stoll(fields[2]), std::stoll(fields[3])};
}

// constuctor
Account::Account(std::int64_t id, std::string name, std::int64_t balance)
    : id_{id}, name_{std::move(name)}, balance_{balance} {};

// member functions
bool Account::depositAmount(std::int64_t amount) {
  if (amount <= 0) {
    return false;
  }
  balance_ += amount;
  return true;
}

bool Account::withdrawAmount(std::int64_t amount) {
  if (amount > balance_ || amount <= 0) {
    return false;
  }
  balance_ -= amount;
  return true;
}

// getters
std::int64_t Account::getId() const { return id_; };

const std::string &Account::getName() const { return name_; };

std::int64_t Account::getBalance() const { return balance_; };

// store file handlers
std::vector<AccountWithPin> store::loadAccounts() {
  std::ifstream file{accountsFile};
  std::vector<AccountWithPin> accounts{};
  std::string line{};
  while (std::getline(file, line)) {
    accounts.emplace_back(parseToAccountWithPin(line));
  }
  return accounts;
};

bool store::saveAccounts(const std::vector<AccountWithPin> &accounts) {
  std::ofstream file{accountsFile};
  if (!file) {
    std::cerr << "error whole file rewriting";
    return false;
  }

  for (const AccountWithPin &account : accounts) {
    file << std::format("{}|{}|{}|{}", account.id, account.name,
                        account.balance, account.pin)
         << '\n';
  }
  return true;
};

bool store::appendTransaction(std::int64_t accountId, Type type,
                              std::int64_t amount, std::int64_t balanceAfter) {
  std::ofstream file{transactionsFile, std::ios::app};
  if (!file) {
    std::cerr << "Error: cannot write transactions.txt\n";
    return false;
  }

  file << std::format("{}|{}|{}|{}", accountId,
                      (type == Type::deposit ? 'd' : 'w'), amount, balanceAfter)
       << "\n";
  return true;
};

std::vector<Transaction> store::loadHistory(std::int64_t accountId) {
  std::ifstream file{transactionsFile};
  std::vector<Transaction> transactions{};
  if (!file) {
    return transactions;
  }
  std::string line{};
  while (std::getline(file, line)) {
    const Transaction transaction{parseToTransaction(line)};

    if (transaction.accountId == accountId) {
      transactions.push_back(parseToTransaction(line));
    }
  }
  return transactions;
}

bool store::appendAccounts(std::int64_t id, std::string name,
                           std::int64_t balance, std::int64_t pin) {
  std::ofstream file{accountsFile, std::ios::app};
  if (!file) {
    std::cerr << "Error: cannot append accounts.txt\n";
    return false;
  }
  file << std::format("{}|{}|{}|{}\n", id, name, balance, pin);
  return true;
}
