## NICK
・引数なしだとsegv<br>

## USER
・引数なしだとsegv<br>
[USER 1 2 3 4 5]<br>
(irc):irc.example.net 461 bobo USER :Syntax error<br>
(ft_irc):localhost 001 tobaba :Welcome to the Internet Relay Network<br>

## JOIN
[JOIN test]<br>
(irc):irc.example.net 403 bob test :No such channel<br>
(ft_irc):localhost -9999 to<br>
[JOIN #]<br>
segv<br>

## QUIT
[QUIT]<br>
(irc)ERROR :Closing connection<br>
(ft_irc):localhost 451 //終わらない<br>

## INVITE
[INVITE #test a]
(irc):irc.example.net 401 to #test :No such nick or channel name<br>
(ft_irc):localhost 401 admin00 #test :No such nick/channel + segv<br>
[(チャンネル外のINVITE)ex)INVITE bob #test]
ft_ircでは一律451<br>
(irc):irc.example.net 401 to bob :No such nick or channel name<br>
(ft_irc):localhost 451<br>

## PART
[引数が3以上]
(irc):irc.example.net 461 a PART :Syntax error<br>
(ft_irc)弾いていない
<br>