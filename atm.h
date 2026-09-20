#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class Type { deposit, withdraw };

struct Transaction {
  std::int64_t accountId{};
  Type type{};
  std::int64_t amount{};
  std::int64_t balanceAfter{};
};

class Account {
private:
  std::int64_t id_{};
  std::string name_{};
  std::int64_t balance_{};

public:
  Account(std::int64_t id, std::string name, std::int64_t balance);
  std::int64_t getId() const;
  const std::string &getName() const;
  std::int64_t getBalance() const;

  bool depositAmount(std::int64_t amount);
  bool withdrawAmount(std::int64_t amount);
};

namespace store {

std::vector<Account> loadAccounts();
bool saveAccounts(const std::vector<Account> &accounts);
bool appendTransaction(std::int64_t accountId, Type type, std::int64_t amount,
                       std::int64_t balanceAfter);
std::vector<Transaction> loadHistory(std::int64_t accountId);

} // namespace store
