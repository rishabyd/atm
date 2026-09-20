#include "atm.h"
#include <cstdint>
#include <format>
#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

void clearInputBuffer() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool promptId(std::int64_t &id) {
  if (!(std::cin >> id)) {
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

void handleDeposit(Account &acc, const std::vector<Account> &accounts) {
  std::int64_t amount{};
  if (!promptAmount("Enter amount to deposit: ", amount)) {
    return;
  }
  if (!acc.depositAmount(amount)) {
    std::cout << "Deposit failed. Amount must be positive.\n";
    return;
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

void handleWithdraw(Account &acc, const std::vector<Account> &accounts) {
  std::int64_t amount{};
  if (!promptAmount("Enter amount to withdraw: ", amount)) {
    return;
  }

  if (!acc.withdrawAmount(amount)) {
    std::cout << "Withdrawal failed. Insufficient funds or invalid amount.\n";
    return;
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

void printAccounts(const std::vector<Account> &accs) {
  std::cout << "\n===== ACCOUNTS =====\n" << '\n';
  for (const Account &acc : accs) {
    std::cout << std::format("{} | {} | {}\n", acc.getId(), acc.getName(),
                             acc.getBalance());
  }
}

Account &selectAccount(std::vector<Account> &accounts) {
  while (true) {
    std::int64_t accountId{};

    if (!promptId(accountId)) {
      continue;
    }

    for (Account &account : accounts) {
      if (account.getId() == accountId) {
        return account;
      }
    }

    std::cout << "Invalid account id. Try again.\n";
  }
}

int main() {
  std::vector<Account> accounts{store::loadAccounts()};
  printAccounts(accounts);
  Account &acc1{selectAccount(accounts)};
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
      handleDeposit(acc1, accounts);
      break;
    case 3:
      handleWithdraw(acc1, accounts);
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
