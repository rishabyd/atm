#include <cstdint>
#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

enum class Type { deposit, withdraw };

struct Transaction {
  std::int64_t amount;
  Type type;
};

class Account {
private:
  std::int64_t balance{};
  std::vector<Transaction> transactions{};

public:
  explicit Account(std::int64_t amount) : balance{amount} {}

  std::int64_t getBalance() const { return balance; }

  bool depositAmount(std::int64_t amount) {
    if (amount <= 0) {
      return false;
    }
    balance += amount;
    transactions.push_back(
        Transaction{.amount = amount, .type = Type::deposit});
    return true;
  }

  bool withdrawAmount(std::int64_t amount) {
    if (amount > balance || amount <= 0) {
      return false;
    }
    balance -= amount;
    transactions.push_back(
        Transaction{.amount = amount, .type = Type::withdraw});
    return true;
  }

  const std::vector<Transaction> &getTransactions() const {
    return transactions;
  }
};

void clearInputBuffer() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool promptAmount(std::string_view prompt, std::int64_t &outAmount) {
  std::cout << prompt;
  if (!(std::cin >> outAmount)) {
    std::cout << "Invalid amount!\n";
    clearInputBuffer();
    return false;
  }

  clearInputBuffer();
  return true;
}

void handleDeposit(Account &acc) {
  std::int64_t amount{};
  if (!promptAmount("Enter amount to deposit: ", amount)) {
    return;
  }

  if (acc.depositAmount(amount)) {
    std::cout << "Deposit successful.\n";
  } else {
    std::cout << "Deposit failed. Amount must be positive.\n";
  }
}

void handleWithdraw(Account &acc) {
  std::int64_t amount{};
  if (!promptAmount("Enter amount to withdraw: ", amount)) {
    return;
  }

  if (acc.withdrawAmount(amount)) {
    std::cout << "Withdrawal successful.\n";
  } else {
    std::cout << "Withdrawal failed. Insufficient funds or invalid amount.\n";
  }
}

void handleDisplayHistory(const Account &acc) {
  std::cout << "Current Balance: Rs." << acc.getBalance() << '\n';
  const auto &transactions = acc.getTransactions();

  if (transactions.empty()) {
    std::cout << "No transactions have been made yet!\n";
    return;
  }

  std::cout << "===== TRANSACTION HISTORY =====\n";
  for (const auto &tran : transactions) {
    std::cout << (tran.type == Type::deposit ? "Deposited" : "Withdrew")
              << ": Rs. " << tran.amount << '\n';
  }
}

void printMenu() {
  std::cout << "\n===== ATM MENU =====\n"
            << "1. Check Balance\n"
            << "2. Deposit Money\n"
            << "3. Withdraw Money\n"
            << "4. View Transaction History\n"
            << "5. Exit\n";
}

int main() {
  Account acc1{10000};
  printMenu();

  while (true) {
    std::cout << "----------------------------------------\n";
    std::cout << "Enter command: ";

    int command{};
    if (!(std::cin >> command)) {
      std::cout << "Invalid input type! Please enter a number.\n";
      clearInputBuffer();
      continue;
    }
    clearInputBuffer();
    switch (command) {
    case 1:
      std::cout << "Current Balance: Rs." << acc1.getBalance() << '\n';
      break;
    case 2:
      handleDeposit(acc1);
      break;
    case 3:
      handleWithdraw(acc1);
      break;
    case 4:
      handleDisplayHistory(acc1);
      break;
    case 5:
      std::cout << "Thank you for using the ATM.\n";
      return 0;
    default:
      std::cout << "Command invalid! Choose a command from 1 to 5.\n";
      break;
    }
  }
}
