#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "client.cpp"
#include <string>
#include <vector>
#include <type_traits>
#include <utility>


TEST_CASE("CreateClient", "[Bank]") {
  Bank bank;
  std::vector<std::string> vector;
  vector.emplace_back("Ivan");
  vector.emplace_back("Kalinin");
  vector.emplace_back("Nauchnyi 8");
  vector.emplace_back("228");
  vector.emplace_back("1488");
  vector.emplace_back("Kalinin");
  vector.emplace_back("Kalinin228");
  REQUIRE(bank.CreateClient(vector));
  REQUIRE(bank.Access(vector[5], vector[6]));
}


TEST_CASE("AmoutOfClient", "[Bank]") {
  Bank bank;
  std::vector<std::string> vector;
  vector.emplace_back("Ivan");
  vector.emplace_back("Kalinin");
  vector.emplace_back("Nauchnyi 8");
  vector.emplace_back("228");
  vector.emplace_back("1488");
  vector.emplace_back("Kalinin");
  vector.emplace_back("Kalinin228");
  bank.CreateClient(vector);
  REQUIRE(bank.Clients() == 1);
  std::vector<std::string> vectorr;
  vectorr.emplace_back("Ivan");
  vectorr.emplace_back("Kalinin");
  vectorr.emplace_back("Nauchnyi 8");
  vectorr.emplace_back("228");
  vectorr.emplace_back("1488");
  vectorr.emplace_back("Kalinin");
  vectorr.emplace_back("Kalinin228");
  bank.CreateClient(vectorr);
  REQUIRE(bank.Clients() == 1);
  std::vector<std::string> vector1;
  vector1.emplace_back("Ivan");
  vector1.emplace_back("Kalenin");
  vector1.emplace_back("Nauchnyi 8");
  vector1.emplace_back("228");
  vector1.emplace_back("1488");
  vector1.emplace_back("Kalinin");
  vector1.emplace_back("Kalenin228");
  bank.CreateClient(vector1);
  REQUIRE(bank.Clients() == 2);
}


TEST_CASE("CreditAcc", "[Bank]") {
  Bank bank;
  std::vector<std::string> vector;
  vector.emplace_back("Ivan");
  vector.emplace_back("Kalinin");
  vector.emplace_back("Nauchnyi 8");
  vector.emplace_back("228");
  vector.emplace_back("1488");
  vector.emplace_back("Kalinin");
  vector.emplace_back("Kalinin228");
  bank.CreateClient(vector);
  std::vector<std::string> vector1;
  vector1.emplace_back("Ivan");
  vector1.emplace_back("Kalenin");
  vector1.emplace_back("Nauchnyi 8");
  vector1.emplace_back("228");
  vector1.emplace_back("1488");
  vector1.emplace_back("Kalenin");
  vector1.emplace_back("Kalenin1228");
  bank.CreateClient(vector1);
  bank.CreateDebitAccount(0);
  bank.CreateDebitAccount(1);
  bank.AddMoney(0, 5000, 0);
  bank.AddMoney(1, 6000, 0);
  bank.GetClient(0)->accounts[0];
  REQUIRE(bank.GetClient(0)->accounts[0]->money_ == 5000);
  REQUIRE(bank.GetClient(1)->accounts[0]->money_ == 6000);
}
