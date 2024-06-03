#include <iostream>
#include <winsock2.h>
#include <cstring>
#include <thread>
#include <vector>

using namespace std;

class Server {
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN serverAddr;
    vector<thread> clientThreads;

    Server() {
        if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0) {
            cerr << "WSAStartup failed!" << endl;
            exit(1);
        }

        server = socket(AF_INET, SOCK_STREAM, 0);
        if (server == INVALID_SOCKET) {
            cerr << "Socket creation failed!" << endl;
            WSACleanup();
            exit(1);
        }

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5555);

        if (bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            cerr << "Bind failed!" << endl;
            closesocket(server);
            WSACleanup();
            exit(1);
        }

        if (listen(server, SOMAXCONN) == SOCKET_ERROR) {
            cerr << "Listen failed!" << endl;
            closesocket(server);
            WSACleanup();
            exit(1);
        }

        cout << "Listening for incoming connections." << endl;
    }

    void start() {
        while (true) {
            SOCKADDR_IN clientAddr;
            int clientAddrSize = sizeof(clientAddr);
            SOCKET clientSocket = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize);
            if (clientSocket == INVALID_SOCKET) {
                cerr << "Accept failed!" << endl;
            } else {
                cout << "Client connected!" << endl;
                clientThreads.push_back(thread(&Server::handleClient, this, clientSocket));
            }
        }
    }

    void handleClient(SOCKET clientSocket) {
        char buffer[1024];
        Matrix matrix;
        matrix.Mostrar();
        int columna;
        char charColumna;
        while (true) {
            cout << "Turno del jugador Cliente (C)." << endl;
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                charColumna = buffer[0];
                columna = charColumna - '0';
                if (matrix.Agregar('C', columna)) {
                    system("cls");
                    matrix.Mostrar();
                    if (matrix.VerificarGanador('C')) {
                        cout << "¡Jugador Cliente (C) gana!" << endl;
                        break;
                    }
                }
            } else {
                cerr << "Error al recibir datos o conexión cerrada." << endl;
                break;
            }

            cout << "Turno del jugador Servidor (S)." << endl << endl;
            cout << "Inserta la columna: ";
            cin >> charColumna;
            columna = charColumna - '0';
            if (matrix.Agregar('S', columna)) {
                system("cls");
                matrix.Mostrar();
                send(clientSocket, &charColumna, 1, 0);
                if (matrix.VerificarGanador('S')) {
                    cout << "¡Jugador Servidor (S) gana!" << endl;
                    break;
                }
            }
        }
        closesocket(clientSocket);
    }

    void CerrarSocket() {
        closesocket(server);
        WSACleanup();
        cout << "Socket cerrado, servidor desconectado." << endl;
    }
};

class Matrix {
public:
    char matriz[6][7];

    Matrix() {
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 7; ++j) {
                matriz[i][j] = ' ';
            }
        }
    }

    void Mostrar() {
        cout << endl << endl;
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 7; ++j) {
                cout << "| " << matriz[i][j] << " ";
            }
            cout << "|" << endl;
        }
        for (int j = 0; j < 7; ++j) {
            cout << "  " << j + 1 << " ";
        }
        cout << endl << endl;
    }

    bool Agregar(char elemento, int columna) {
        if (columna < 1 || columna > 7) {
            cerr << "Columna inválida!" << endl;
            return false;
        }
        columna--;  // Ajustar índice de columna a base 0
        for (int i = 5; i >= 0; --i) {
            if (matriz[i][columna] == ' ') {
                matriz[i][columna] = elemento;
                return true;
            }
        }
        cerr << "Columna llena!" << endl;
        return false;
    }

    bool VerificarGanador(char elemento) {
        // Verificar filas
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (matriz[i][j] == elemento && matriz[i][j+1] == elemento &&
                    matriz[i][j+2] == elemento && matriz[i][j+3] == elemento) {
                    return true;
                }
            }
        }
        // Verificar columnas
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 7; ++j) {
                if (matriz[i][j] == elemento && matriz[i+1][j] == elemento &&
                    matriz[i+2][j] == elemento && matriz[i+3][j] == elemento) {
                    return true;
                }
            }
        }
        // Verificar diagonales
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (matriz[i][j] == elemento && matriz[i+1][j+1] == elemento &&
                    matriz[i+2][j+2] == elemento && matriz[i+3][j+3] == elemento) {
                    return true;
                }
            }
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = 3; j < 7; ++j) {
                if (matriz[i][j] == elemento && matriz[i+1][j-1] == elemento &&
                    matriz[i+2][j-2] == elemento && matriz[i+3][j-3] == elemento) {
                    return true;
                }
            }
        }
        return false;
    }
};

int main() {
    Server servidor;
    servidor.start();
    servidor.CerrarSocket();
    return 0;
}