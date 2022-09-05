mkdir testCan
xcopy /s /q .\test\* .\testCan\
copy .\Debug\FIOU.exe .\testCan\
.\testCan\FIOU.exe > output.txt