mkdir testCan
xcopy /s /q .\test\* .\testCan\
copy .\Debug\FIOU.exe .\testCan\
cd .\testCan\
.\FIOU.exe > report.txt