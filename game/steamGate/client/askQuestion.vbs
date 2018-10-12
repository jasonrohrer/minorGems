result = InputBox( "Enter your Email address to create an account on the One Hour One Life game server." _

& vbCrLf & vbCrLf & "Note that each account needs a unique email address." _

& vbCrLf & vbCrLf & "Email:", "One Hour One Life: New account creation" )


Set objFSO = CreateObject( "Scripting.FileSystemObject" )

outFile = "answer.txt"
Set objFile = objFSO.CreateTextFile( outFile,True )
objFile.Write "" & result
objFile.Close
