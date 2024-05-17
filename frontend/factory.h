#include <string>
#include <iostream>
#include <memory>

class Account {
 public:
  Account(int money, int dangerous_limit) : money_(money), dangerous_limit_(dangerous_limit) {}

  Account() = default;

  virtual void Print() const = 0;

  virtual void SetPercent(double percent) = 0;

  virtual void SetCommision(double commision) = 0;

  virtual void SetTimePassed(int time) = 0;

  virtual void SetTimeToEnd(int time) = 0;

  virtual void SetLimit(int limit) = 0;

  virtual void SetPenny(double penny) = 0;

  void SetMoney(int money) {
    money_ = money;
  }

  void SetDangerousLimit(int dang_lim) {
    dangerous_limit_ = dang_lim;
  }
  
  int money_ = 0;
  int dangerous_limit_ = 100'000;
};

class CreditAccount : public Account {
 public:
  CreditAccount(int money, int dangerous_limit) : Account(money, dangerous_limit) {}

  CreditAccount(int money, int dangerous_limit, int limit, double commision) :
    Account(money, dangerous_limit),
    commision_(commision), 
    limit_(limit) {}
  
  CreditAccount() = default;

  void SetCommision(double commision) override {
    commision_ = commision;
  }

  void SetPercent(double commision) override {
    commision_ = commision;
  }

  void SetTimePassed(int) override {
    return;
  }

  void SetTimeToEnd(int) override {
    return;
  }
  
  void SetLimit(int limit) {
    limit_ = limit;
  }

  void SetPenny(double penny) {
    penny_ = penny;
  }

  void Print() const override {
    std::cout << money_ << ' ' << dangerous_limit_ << ' ' << commision_ << ' ' << limit_ << ' ' << penny_ << '\n';
  }
 private:
  double commision_ = 10.0;
  int limit_ = 15'000;
  double penny_ = 0.0;
};

class DepositAccount : public Account {
 public:
  DepositAccount(int money, int dangerous_limit) : Account(money, dangerous_limit) {}

  DepositAccount(int money, int dangerous_limit, double percent, int time) :
    Account(money, dangerous_limit),
    percent_(percent), 
    time_toend_(time) {}
  
  DepositAccount() = default;

  void SetCommision(double commision) override {
    percent_ = commision;
  }

  void SetPercent(double commision) override {
    percent_ = commision;
  }

  void SetTimePassed(int time) override {
    time_passed_ = time;
  }

  void SetTimeToEnd(int time) override {
    time_toend_ = time;
  }
  
  void SetLimit(int) {
    return;
  }

  void SetPenny(double) {
    return;
  }

  void Print() const override {
    std::cout << money_ << ' ' << dangerous_limit_ << ' ' << percent_ << ' ' << time_passed_ << ' ' << time_toend_ << '\n';
  }
 private:
  double percent_ = 10.0;
  int time_passed_ = 0;
  int time_toend_ = 365;
};

class DebitAccount : public Account {
 public:
  DebitAccount(int money, int dangerous_limit) : Account(money, dangerous_limit) {};

  DebitAccount() = default;

  void SetCommision(double) override {
    return;
  }

  void SetPercent(double) override {
    return;
  }

  void SetTimePassed(int) override {
    return;
  }

  void SetTimeToEnd(int) override {
    return;
  }
  
  void SetLimit(int) {
    return;
  }

  void SetPenny(double) {
    return;
  }

  void Print() const override {
    std::cout << money_ << ' ' << dangerous_limit_ << '\n';
  }
};

class AccountFactory {
 public:
  virtual std::shared_ptr<Account> CreateAccount(int money, int dangerous_limit) const = 0;
};

class DepositFactory : public AccountFactory {
 public:
  std::shared_ptr<Account> CreateAccount(int money, int dangerous_limit) const override {
    return std::make_shared<DepositAccount>(money, dangerous_limit);
  }
  
  DepositFactory() = default;
 private:
  int id = 1;
};

class DebitFactory : public AccountFactory {
  public:
   std::shared_ptr<Account> CreateAccount(int money, int dangerous_limit) const override {
    return std::make_shared<DebitAccount>(money, dangerous_limit);
   }
  private:
   int id = 0;
};

class CreditFactory : public AccountFactory {
 public:
  std::shared_ptr<Account> CreateAccount(int money, int dangerous_limit) const override {
    return std::make_shared<CreditAccount>(money, dangerous_limit);
  }
 private:
  int id = 2;
};