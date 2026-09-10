#include <iostream>
#include <vector>

enum class Type { deposit, withdraw };
struct transType {
  int amount;
  Type type;
};

class Account {

private:
  int balance{};
  std::vector<transType> transactions{};

public:
  explicit Account(int amount) : balance{amount} {}
  int getBalance() const { return balance; }
  bool depositAmount(int amount) {
    if (amount <= 0) {
      return false;
    }

    balance += amount;
    transactions.push_back({amount, Type::deposit});

    return true;
  }

  bool withdrawAmount(int amount) {
    if (amount > balance || amount <= 0) {
      return false;
    }

    balance -= amount;
    transactions.push_back({amount, Type::withdraw});
    return true;
  };

  const std::vector<transType> &getTransactions() const { return transactions; }
};

int main() {
  Account acc1{10000};
  std::cout << "===== ATM MENU =====" << '\n';
  std::cout << R"(
  1. Check Balance
  2. Deposit Money
  3. Withdraw Money
  4. View Transaction History
  5. Exit)" << '\n';

  while (true) {

    int command{};

    std::cout << "----------------------------------------" << '\n';
    std::cout << "Enter command: ";
    std::cin >> command;
    if (command < 1 || command > 5) {

      std::cout << "Command invalid! Bro choose valid command from 1-5."
                << '\n';
      continue;
    }

    switch (command) {
    case 1: {

      std::cout << "Your current balance: ₹" << acc1.getBalance() << '\n';
      continue;
    }
    case 2: {
      int amount{};
      std::cout << "Enter amount that you want to deposit: ";
      std::cin >> amount;
      (acc1.depositAmount(amount) ? std::cout << "deposit succesful." << "\n"
                                  : std::cout << "deposit failed." << "\n");
      continue;
    }
    case 3: {
      int amount{};
      std::cout << "Enter amount that you want to withdraw: ";
      std::cin >> amount;

      (acc1.withdrawAmount(amount) ? std::cout << "withdraw succesful." << "\n"
                                   : std::cout << "withdraw failed." << "\n");
      continue;
    }
    case 4: {

      std::cout << "Your current balance: " << acc1.getBalance() << '\n';
      std::vector transactions{acc1.getTransactions()};
      if (transactions.size()) {
        std::cout << "===== TRANSACTION HISTORY =====" << '\n';
        for (transType tran : transactions) {
          std::cout << (tran.type == Type::deposit ? "Deposited" : "Withdrew")
                    << ": Rs. " << tran.amount << '\n';
        }

      } else {
        std::cout << "bro no transactions made yet!" << '\n';
      }
      continue;
    }
    case 5:
      std::cout << "Thank you for using the ATM." << '\n';
      return 0;
    }
  }
}
