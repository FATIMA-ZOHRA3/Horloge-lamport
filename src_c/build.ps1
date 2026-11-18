# Compile le programme. Usage (depuis src_c) :  .\build.ps1
gcc -Wall -Wextra main.c process.c scenario.c hooks.c network/tcp_protocol.c network/tcp_server.c network/tcp_client.c -o process.exe -lws2_32
if ($LASTEXITCODE -eq 0) { Write-Host "OK : process.exe" } else { Write-Host "Echec de compilation" }
# Compile le programme. Usage (depuis src_c) :  .\build.ps1
gcc -Wall -Wextra main.c process.c scenario.c hooks.c horloges.c network/tcp_protocol.c network/tcp_server.c network/tcp_client.c -o process.exe -lws2_32
if ($LASTEXITCODE -eq 0) { Write-Host "OK : process.exe" } else { Write-Host "Echec de compilation" }