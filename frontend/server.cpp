#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <vector>
#include <sstream>
#include "client.cpp"

int main() {
  // этап 1 - создание сокета
  int serv_sock = 0;
  const short BUFF_SIZE = 1024;
  serv_sock = socket(AF_INET, SOCK_STREAM, 0);

  if (serv_sock == -1) {
    std::cout << "Error initialization socket" << std::endl;
    close(serv_sock);
    return 1;
  } else {
    std::cout << "Socket initialized!\n";
  }

  // этап 2 - привязка сокета к порту и адресу
  sockaddr_in servInfo;
  in_addr ip_to_client;
  int error = inet_pton(AF_INET, "127.0.0.1", &ip_to_client);
  if (error <= 0) {
    std::cout << "Error in IP translation to special numeric format" << std::endl;
    return 1;
  }

  servInfo.sin_family = AF_INET;
  servInfo.sin_addr = ip_to_client;
  servInfo.sin_port = htons(9084);
  error = bind(serv_sock, (sockaddr*)&servInfo, sizeof(servInfo));

  if (error != 0) {
    std::cout << "Error Socket binding to server info." << std::endl;
		close(serv_sock);
		return 1;
	} else {
    std::cout << "Binding socket to Server info is OK" << std::endl;
  }

  // этап 3 - прослушка - максимальное число пользователей, которые могут подключиться

  error = listen(serv_sock, SOMAXCONN);

  if (error != 0) {
    std::cout << "Can't start to listen to." << std::endl;
		close(serv_sock);
		return 1;
  } else {
    std::cout << "Listening to music" << std::endl;
  }

  // этап 4 - подключаем клиента по сокету

  sockaddr_in clientInfo;

  unsigned int clientinfo_size = sizeof(clientInfo);

  int ClientConn = accept(serv_sock, (sockaddr*)&clientInfo, &clientinfo_size);

  if (ClientConn == -1) {
		std::cout << "Client detected, but can't connect to a client. Error # " << std::endl;
		close(serv_sock);
		close(ClientConn);
		return 1;
  } else {
		std::cout << "Connection to a client established successfully" << std::endl;
  }

  Bank bank;

  // этап 5 - передаём данные между клиентом и сервером

  std::vector<char> server_buffer(BUFF_SIZE);
  std::vector<char> client_buffer(BUFF_SIZE);
  client_buffer[0] = '1';
  client_buffer[1] = 'R';
  client_buffer[2] = 'D';
  client_buffer[3] = 'C';
  short packet_size = 0;
  while (true) {
    packet_size = recv(ClientConn, server_buffer.data(), server_buffer.size(), 0);
    std::cout << "Client's message: " << server_buffer.data() << std::endl;

    std::string data = server_buffer.data();

    std::istringstream stream(data);

    std::string word;

    stream >> word;

    int number = std::stoi(word);

    if (number == 1) { // login
      std::string login;
      std::string password;
      stream >> login >> password;
      if (bank.Access(login, password)) {
        std::string ans = "1 ";
        auto id = bank.GetId(login, password).second;
        auto client = bank.GetClient(id);
        ans += std::to_string(id) + ' ';
        ans += client->Name + ' ';
        ans += client->Surname + ' ';
        ans += client->Address + ' ';
        ans += client->pasportSeries + ' ';
        ans += client->pasportNumber + ' ';
        ans += (client->accounts[0] == nullptr ? "-1 " : std::to_string(client->accounts[0]->money_));
        ans += (client->accounts[1] == nullptr ? "-1 " : std::to_string(client->accounts[1]->money_));
        ans += (client->accounts[2] == nullptr ? "-1 " : std::to_string(client->accounts[2]->money_));
        std::copy(ans.begin(), ans.end(), client_buffer.begin());
      } else {
        client_buffer[0] = '0';
      }
    } else if (number == 2) { // registration
      std::vector<std::string> vec(7);
      stream >> vec[0] >> vec[1] >> vec[2] >> vec[3] >> vec[4] >> vec[5] >> vec[6];
      if (bank.CreateClient(vec)) {
        client_buffer[0] = '1';
      } else {
        client_buffer[1] = '0';
      }
    } else if (number == 3) { // open account
      std::string acc;
      std::string id;
      stream >> acc >> id;
      int accid = std::stoi(acc);
      auto idc = std::stoi(id);
      if (accid == 0) {
        bank.CreateDebitAccount(idc);
      } else if (accid == 1) {
        bank.CreateDepositAccount(idc);
      } else if (accid == 2) {
        bank.CreateCreditAccount(idc);
      }
      client_buffer[0] = '1';
    } else if (number == 4) { // add money
      std::string amount;
      std::string id;
      std::string acc;
      stream >> acc >> amount >> id;
      auto racc = std::stoi(acc);
      auto cid = std::stoi(id);
      auto ramount = std::stoi(amount);
      if (bank.AddMoney(cid, ramount, racc)) {
        client_buffer[0] = '1';
      } else {
        client_buffer[0] = '0';
      }
    } else if (number == 5) { // transaction
      std::string login1;
      std::string login2;
      std::string amount;
      std::string id1;
      stream >> login1 >> login2 >> amount >> id1;
      auto ramount = std::stoi(amount);
      auto rid1 = std::stoi(id1);
      auto client = bank.GetClient(rid1);
      // auto trans = bank.TransactionClient(rid1, 0, 0, 0, ramount);
    } else if (number == 6) { // withdraw money
      std::string amount;
      std::string id;
      std::string acc;
      stream >> acc >> amount >> id;
      auto racc = std::stoi(acc);
      auto cid = std::stoi(id);
      auto ramount = std::stoi(amount);
      if (bank.WithdrawMoney(cid, ramount, racc)) {
        client_buffer[0] = '1';
      } else {
        client_buffer[0] = '0';
      }
    }

    std::cout << "Your message: ";

    packet_size = send(ClientConn, client_buffer.data(), client_buffer.size(), 0);

		if (packet_size == -1) {
			std::cout << "Can't send message to Client." << std::endl;
			close(serv_sock);
			close(ClientConn);
			continue;
		}
  }
  std::cout << "Closing connection \n";
}