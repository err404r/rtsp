#include "rtsp_control.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

int rtsp_control_init_struct_data(RtspControlConnection* connection) {
    //@todo добавить данные для обработки или удалить функцию
    return 0;
}

int rtsp_control_connect(RtspControlConnection* connection, unsigned char* rtsp_server_string) {

    unsigned char server_addr_buf[16];
    unsigned short int server_port;
    sscanf(rtsp_server_string, "rtsp://%[^:]:%hu[^/].*", server_addr_buf, &server_port);
    printf("server addr: %s\nserver port %u\n", server_addr_buf, server_port);
    connection->rtsp_url = rtsp_server_string;

    connection->socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(connection->socket_fd < 0) {
        perror("Can't create socket:");
        return -1;
    }

    bzero(&connection->server_addr, sizeof(connection->server_addr));
    connection->server_addr.sin_family = AF_INET;
    connection->server_addr.sin_addr.s_addr=inet_addr(server_addr_buf);
    connection->server_addr.sin_port=htons(server_port);

    int retv = connect(connection->socket_fd, (struct sockaddr *)&connection->server_addr, sizeof(connection->server_addr));
    if(retv != 0) {
        perror("Can't connect to rtsp server:");
        return -1;
    }

    return 0;
}

int rtsp_control_init_media(RtspControlConnection* connection) {
    unsigned char reply_buf[RTSP_CONTROL_BUF_SIZE];
    int retv;

    connection->request_number = 0;
    bzero(connection->rtsp_headers, RTSP_CONTROL_BUF_SIZE);
    strcpy(connection->rtsp_headers, "User-Agent: LibVLC/2.2.0 (LIVE555 Streaming Media v2014.01.13)\x0D\n");
    retv = rtsp_control_run_command(connection, "OPTIONS", reply_buf, RTSP_CONTROL_BUF_SIZE);
    if(retv != 0) {
        return -10;
    }

    strcpy(connection->rtsp_headers + strlen(connection->rtsp_headers), RTSP_ACCEPT_HEADER);
    retv = rtsp_control_run_command(connection, "DESCRIBE", reply_buf, RTSP_CONTROL_BUF_SIZE);
    if(retv != 0) {
        return -20;
    }
    bzero(connection->rtsp_headers + strlen(connection->rtsp_headers) - strlen(RTSP_ACCEPT_HEADER),
          strlen(RTSP_ACCEPT_HEADER));
    //@todo честно разбирать sdp структуру
    strcpy(connection->rtsp_headers + strlen(connection->rtsp_headers), RTSP_TRANSPORT_HEADER);

    unsigned char* old_url = connection->rtsp_url;
    unsigned char setup_url[256] = {0};
    sprintf(setup_url, "%s/track1", old_url);
    connection->rtsp_url = setup_url;
    retv = rtsp_control_run_command(connection, "SETUP", reply_buf, RTSP_CONTROL_BUF_SIZE);
    if(retv != 0) {
        return -30;
    }
    unsigned char* session_buf = strstr(reply_buf, "Session:");
    if(session_buf == NULL) {
        fprintf(stderr, "Session header not found!\n");
        return -31;
    }
    strcpy(connection->rtsp_headers + strlen(connection->rtsp_headers), session_buf);
    //connection->rtsp_headers[strlen(connection->rtsp_headers) - 1] = 0; //Удаляем двойной перенос строки 0x0A
    //connection->rtsp_headers[strlen(connection->rtsp_headers) - 2] = 0; //Удаляем двойной перенос строки 0x0D
    connection->rtsp_url = old_url;

    strcpy(connection->rtsp_headers + strlen(connection->rtsp_headers), RTSP_RANGE_HEADER);
    retv = rtsp_control_run_command(connection, "PLAY", reply_buf, RTSP_CONTROL_BUF_SIZE);
    if(retv != 0) {
        return -40;
    }
    bzero(connection->rtsp_headers  + strlen(connection->rtsp_headers) - strlen(RTSP_RANGE_HEADER),
          strlen(RTSP_RANGE_HEADER));

    return 0;
}

int rtsp_control_run_command(RtspControlConnection* connection, unsigned char* command, unsigned char* server_reply, size_t reply_size) {
    static unsigned char command_buf[RTSP_CONTROL_BUF_SIZE];
    bzero(command_buf, RTSP_CONTROL_BUF_SIZE);
    sprintf(command_buf,
            "%s %s RTSP/1.0\x0D\nCSeq: %u\x0D\n%s\x0D\n",
            command,
            connection->rtsp_url,
            (unsigned int)connection->request_number+1,
            connection->rtsp_headers);
    printf("full request: {\n%s}\n", command_buf);
    int retv = sendto(connection->socket_fd, command_buf, strlen(command_buf), 0,
           (struct sockaddr *)&connection->server_addr,sizeof(connection->server_addr));
    if(retv < 0) {
        perror("Send command error:");
        return -1;
    }

    bzero(server_reply, RTSP_CONTROL_BUF_SIZE);
    retv = recvfrom(connection->socket_fd, server_reply, reply_size, 0, NULL, NULL);
    if(retv < 0) {
        perror("Recv command error:");
        return -2;
    }
    printf("full response: {\n%s}\n", server_reply);
    if(strncmp(server_reply, RTSP_CORRECT_ANSWER, strlen(RTSP_CORRECT_ANSWER)) != 0) {
        fprintf(stderr, "Wrong RTSP response\n");
        return -3;
    }
    connection->request_number++;
    return 0;
}

int rtsp_control_disconnect(RtspControlConnection* connection) {
    //rtsp_control_run_command(connection, "TEARDOWN", reply_buf, RTSP_CONTROL_BUF_SIZE);
    //@todo добавить отправку комманду завершающей rtsp соединение.
    close(connection->socket_fd);
    return 0;
}

int rtsp_control_free_struct_data(RtspControlConnection* connection) {
    //@todo добавить данные для обработки или удалить функцию
    return 0;
}
