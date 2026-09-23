#include "atm.hpp"
#include <algorithm>
#include <cstdint>
#include <format>
#include <iostream>
#include <istream>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <vector>

void clearInputBuffer() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool promptAccountCred(std::int64_t &id, std::int64_t &pin) {
  std::cout << "Enter account number: ";
  if (!(std::cin >> id)) {
    std::cout << "inavalid number\n";
    clearInputBuffer();
    return false;
  }
  std::cout << "Enter your account PIN: ";
  if (!(std::cin >> pin)) {
    std::cout << "inavalid number\n";
    clearInputBuffer();
    return false;
  }
  clearInputBuffer();
  return true;
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
  if (!acc.depositAmount(amount)) {
    std::cout << "Deposit failed. Amount must be positive.\n";
    return;
  }
  std::vector<AccountWithPin> accounts{store::loadAccounts()};
  auto it = std::ranges::find_if(accounts, [&acc](const AccountWithPin &item) {
    return item.id == acc.getId();
  });
  if (it != accounts.end()) {
    it->balance += amount;
    std::cout << "Account " << it->id << " balance updated.\n";
  }
  if (!store::saveAccounts(accounts)) {
    std::cout << "Deposit occurred, but the account file could not be saved.\n";
    return;
  }
  if (!store::appendTransaction(acc.getId(), Type::deposit, amount,
                                acc.getBalance())) {
    std::cout
        << "Deposit succeeded, but transaction history could not be saved.\n";
    return;
  }
  std::cout << "Deposit successful.\n";
}

void handleWithdraw(Account &acc) {
  std::int64_t amount{};
  if (!promptAmount("Enter amount to withdraw: ", amount)) {
    return;
  }
  if (!acc.withdrawAmount(amount)) {
    std::cout << "Withdrawal failed. Insufficient funds or invalid amount.\n";
    return;
  }
  std::vector<AccountWithPin> accounts{store::loadAccounts()};
  auto it = std::ranges::find_if(accounts, [&acc](const AccountWithPin &item) {
    return item.id == acc.getId();
  });
  if (it != accounts.end()) {
    it->balance -= amount;
    std::cout << "Account " << it->id << " balance updated.\n";
  }
  if (!store::saveAccounts(accounts)) {
    std::cout
        << "Withdrawal occurred, but the account file could not be saved.\n";
    return;
  }
  if (!store::appendTransaction(acc.getId(), Type::withdraw, amount,
                                acc.getBalance())) {
    std::cout << "Withdrawal succeeded, but transaction history could not be "
                 "saved.\n";
    return;
  }
  std::cout << "Withdrawal success.\n";
}

void handleDisplayHistory(const Account &acc1) {
  std::cout << std::format("Current Balance: Rs.{}\n\n", acc1.getBalance());
  const auto transactions{store::loadHistory(acc1.getId())};
  if (transactions.empty()) {
    std::cout << "No transactions have been made yet!\n";
    return;
  }
  std::cout << "===== TRANSACTION HISTORY =====\n";
  for (const auto &tran : transactions) {
    std::cout << std::format(
        "{}: Rs.{}  | Balance after: {}\n",
        (tran.type == Type::deposit ? "Deposited" : "Withdrew"), tran.amount,
        tran.balanceAfter);
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

std::optional<Account> selectAccount(std::vector<AccountWithPin> &accounts) {
  while (true) {
    std::int64_t accountId{};
    std::int64_t pin{};
    if (!promptAccountCred(accountId, pin)) {
      return std::nullopt;
    }
    auto it{std::ranges::find_if(
        accounts, [&accountId, &pin](const AccountWithPin &account) {
          return account.id == accountId && account.pin == pin;
        })};
    if (it != accounts.end()) {
      return Account{it->id, it->name, it->balance};
    }
    std::cout << "Account not found. Try again.\n";
    ;
  }
}

int getRandomNumber(int min, int max) {
  static std::random_device rd;
  static std::mt19937 gen{rd()};
  std::uniform_int_distribution<int> distrib{min, max};
  return distrib(gen);
}

std::optional<Account> getVerifiedAccount() {
  std::vector<AccountWithPin> accounts{store::loadAccounts()};
  return selectAccount(accounts);
}

// register account
#include <cstdint>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

void registerAccount() {
  std::cout << "\n=== Account Registration ===\nEnter full name: ";

  std::string name;
  if (!std::getline(std::cin >> std::ws, name)) {
    std::cin.clear();
    clearInputBuffer();
    std::cout << "Error: Invalid input encountered.\n";
    return;
  }
  if (name.empty()) {
    std::cout << "Error: Name cannot be empty.\n";
    return;
  }
  const std::int64_t randomId{getRandomNumber(1, 100000)};
  const std::int64_t randomPin{getRandomNumber(1001, 9999)};

  if (!store::appendAccounts(randomId, name, defaultBalance, randomPin)) {
    std::cout << "Error: Failed to write account to database.\n";
    return;
  }

  std::cout << std::format(
      "Account created successfully.\nAccount ID: {} | PIN: {}\n", randomId,
      randomPin);
}

int main() {
  while (true) {

    std::cout << "\n1- Login\n2- Register\nEnter command: ";
    int chooseCommand{};
    std::cin >> chooseCommand;

    if (chooseCommand == 1) {
      std::optional<Account> optAccount{getVerifiedAccount()};
      if (!optAccount) {
        break;
      }
      Account acc1{*optAccount};
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
    } else if (chooseCommand == 2) {
      registerAccount();
    } else {
      std::cout << "Invalid command! please enter 1 or 2.";
      clearInputBuffer();
      continue;
    }
  }
}
