#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")


// ポート番号
const unsigned short SERVERPORT = 8888;
// 送受信するメッセージの最大値
const unsigned int MESSAGELENGTH = 1024;

enum class Choice {
    ROCK,
    PAPER,
    SCISSORS
};

Choice getRandomChoice() {
    srand(static_cast<unsigned int>(time(nullptr)));
    return static_cast<Choice>(rand() % 3);
}

int compareChoices(Choice clientChoice, Choice serverChoice) {
    if (clientChoice == serverChoice)
        return 0; // Tie
    else if ((clientChoice == Choice::ROCK && serverChoice == Choice::SCISSORS) ||
        (clientChoice == Choice::PAPER && serverChoice == Choice::ROCK) ||
        (clientChoice == Choice::SCISSORS && serverChoice == Choice::PAPER))
        return 1; // Client wins
    else
        return -1; // Server wins
}

int main() {
    // WinSockの初期化
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // ソケットの作成
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // サーバーのアドレス情報設定
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVERPORT); // ポート番号を指定

    // ソケットにアドレスをバインド
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 接続待機
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening for incoming connections..." << std::endl;

    // クライアントの接続待機
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    // クライアントとの通信
    while (true) {
        // クライアントから選択を受信
        int clientChoice;
        int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&clientChoice), sizeof(clientChoice), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Recv failed or client disconnected." << std::endl;
            break;
        }

        // サーバーの選択をランダムに決定
        Choice serverChoice = getRandomChoice();

        // 選択結果を比較して結果を送信
        int result = compareChoices(static_cast<Choice>(clientChoice), serverChoice);
        send(clientSocket, reinterpret_cast<const char*>(&result), sizeof(result), 0);
    }

    // ソケットを閉じる
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}