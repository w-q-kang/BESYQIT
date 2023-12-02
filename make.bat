if not exist "bin\" mkdir bin
if not exist "bin\Debug\" mkdir bin\Debug
if not exist "obj\" mkdir obj
if not exist "obj\src\" mkdir obj\src
if not exist "obj\Debug\" mkdir obj\Debug
if not exist "obj\Debug\src\" mkdir obj\Debug\src

g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c main.cpp -o obj\Debug\main.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Basics.cpp -o obj\Debug\src\Basics.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Compliance.cpp -o obj\Debug\src\Compliance.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Datetime.cpp -o obj\Debug\src\Datetime.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\globals.cpp -o obj\Debug\src\globals.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Interpreter.cpp -o obj\Debug\src\Interpreter.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Item.cpp -o obj\Debug\src\Item.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Juubes.cpp -o obj\Debug\src\Juubes.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Lexer.cpp -o obj\Debug\src\Lexer.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Log.cpp -o obj\Debug\src\Log.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Opcode.cpp -o obj\Debug\src\Opcode.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Paragraph.cpp -o obj\Debug\src\Paragraph.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Parser.cpp -o obj\Debug\src\Parser.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Preprocess.cpp -o obj\Debug\src\Preprocess.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Rowitem.cpp -o obj\Debug\src\Rowitem.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Second.cpp -o obj\Debug\src\Second.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Sentence.cpp -o obj\Debug\src\Sentence.o
g++.exe -Wall -fexceptions -g -std=c++17 -Og -Iinclude -c src\Third.cpp -o obj\Debug\src\Third.o
g++.exe  -o bin\Debug\BESYQIT.exe obj\Debug\main.o obj\Debug\src\Basics.o obj\Debug\src\Compliance.o obj\Debug\src\Datetime.o obj\Debug\src\globals.o obj\Debug\src\Interpreter.o obj\Debug\src\Item.o obj\Debug\src\Juubes.o obj\Debug\src\Lexer.o obj\Debug\src\Log.o obj\Debug\src\Opcode.o obj\Debug\src\Paragraph.o obj\Debug\src\Parser.o obj\Debug\src\Preprocess.o obj\Debug\src\Rowitem.o obj\Debug\src\Second.o obj\Debug\src\Sentence.o obj\Debug\src\Third.o  -Wl,--stack,700000000 -Wl,--heap,500000000  
