@Echo off

:Start

::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: 아래에 실행되는 프로그램의 실행 경로를 적어줍니다.

server3p.exe 4003

:::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: 로그 출력을 위한 경로도 지정해줍니다.

echo Program terminated at %Date% %Time% with Error %ErrorLevel% >> program.log 

echo Press Ctrl-C if you don't want to restart automatically



:: ping을 10번 보낸 후(일정시간 대기를 위함) Start 지점으로 goto 하여 프로그램을 실행합니다.

ping -n 10 localhost



goto Start