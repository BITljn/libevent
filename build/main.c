#include <event2/event.h>
#include <event2/util.h>
#include <event2/event-config.h> // 仅在某些版本的 libevent 中需要
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

void timer_callback(evutil_socket_t fd, short event, void *arg) {
    printf("Timer event fired\n");
}

void udp_read_callback(evutil_socket_t fd, short event, void *arg) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    int recv_len = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
    if (recv_len > 0) {
        buffer[recv_len] = '\0';
        printf("Received UDP message: %s\n", buffer);
    }
}

int main() {
    struct event_base *base = event_base_new();
    if (!base) {
        fprintf(stderr, "Couldn't initialize event base.\n");
        return 1;
    }

    struct event *timer_event = evtimer_new(base, timer_callback, NULL);
    if (!timer_event) {
        fprintf(stderr, "Couldn't create timer event.\n");
        return 1;
    }

    struct timeval five_seconds = {5, 0};
    evtimer_add(timer_event, &five_seconds);

    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in udp_addr;
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_port = htons(12345);
    udp_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udp_socket, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0) {
        perror("bind");
        return 1;
    }

    struct event *udp_event = event_new(base, udp_socket, EV_READ | EV_PERSIST, udp_read_callback, NULL);
    if (!udp_event) {
        fprintf(stderr, "Couldn't create UDP event.\n");
        return 1;
    }

    event_add(udp_event, NULL);

    printf("Entering event loop...\n");
    event_base_dispatch(base);

    event_free(timer_event);
    event_free(udp_event);
    event_base_free(base);
    close(udp_socket);

    return 0;
}