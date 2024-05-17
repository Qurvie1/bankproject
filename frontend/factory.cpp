#include "factory.h"

int main() {
  std::shared_ptr<AccountFactory> deposit = std::make_shared<DepositFactory>();
  std::shared_ptr<AccountFactory> credit = std::make_shared<CreditFactory>();
  std::shared_ptr<AccountFactory> debit = std::make_shared<DebitFactory>();
  auto dep_acc = deposit->CreateAccount(100, 200);
  auto cr_acc = credit->CreateAccount(200, 300);
  auto deb_acc = debit->CreateAccount(400, 600);
  dep_acc->Print();
  cr_acc->Print();
  deb_acc->Print();
  cr_acc->SetLimit(100);
  cr_acc->Print();
  cr_acc->SetMoney(1'000'000);
  cr_acc->Print();
  return 0;
}