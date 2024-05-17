#include "client.h"

size_t MyHash::operator()(const std::pair<std::string, std::string>& val) const noexcept {
  size_t seed = 0;
  seed ^= std::hash<std::string>()(val.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= std::hash<std::string>()(val.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed;
}

Bank::Bank() : clients(0) {
  Director director;
  std::shared_ptr<IBuilder> cbuilder = std::make_shared<ClientBuilder>();
  director.SetBuilder(cbuilder);
  creator = std::make_unique<Director>(director);
  deposit = std::make_shared<DepositFactory>();
  credit = std::make_shared<CreditFactory>();
  debit = std::make_shared<DebitFactory>();
};

bool Bank::CreateClient(std::vector<std::string>& data) {
  auto ptr = creator->createClient(data, clients);
  auto it = authorization.find({data[5], data[6]});
  if (it != authorization.end()) {
    return false;
  }
  authorization.insert({{data[5], data[6]}, clients});
  client_map[clients++] = ptr;
  return true;
}

bool Bank::Access(std::string login, std::string password) const {
  if (authorization.empty()) {
    return false;
  }
  auto it = authorization.find({login, password});
  return it != authorization.end();
}

std::pair<bool, size_t> Bank::GetId(std::string login, std::string password) const {
  auto it = authorization.find({login, password});
  if (it == authorization.end()) {
    return {false, 1};
  }
  return {true, it->second};
}

std::shared_ptr<Client> Bank::GetClient(size_t id) const {
  auto it = client_map.find(id);
  if (it == client_map.end()) {
    return nullptr;
  }
  return it->second;
}

bool Bank::CreateDepositAccount(size_t client_id) const {
  auto it = client_map.find(client_id);
  if (it == client_map.end() || it->second->accounts[1] != nullptr) {
    return false;
  }
  it->second->accounts[1] = deposit->CreateAccount(0, 100'000);
  return true;
}

bool Bank::CreateDebitAccount(size_t client_id) const {
  auto it = client_map.find(client_id);
  if (it == client_map.end() || it->second->accounts[0] != nullptr) {
    return false;
  }
  it->second->accounts[0] = debit->CreateAccount(0, 100'000);
  return true;
}

bool Bank::CreateCreditAccount(size_t client_id) const {
  auto it = client_map.find(client_id);
  if (it == client_map.end() || it->second->accounts[2] != nullptr) {
    return false;
  }
  it->second->accounts[2] = credit->CreateAccount(0, 100'000);
  return true;
}

bool Bank::WithdrawMoney(size_t client_id, int amount, int account_id) const {
  auto it = client_map.find(client_id);
  if (it == client_map.end() || it->second->accounts[account_id] == nullptr) {
    return false;
  }
  int money = it->second->accounts[account_id]->money_;
  if (money < amount) {
    return false;
  }
  it->second->accounts[account_id]->money_ = money - amount;
  return true;
}

bool Bank::AddMoney(size_t client_id, int amount, int account_id) const {
  auto it = client_map.find(client_id);
  if (it == client_map.end() || it->second->accounts[account_id] == nullptr) {
    return false;
  }
  int money = it->second->accounts[account_id]->money_;
  it->second->accounts[account_id]->money_ = money + amount;
  return true;
}

Transaction::Transaction(std::shared_ptr<Client> receiver, std::shared_ptr<Client> caller, int receiver_acc, int caller_acc, int amount) : receiver_(receiver), caller_(caller), amount_(amount), receiver_acc_(receiver_acc), caller_acc_(caller_acc) {}

std::shared_ptr<ICommand> Bank::TransactionClient(size_t id_from, size_t id_to, int from_id_acc, int to_id_acc, int amount) const {
  std::shared_ptr<Client> client_from = GetClient(id_from);
  std::shared_ptr<Client> client_to = GetClient(id_to);
  std::shared_ptr<ICommand> trans = std::make_shared<Transaction>(client_to, client_from, to_id_acc, from_id_acc, amount);
  trans->Execute();
  return trans;
}

size_t Bank::Clients() const noexcept {
  return clients;
}

void Director::SetBuilder(std::shared_ptr<IBuilder> builder) {
  builder_ = std::move(builder);
}

std::shared_ptr<Client> Director::createClient(std::vector<std::string>& data, size_t id) {
  builder_->Reset();
  builder_->SetName(data[0]);
  builder_->SetSurname(data[1]);
  builder_->SetAddress(data[2]);
  builder_->SetSeries(data[3]);
  builder_->SetNumber(data[4]);
  builder_->SetLogin(data[5]);
  builder_->SetPassword(data[6]);
  builder_->SetId(id);
  return std::move(builder_->GetClient());
}

Client::Client(std::string name, std::string surname, std::string address, std::string series, std::string number, std::string login, std::string password, size_t id) : Name(name), Surname(surname), Address(address), pasportSeries(series), pasportNumber(number), Login(login), Password(password), clientid(id){
  accounts.resize(3, nullptr);
}

void Client::AddMoney(int amount, int acc) {
  accounts[acc]->money_ += amount;
}

void Client::BorrowMoney(int amount, int acc) {
  accounts[acc]->money_ -= amount;
}