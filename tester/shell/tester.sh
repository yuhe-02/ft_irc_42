#!/bin/bash
#
# multi_client_test.sh
#
# 使い方：
#   1) 別ターミナルでサーバ起動（例: valgrind ./irc_serv 6667）
#   2) このスクリプトを実行し、サーバが複数クライアント(nc)からの入力を受け付ける様子をテストする。
#   3) 各クライアントのログが clientA.log, clientB.log, admin.log に保存される。
#

HOST="127.0.0.1"
PORT="6667"

##########################
# クライアントA (一般ユーザ)
##########################
(
  echo "PASS test"            # PASSコマンド(一般ユーザパス)
  echo "NICK userA"           # ニックネーム
  echo "USER userA 0 * :User A"
  sleep 1

  echo "JOIN #room"           # チャンネル参加
  sleep 1

  echo "PRIVMSG #room :Hello from userA!"
  sleep 2

  echo "PART #room"           # チャンネル退出
  sleep 1

  echo "QUIT :bye from A"     # サーバ切断
) | nc "$HOST" "$PORT" > clientA.log &
PID_A=$!


##########################
# クライアントB (一般ユーザ)
##########################
(
  echo "PASS test"
  echo "NICK userB"
  echo "USER userB 0 * :User B"
  sleep 2

  # 少し遅れてJOIN
  echo "JOIN #room"
  sleep 1

  echo "PRIVMSG #room :Hello from userB!"
  sleep 2

  echo "QUIT :bye from B"
) | nc "$HOST" "$PORT" > clientB.log &
PID_B=$!


################################
# 管理者ユーザ (最後にEXITする)
################################
(
  echo "PASS admin"           # adminパス => 管理者
  echo "NICK AdminMaster"
  echo "USER AdminMaster 0 * :Administrator"
  sleep 5

  # 管理者がチャンネルにJOINしてもよい
  echo "JOIN #room"
  sleep 2

  # EXITコマンドでサーバ終了
  echo "EXIT"
) | nc "$HOST" "$PORT" > admin.log &
PID_ADMIN=$!


########################
# 全クライアント終了待ち
########################
wait $PID_A
wait $PID_B
wait $PID_ADMIN

echo "All clients finished. Check clientA.log, clientB.log, admin.log."
