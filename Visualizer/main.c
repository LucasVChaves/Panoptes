// build: gcc main.c -o espcam -lraylib -lm -lpthread -ldl -lX11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>         // read, close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <raylib.h>

#define SERVER_IP "192.168.0.37"
#define SERVER_PORT 80
#define BUFFER_SIZE 4096

pthread_mutex_t frameMutex;
unsigned char *frameBuffer = NULL;
int frameSize = 0;
bool newFrameAvailable = false;
volatile bool exitFlag = false;  // sinal para sair da thread

int recv_all(int sock, unsigned char *buf, int len) {
    int total = 0;
    while(total < len) {
        int n = recv(sock, buf + total, len - total, 0);
        if(n <= 0) {
            return n;
        }
        total += n;
    }
    return total;
}

int recv_line(int sock, char *buf, int maxLen) {
    int i = 0;
    char c;
    int n;
    while(i < maxLen - 1) {
        n = recv(sock, &c, 1, 0);
        if(n <= 0) break;
        buf[i++] = c;
        if(c == '\n') break;
    }
    buf[i] = '\0';
    return i;
}

void *streamThread(void *arg) {
    int sock;
    struct sockaddr_in server;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        printf("Erro ao criar socket.\n");
        return NULL;
    }
    
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    
    if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Erro na conexão.\n");
        close(sock);
        return NULL;
    }
    
    char *httpRequest = "GET / HTTP/1.1\r\nHost: " SERVER_IP "\r\n\r\n";
    if(send(sock, httpRequest, strlen(httpRequest), 0) < 0) {
        printf("Erro ao enviar requisição HTTP.\n");
        close(sock);
        return NULL;
    }
    
    char headerBuf[BUFFER_SIZE];
    while(1) {
        char line[512];
        int len = recv_line(sock, line, sizeof(line));
        if(len <= 0) break;
        if(strcmp(line, "\r\n") == 0) break;
    }
    
    while(!exitFlag) {
        char boundaryLine[100];
        int blen = recv_line(sock, boundaryLine, sizeof(boundaryLine));
        if(blen <= 0) break;
        
        int contentLength = 0;
        while(1) {
            char line[512];
            int len = recv_line(sock, line, sizeof(line));
            if(len <= 0) break;
            if(strcmp(line, "\r\n") == 0) break;
            if(strncmp(line, "Content-Length:", 15) == 0) {
                contentLength = atoi(line + 15);
            }
        }
        
        if(contentLength <= 0) continue;
        
        unsigned char *jpegData = (unsigned char *)malloc(contentLength);
        if(!jpegData) break;
        
        int received = recv_all(sock, jpegData, contentLength);
        if(received != contentLength) {
            free(jpegData);
            break;
        }
        
        pthread_mutex_lock(&frameMutex);
        if(frameBuffer) {
            free(frameBuffer);
        }
        frameBuffer = jpegData;
        frameSize = contentLength;
        newFrameAvailable = true;
        pthread_mutex_unlock(&frameMutex);
    }
    
    close(sock);
    return NULL;
}

int main(void) {
    const int screenWidth = 640;
    const int screenHeight = 480;
    InitWindow(screenWidth, screenHeight, "Visualizador MJPEG");
    
    pthread_mutex_init(&frameMutex, NULL);
    
    pthread_t thread;
    if(pthread_create(&thread, NULL, streamThread, NULL)) {
        printf("Erro ao criar thread.\n");
        return 1;
    }
    
    Texture2D videoTexture = {0};
    bool textureLoaded = false;
    
    SetTargetFPS(30);
    
    while(!WindowShouldClose()) {
        pthread_mutex_lock(&frameMutex);
        if(newFrameAvailable && frameBuffer != NULL) {
            unsigned char *jpegCopy = frameBuffer;
            int jpegSizeCopy = frameSize;
            newFrameAvailable = false;
            frameBuffer = NULL;
            pthread_mutex_unlock(&frameMutex);
            
            Image img = LoadImageFromMemory(".jpg", jpegCopy, jpegSizeCopy);
            if(img.data) {
                Texture2D newTexture = LoadTextureFromImage(img);
                UnloadImage(img);
                
                if(textureLoaded) {
                    UnloadTexture(videoTexture);
                }
                videoTexture = newTexture;
                textureLoaded = true;
            }
            free(jpegCopy);
        } else {
            pthread_mutex_unlock(&frameMutex);
        }
        
        BeginDrawing();
            ClearBackground(BLACK);
            if(textureLoaded) {
                DrawTextureEx(videoTexture,
                              (Vector2){ (screenWidth - videoTexture.width * 2) / 2, (screenHeight - videoTexture.height * 2) / 2 },
                              0.0f, 2.0f, WHITE);
            } else {
                DrawText("Aguardando stream...", 10, 10, 20, RAYWHITE);
            }
        EndDrawing();
    }
    
    exitFlag = true;
    pthread_join(thread, NULL);
    
    if(textureLoaded) UnloadTexture(videoTexture);
    pthread_mutex_destroy(&frameMutex);
    CloseWindow();
    
    return 0;
}
