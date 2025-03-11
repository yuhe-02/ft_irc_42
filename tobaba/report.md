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
