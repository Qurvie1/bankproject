#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "factory.h"

class Director;
class Bank;

struct MyHash {
  size_t operator()(const std::pair<std::string, std::string>& val) const noexcept;
};

class ICommand {
 public:
  virtual void Execute() = 0;
  virtual void Undo() = 0;
};

class Client {
 public:
  Client(std::string name, std::string surname, std::string address, std::string series, std::string number, std::string login, std::string password, size_t id);

  std::vector<std::shared_ptr<Account>> accounts;

  void AddMoney(int amount, int acc);

  void BorrowMoney(int amount, int acc);

  std::string Name;

  std::string Surname;

  std::string Address;

  std::string pasportSeries;

  std::string pasportNumber;
  
 private:
  
  std::string Login;

  std::string Password;

  bool Dangerous;

  size_t clientid;
};

class IBuilder {
 public:
  virtual void Reset() = 0;

  virtual void SetName(std::string&) = 0;

  virtual void SetSurname(std::string&) = 0;

  virtual void SetAddress(std::string&) = 0;

  virtual void SetSeries(std::string&) = 0;

  virtual void SetNumber(std::string&) = 0;

  virtual void SetLogin(std::string&) = 0;

  virtual void SetPassword(std::string&) = 0;

  virtual void SetId(size_t id) = 0;

  virtual std::shared_ptr<Client> GetClient() const = 0;
};

class ClientBuilder : public IBuilder {
 public:
  void Reset() final {
    Name.clear();
    Surname.clear();
    Address.clear();
    Series.clear();
    Number.clear();
    Login.clear();
    Password.clear();
  }

  void SetName(std::string& name) final {
    Name = name;
  }

  void SetSurname(std::string& surname) final {
    Surname = surname;
  }

  void SetAddress(std::string& address) final {
    Address = address;
  }

  void SetSeries(std::string& series) final {
    Series = series;
  }

  void SetNumber(std::string& number) final {
    Number = number;
  }

  void SetLogin(std::string& login) final {
    Login = login;
  }

  void SetPassword(std::string& passw) final {
    Password = passw;
  }

  void SetId(size_t id) final {
    id_ = id;
  }

  std::shared_ptr<Client> GetClient() const final {
    return std::make_shared<Client>(Name, Surname, Address, Series, Number, Login, Password, id_);
  }

  std::string Name;

  std::string Surname;

  std::string Address;

  std::string Series;

  std::string Number;
 private:

  std::string Login;

  std::string Password;

  size_t id_;
};

class Director {
 public:
  void SetBuilder(std::shared_ptr<IBuilder> builder);
  std::shared_ptr<Client> createClient(std::vector<std::string>& data, size_t id);
 private:
  std::shared_ptr<IBuilder> builder_;
};

class Bank {
 public:
  Bank();
  
  bool CreateClient(std::vector<std::string>& data);

  bool Access(std::string login, std::string password) const;

  std::pair<bool, size_t> GetId(std::string login, std::string password) const;

  std::shared_ptr<Client> GetClient(size_t id) const;

  bool CreateDepositAccount(size_t client_id) const;

  bool CreateCreditAccount(size_t client_id) const;

  bool CreateDebitAccount(size_t client_id) const;

  std::shared_ptr<ICommand> TransactionClient(size_t client_id_from, size_t client_id_to, int from_id_acc, int to_id_acc, int amount) const;

  bool WithdrawMoney(size_t client_id, int amount, int account_id) const;

  bool AddMoney(size_t client_id, int amount, int account_id) const;

  size_t Clients() const noexcept;

 private:

  size_t clients;
  
  std::unique_ptr<Director> creator;

  std::unordered_map<size_t, std::shared_ptr<Client>> client_map;

  std::unordered_map<std::pair<std::string, std::string>, size_t, MyHash> authorization;

  std::shared_ptr<AccountFactory> deposit;

  std::shared_ptr<AccountFactory> credit;

  std::shared_ptr<AccountFactory> debit;
};

class Transaction : public ICommand {
 public:
  Transaction(std::shared_ptr<Client> receiver, std::shared_ptr<Client> caller, int receiver_acc, int caller_acc, int amount);

  void Execute() final {
    caller_->BorrowMoney(amount_, caller_acc_);
    receiver_->AddMoney(amount_, receiver_acc_);
  }

  void Undo() final {
    caller_->AddMoney(amount_, caller_acc_);
    receiver_->BorrowMoney(amount_, receiver_acc_);
  }

 private:
  std::shared_ptr<Client> receiver_;
  std::shared_ptr<Client> caller_;
  int receiver_acc_;
  int caller_acc_;
  int amount_;
};