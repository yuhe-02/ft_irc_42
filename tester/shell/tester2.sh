#!/bin/bash
#
# complex_multi_client_test.sh
#
# 使い方:
#   1) 別ターミナルで IRCサーバ起動 (例: valgrind ./irc_serv 6667)
#   2) このスクリプトを実行 -> 複数の nc がポート6667へ並行接続し、
#      複雑なシナリオを自動でテストする。
#   3) ログ: userA.log, userB.log, userC.log, admin.log を確認。
#

HOST="127.0.0.1"
PORT="6667"

###############################
# UserA (一般ユーザ) シナリオ
###############################
(
  echo "PASS test"
  echo "NICK userA"
  echo "USER userA 0 * :RealNameA"
  sleep 1

  # パブリックチャンネルへ参加
  echo "JOIN #public"
  sleep 2

  # 鍵付きチャンネルがあるか試しにJOIN(まだ +k 設定前かもしれないが実装次第で失敗する)
  echo "JOIN #locked secretKey"
  sleep 2

  # 発言してみる
  echo "PRIVMSG #public :Hello from userA!"
  sleep 2

  # 終了
  echo "QUIT :bye from userA"
) | nc "$HOST" "$PORT" > userA.log &
PID_A=$!


###############################
# UserB (一般ユーザ) シナリオ
###############################
(
  echo "PASS test"
  echo "NICK userB"
  echo "USER userB 0 * :RealNameB"
  sleep 3

  echo "JOIN #public"
  sleep 2

  echo "PRIVMSG #public :Hello from userB!"
  sleep 2

  # Kick権限がないため試しにKICKしてみる
  echo "KICK #public userA :Trying to Kick"
  sleep 2

  # 自主的にQUITを待たずに何か操作(実装次第)
  # echo "JOIN #inviteOnly" など試す -> +i がついていれば失敗
  sleep 2

  echo "QUIT :bye from userB"
) | nc "$HOST" "$PORT" > userB.log &
PID_B=$!


###############################
# UserC (一般ユーザ) シナリオ
###############################
(
  echo "PASS test"
  echo "NICK userC"
  echo "USER userC 0 * :RealNameC"
  sleep 5

  # (inviteOnly の #public に参加しようとして失敗するか)
  echo "JOIN #public"
  sleep 2

  # 待機しているとAdminからINVITEがくるかも
  # (シナリオ上で Admin が 'INVITE userC #public' する)
  sleep 5

  # 発言テスト
  echo "PRIVMSG #public :Hello from userC!"
  sleep 2

  echo "PART #public :Leaving channel"
  sleep 2

  echo "QUIT :bye from userC"
) | nc "$HOST" "$PORT" > userC.log &
PID_C=$!


###############################
# Admin (管理者) シナリオ
###############################
(
  echo "PASS admin"
  echo "NICK AdminMaster"
  echo "USER AdminMaster 0 * :RealOperator"
  sleep 2

  # 参加
  echo "JOIN #public"
  sleep 2

  # Invite Onlyに
  echo "MODE #public +i"
  sleep 2

  # userC を招待
  echo "INVITE userC #public"
  sleep 3

  # 鍵付きにする
  echo "MODE #public +k secretKey"
  sleep 2

  # トピック変更
  echo "TOPIC #public :Public Channel for Testing"
  sleep 2

  # userB を kick
  echo "KICK #public userB :Cleanup Kick"
  sleep 2

  # サーバを落とす(EXIT)
  echo "EXIT"
) | nc "$HOST" "$PORT" > admin.log &
PID_ADMIN=$!


# 全ncプロセスの終了待ち
wait $PID_A
wait $PID_B
wait $PID_C
wait $PID_ADMIN

echo "All clients finished. Check userA.log, userB.log, userC.log, admin.log for server responses."
