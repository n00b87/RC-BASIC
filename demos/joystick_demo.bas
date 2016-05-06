WindowOpen(0,"Joystick Test",50,50,640,480,0)
CanvasOpen(0,640,480,0,0,640,480,0)
LoadFont(0,"FreeMono.ttf",16)
SetColor(rgb(255,255,255))

dim button[30]
dim axis[30]

sub GetJoystick(joy_num)
	for i = 0 to NumJoyButtons(joy_num) - 1
		button[i] = joybutton(joy_num, i)
	next
	for i = 0 to NumJoyAxes(joy_num) - 1
		axis[i] = JoyAxis(joy_num, i)
	next
end sub

while true
	ClearCanvas()
	GetJoystick(0)
	for i = 0 to 11
		if button[i] then
			SetColor(rgb(255,0,0))
			DrawText("Button " + str$(i) + " was pressed", 30, i * 15)
		else
			SetColor(rgb(255,255,255))
			DrawText("Button " + str$(i) + " not pressed", 30, i * 15)
		end if
		SetColor(rgb(255,255,255))
		DrawText("Axis(" + str$(i) + ") = " + str$(axis[i]), 300, i * 15)
	next
	if key(k_escape) then
		exit while
	end if
	Update()
	Wait(5)
wend
WindowClose(0)
end

