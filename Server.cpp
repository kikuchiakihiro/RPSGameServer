#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")


// �|�[�g�ԍ�
const unsigned short SERVERPORT = 8888;
// ����M���郁�b�Z�[�W�̍ő�l
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
    // WinSock�̏�����
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // �\�P�b�g�̍쐬
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // �T�[�o�[�̃A�h���X���ݒ�
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVERPORT); // �|�[�g�ԍ����w��

    // �\�P�b�g�ɃA�h���X���o�C���h
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // �ڑ��ҋ@
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening for incoming connections..." << std::endl;

    // �N���C�A���g�̐ڑ��ҋ@
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    // �N���C�A���g�Ƃ̒ʐM
    while (true) {
        // �N���C�A���g����I������M
        int clientChoice;
        int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&clientChoice), sizeof(clientChoice), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Recv failed or client disconnected." << std::endl;
            break;
        }

        // �T�[�o�[�̑I���������_���Ɍ���
        Choice serverChoice = getRandomChoice();

        // �I�����ʂ��r���Č��ʂ𑗐M
        int result = compareChoices(static_cast<Choice>(clientChoice), serverChoice);
        send(clientSocket, reinterpret_cast<const char*>(&result), sizeof(result), 0);
    }

    // �\�P�b�g�����
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}