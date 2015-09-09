#ifndef RTSP_CONTROL_H
#define RTSP_CONTROL_H

#include <sys/socket.h>
#include <netinet/in.h>

#define RTSP_CONTROL_BUF_SIZE 1024
#define RTSP_CORRECT_ANSWER "RTSP/1.0 200 OK"
#define RTSP_TRANSPORT_HEADER "Transport: RTP/AVP;multicast;client_port=6000-6001\x0D\n"
#define RTSP_RANGE_HEADER "Range: npt=0.000-\x0D\n"
#define RTSP_ACCEPT_HEADER "Accept: application/sdp\x0D\n"

typedef struct {
    struct sockaddr_in server_addr; //<< Структура описывающая адрес rtsp сервераs
    int socket_fd; //<< Файловый дескриптор для сокета
    unsigned char session_id[16]; //<< Идентификатор rtsp сессии
    unsigned char* rtsp_url; //<< url rtsp сервера
    unsigned short int request_number; //<< Последовательный номер rtsp запроса
    unsigned char rtsp_headers[RTSP_CONTROL_BUF_SIZE]; //<< Дополнительные заголовки к rtsp запросу
} RtspControlConnection;

/*
 * @breef Инициализация структуры описывающей rtsp подключение
 * Инициализация структуры описывающей подключение к rtsp серверу.
 * Указатель на структуру должен быть выделен заранее.
 * @param connection Структура описывающея rtsp подключение
 * @return Статус завершения операции.
 */
int rtsp_control_init_struct_data(RtspControlConnection* connection);

/*
 * @breef Выполнить подключение к rtsp серверу
 * Установить tcp соединение с rtsp сервером.
 * @param connection Структура описывающея rtsp подключение
 * @param rtsp_server_string Строка содержащая url для подключения к rtsp серверу
 * @return Статус завершения операции.
 */
int rtsp_control_connect(RtspControlConnection* connection, unsigned char* rtsp_server_string);

/*
 * @breef Выполнить последовательность комманд для инициализации медиа потока.
 * Выполнить последовательность комманд необходимую для начала видеотрансляции.
 * @param connection Структура описывающея rtsp подключение
 * @return Статус завершения операции.
 */
int rtsp_control_init_media(RtspControlConnection* connection);

/*
 * @breef Выполнить управляющую комманду rtsp
 * Выполнить управляющую комманду rtsp соединения.
 * @param connection Структура описывающея rtsp подключение
 * @return Статус завершения операции.
 */
int rtsp_control_run_command(RtspControlConnection* connection,
                             unsigned char* command,
                             unsigned char* server_reply,
                             size_t reply_size);

/*
 * @breef Отключение от rtsp сервера
 * Завершния соединения с rtsp сервером, также отправляет комманды необходимые для завершения потока.
 * @param connection Структура описывающея rtsp подключение
 * @return Статус завершения операции.
 */
int rtsp_control_disconnect(RtspControlConnection* connection);

/*
 * @breef Освобождение структуры описывающей rtsp подключение
 * Освобождение памяти занятой динамическими элементами структуры сама структура освобождается отдельно.
 * @param connection Структура описывающея rtsp подключение
 * @return Статус завершения операции.
 */
int rtsp_control_free_struct_data(RtspControlConnection* connection);

#endif // RTSP_CONTROL_H
