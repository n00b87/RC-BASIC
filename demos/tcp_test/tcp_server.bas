'Server

TCP_SocketOpen(1,"",1234)
dim client
txt$ = "HELLO CLIENT\n\n"
client = false
while true

	client = TCP_AcceptSocket(1,2)
	if client then
		TCP_SendData(2, txt$)
		TCP_SocketClose(2)
		exit while
	end if
	
wend

TCP_SocketClose(1)

print "The End"
System("pause")
end




