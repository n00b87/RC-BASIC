WindowOpen(0,"Joystick Test",50,50,640,480,0)
CanvasOpen(0,640,480,0,0,640,480,0)
LoadFont(0,"FreeMono.ttf",16)
SetColor(rgb(255,255,255))

dim button[30]
dim axis[30]
dim hat$[30]

sub GetJoystick(joy_num)
	for i = 0 to 11
		button[i] = 0
		axis[i] = 0
		hat$[i] = ""
	next
	if NumJoyHats(joy_num) > 0 then
		for i = 0 to NumJoyHats(joy_num)-1
			Select Case JoyHat(joy_num, i)
				Case HAT_LEFT
					hat$[i] = "LEFT"
				Case HAT_RIGHT
					hat$[i] = "RIGHT"
				Case HAT_UP
					hat$[i] = "UP"
				Case HAT_DOWN
					hat$[i] = "DOWN"
				Case HAT_LEFTUP
					hat$[i] = "LEFTUP"
				Case HAT_LEFTDOWN
					hat$[i] = "LEFTDOWN"
				Case HAT_RIGHTUP
					hat$[i] = "RIGHTUP"
				Case HAT_RIGHTDOWN
					hat$[i] = "RIGHTDOWN"
				Case HAT_CENTERED
					hat$[i] = "CENTERED"
			End Select
		next
	End If
	
	If NumJoyButtons(joy_num) > 0 then
		for i = 0 to NumJoyButtons(joy_num) - 1
			button[i] = joybutton(joy_num, i)
		next
	End If
	
	If NumJoyAxes(joy_num) > 0 then
		for i = 0 to NumJoyAxes(joy_num) - 1
			axis[i] = JoyAxis(joy_num, i)
		next
	End If
end sub

while true
	ClearCanvas()
	GetJoystick(0)
	for i = 0 to 11
		if button[i] then
			SetColor(rgb(255,0,0))
			DrawText("Button " + str$(i) + " was pressed", 10, i * 15)
		else
			SetColor(rgb(255,255,255))
			DrawText("Button " + str$(i) + " not pressed", 10, i * 15)
		end if
		SetColor(rgb(255,255,255))
		DrawText("Axis(" + str$(i) + ") = " + str$(axis[i]), 280, i * 15)
		DrawText("Hat(" + str$(i) + ") = " + hat$[i], 450, i * 15)
	next
	GetJoystick(1)
	for i = 0 to 11
		if button[i] then
			SetColor(rgb(255,0,0))
			DrawText("Button " + str$(i) + " was pressed", 10, i * 15 + 240)
		else
			SetColor(rgb(255,255,255))
			DrawText("Button " + str$(i) + " not pressed", 10, i * 15+240)
		end if
		SetColor(rgb(255,255,255))
		DrawText("Axis(" + str$(i) + ") = " + str$(axis[i]), 280, i * 15+240)
		DrawText("Hat(" + str$(i) + ") = " + hat$[i], 450, i * 15+240)
	next
	if key(k_escape) then
		exit while
	end if
	Update()
	Wait(5)
wend
WindowClose(0)
end

