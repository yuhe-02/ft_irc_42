make -C client && mv client/irc_client ./
make -C server && mv server/irc_serv ./


# 以下で起動できる 
# 必ずサーバーから先に起動する

# window 1
# ./irc_serv 1000 test

# window 2
# ./irc_client 127.0.0.1 1000
