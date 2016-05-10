WindowOpen(0, "Test 0", WINDOWPOS_CENTERED, WINDOWPOS_CENTERED, 640, 480, 0)
WindowOpen(1, "Test 1", 20, 20, 640, 480, 0)

While true
	If WindowHasInputFocus(0) Then
		If Key(K_R) Then
			print "You typed R"
		ElseIf Key(K_ESCAPE) Then
			Exit While
		End If
	ElseIf WindowHasInputFocus(1) Then
		If Key(K_R) Then
			SetColor(RGB(255,0,0))
			Prints("You typed R")
		End If
	End If
Wend

WindowClose(1)
WindowClose(0)
End
