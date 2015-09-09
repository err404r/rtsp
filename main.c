#include <stdio.h>
#include "rtsp_control.h"

#define USAGE_STR "usage: %s <rtsp link>\n"

int main(int argc, unsigned char* argv[]) {
    if(argc != 2) {
        printf(USAGE_STR, argv[0]);
        return 1;
    }
    while(1) {
        sleep(15);

        RtspControlConnection con;
        int resv;

        resv = rtsp_control_init_struct_data(&con);
        if(resv < 0) {
            continue;
        }

        resv = rtsp_control_connect(&con, argv[1]);
        if(resv < 0) {
            continue;
        }

        resv = rtsp_control_init_media(&con);
        if(resv < 0) {
            continue;
        }

        unsigned char recv_buf[RTSP_CONTROL_BUF_SIZE];
        while(1) {
            resv = rtsp_control_run_command(&con, "GET_PARAMETER", recv_buf, RTSP_CONTROL_BUF_SIZE);
            if(resv < 0) {
                break;
            }
            sleep(60);
        }
    }

    return 0;
}

