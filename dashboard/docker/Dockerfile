FROM gcc:13
RUN apt-get update && apt-get install -y --no-install-recommends make \
    && rm -rf /var/lib/apt/lists/*
COPY run_clocks.sh /usr/local/bin/run_clocks.sh
# Supprime les fins de ligne Windows (CRLF) qui cassent les scripts .sh
RUN sed -i 's/\r$//' /usr/local/bin/run_clocks.sh && chmod +x /usr/local/bin/run_clocks.sh
WORKDIR /build
ENTRYPOINT ["/usr/local/bin/run_clocks.sh"]