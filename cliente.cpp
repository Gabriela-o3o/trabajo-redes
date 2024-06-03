#include <iostream>
#include <winsock2.h>
#include <cstring>

using namespace std;

// se crea clase tablero para el juego
class Tablero {
public:
    char matriz[6][7];

    Tablero() {
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
        //valida si se puede agregar
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

// cliente para la conexion
class Client {
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN serverAddr;
    char buffer[1024];

    Client(const char* serverIp, int port) {
        if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0) {
            cerr << "fallo la inicializacion" << endl;
            exit(1);
        }
            // crea el socket
        server = socket(AF_INET, SOCK_STREAM, 0);
        if (server == INVALID_SOCKET) {
            cerr << "fallo creacion del socket" << endl;
            WSACleanup();
            exit(1);
        }

        serverAddr.sin_addr.s_addr = inet_addr(serverAddress);
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverport);

        if (connect(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            cerr << "fallo la conexion" << endl;
            closesocket(server);
            WSACleanup();
            exit(1);
        }

        cout << "conectado al servidor" << endl;
    }

    ~Client() {
        CerrarSocket();
    }

    void Enviar(char mensaje) {
        buffer[0] = mensaje;
        buffer[1] = '\0';  
        if (send(server, buffer, 1, 0) == SOCKET_ERROR) {
            cerr << "Error al mandar el mensaje" << endl;
        } else {
            cout << "Mensaje enviado" << endl;
        }
        memset(buffer, 0, sizeof(buffer));
    }

    string Recibir() {
        int bytesReceived = recv(server, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';  
            cout << "servidor dice " << buffer << endl;
            string buf(buffer);
            memset(buffer, 0, sizeof(buffer));
            return buf;
        } else {
            cerr << "Error al recibir la informacio "<< endl;
            return "";
        }
    }

    void CerrarSocket() {
        closesocket(server);
        WSACleanup();
        cout << "Socket cerrado " << endl;
    }
};

int main() {
    Client cliente("127.0.0.1", 5555);  
    Tablero matrix;
    matrix.Mostrar();
    int columna;
    char charColumna;
    while (true) {
        cout << "Turno del jugador Cliente (X)." << endl;
        cout << "Inserta la columna: ";
        cin >> charColumna;
        columna = charColumna - '0';
        if (matrix.Agregar('X', columna)) {
            cliente.Enviar(charColumna);
            system("cls");
            matrix.Mostrar();
            if (matrix.VerificarGanador('X')) {
                cout << "¡Jugador Cliente (X) gana!" << endl;
                break;
            }
        }

        cout << "Turno del jugador Servidor (O)." << endl;
        charColumna = cliente.Recibir()[0];
        columna = charColumna - '0';
        if (matrix.Agregar('O', columna)) {
            system("cls");
            matrix.Mostrar();
            if (matrix.VerificarGanador('O')) {
                cout << "¡Jugador Servidor (O) gana!" << endl;
                break;
            }
        }
    }
    cliente.CerrarSocket();
    return 0;
}
