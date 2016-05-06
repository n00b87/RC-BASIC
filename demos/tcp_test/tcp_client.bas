'Client

TCP_SocketOpen(1, "127.0.0.1", 1234)

txt$ = ""

TCP_GetData(1, txt$, 15)

print "Server Sent: " + txt$

TCP_SocketClose(1)

System("pause")

end





