'UDP Server

udp_socketopen(0, 2000)

print "waiting for quit"
udata$ = ""

host$ = ""

port = 0


while true
	gotSomething = udp_getdata(0,udata$,host$,port)
	if udata$ = "quit" then
		exit while
	end if

	if length(udata$) > 0 then
		print "Data: " + udata$()
	end if
wend

udp_socketclose(0)

end


