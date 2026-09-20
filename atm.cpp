#include "atm.h"
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
    getline(stream, field, '|');
  }
  return Account{std::stoll(fields[0]), fields[1], std::stoll(fields[2])};
}

Transaction parseToTransaction(const std::string &line) {
  std::istringstream stream{line};
  std::array<std::string, 4> fields{};
  for (std::string &field : fields) {
    getline(stream, field, '|');
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
std::vector<Account> store::loadAccounts() {
  std::ifstream file{accountsFile};
  if (!file) {
    std::vector<Account> accounts{};
    accounts.emplace_back(1, "Alice", 10000);
    accounts.emplace_back(2, "Bob", 5000);
    accounts.emplace_back(3, "Charlie", 2500);
    accounts.emplace_back(4, "Dhana", 800);
    saveAccounts(accounts);
    return accounts;
  }

  std::vector<Account> accounts{};
  std::string line{};
  while (std::getline(file, line)) {
    accounts.emplace_back(parseToAccount(line));
  }
  return accounts;
};

bool store::saveAccounts(const std::vector<Account> &accounts) {
  std::ofstream file{accountsFile};
  if (!file) {
    std::cerr << "error whole file rewriting";
    return false;
  }

  for (const Account &account : accounts) {
    file << std::format("{}|{}|{}", account.getId(), account.getName(),
                        account.getBalance())
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
