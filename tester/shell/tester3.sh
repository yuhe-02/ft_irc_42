#!/bin/bash
#
# shell_script_complex_new.sh
#
# 想定動作:
#  1) userX, userY, userZ (一般ユーザーPASS = "test") と AdminMaster (PASS = "admin") で並行テスト
#  2) チャンネル #room1 にトピックロック +t / 人数制限 +l を設定
#  3) チャンネル #room2 に鍵 +k を設定
#  4) 各ユーザーのJOIN / PART / PRIVMSG / MODE / QUIT / EXIT をチェック
#
# 使い方:
#   1) 別ターミナルで `valgrind ./irc_serv 6667`
#   2) ここで `./shell_script_complex_new.sh`
#   3) ログ: x.log, y.log, z.log, admin.log を要確認

HOST="127.0.0.1"
PORT="6667"

#######################################
# userX シナリオ
#######################################
(
  echo "PASS test"
  echo "NICK userX"
  echo "USER userX 0 * :X user"
  sleep 1

  # #room1 に参加
  echo "JOIN #room1"
  sleep 2

  # #room2 に参加 (鍵がまだない想定 -> 先にJOINしとく)
  echo "JOIN #room2"
  sleep 2

  # トピック変更 (operatorじゃないので失敗を期待)
  echo "TOPIC #room1 :TestTopicFromUserX"
  sleep 2

  # モード変更 (operatorじゃないので失敗)
  echo "MODE #room2 +t"
  sleep 1

  # メッセージ送信
  echo "PRIVMSG #room2 :Hello from userX in #room2!"
  sleep 2

  # PART #room2
  echo "PART #room2 :Leaving"
  sleep 2

  # QUIT
  echo "QUIT :Goodbye from userX"
) | nc "$HOST" "$PORT" > x.log &
PID_X=$!


#######################################
# userY シナリオ
#######################################
(
  echo "PASS test"
  echo "NICK userY"
  echo "USER userY 0 * :Y user"
  sleep 3

  # JOIN #room1
  echo "JOIN #room1"
  sleep 2

  # トピック変更失敗 (operatorじゃない)
  echo "TOPIC #room1 :NewTopicByUserY"
  sleep 2

  # 人数制限設定しようとして失敗
  echo "MODE #room1 +l 2"
  sleep 2

  # メッセージ
  echo "PRIVMSG #room1 :Hello from userY in #room1!"
  sleep 2

  # QUIT
  echo "QUIT :Bye from userY"
) | nc "$HOST" "$PORT" > y.log &
PID_Y=$!


#######################################
# userZ シナリオ
#######################################
(
  echo "PASS test"
  echo "NICK userZ"
  echo "USER userZ 0 * :Z user"
  sleep 4

  # #room2 に鍵がまだ設定されていないかも知れない… → 実装次第
  # 後に鍵が設定されるとJOINできないかもしれない。検証用
  echo "JOIN #room2 someWrongKey"
  sleep 2

  # もし鍵が合わず失敗なら、後から正しい鍵でJOINし直すシナリオ
  echo "JOIN #room2 secretKey"
  sleep 2

  # チャンネル制限 +l などがあるかもしれない → 入れなければエラーチェック
  # メッセージ
  echo "PRIVMSG #room2 :Hello from userZ in #room2!"
  sleep 2

  # QUIT
  echo "QUIT :Bye from userZ"
) | nc "$HOST" "$PORT" > z.log &
PID_Z=$!


#######################################
# Admin (管理者) シナリオ
#######################################
(
  echo "PASS admin"
  echo "NICK AdminMaster"
  echo "USER AdminMaster 0 * :Operator"
  sleep 2

  # #room1にJOIN
  echo "JOIN #room1"
  sleep 2

  # トピック制限 +t, 人数制限 +l 2
  echo "MODE #room1 +t"
  sleep 1
  echo "MODE #room1 +l 2"
  sleep 2

  # #room2 に鍵 +k secretKey
  echo "JOIN #room2"
  sleep 2
  echo "MODE #room2 +k secretKey"
  sleep 2

  # (オプション) userZ を operator にする → 実装あれば
  # echo "MODE #room2 +o userZ"
  # sleep 2

  # もし #room1 に既に userX, userY が居る → 2人制限の状態で 3人目来たら失敗する
  # ここでは JOIN し直すなどの動きは省略

  # 最後にサーバシャットダウン
  echo "EXIT"
) | nc "$HOST" "$PORT" > admin.log &
PID_ADMIN=$!


# プロセス終了待ち
wait $PID_X
wait $PID_Y
wait $PID_Z
wait $PID_ADMIN

echo "All clients finished. Logs: x.log, y.log, z.log, admin.log"
