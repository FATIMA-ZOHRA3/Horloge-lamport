FROM gcc:latest
WORKDIR /app
COPY src_c ./src_c
COPY scenarios ./scenarios
WORKDIR /app/src_c
RUN gcc -Wall -Wextra main.c process.c scenario.c hooks.c horloges.c journal.c network/tcp_protocol.c network/tcp_server.c network/tcp_client.c -o process
COPY lancer.sh /app/lancer.sh
RUN sed -i 's/\r$//' /app/lancer.sh && chmod +x /app/lancer.sh
ENTRYPOINT ["/app/lancer.sh"]