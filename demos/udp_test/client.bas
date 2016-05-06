'UDP Client
quit = 0
dim s$
udp_socketopen(0, 0)
Print "Type quit to end"
while s$ <> "quit"
	print "Fill the buffer\n"
	s$ = input(">")
	udp_senddata(0, s, "127.0.0.1", 2000)
wend

udp_socketclose(0)

System("pause")
