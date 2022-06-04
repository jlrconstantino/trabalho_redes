# ------------------- # --- VARIÁVEIS DE AMBIENTE --- # -------------------- #

CPP = g++
RM = rm -f
CFLAGS = -Wall -Wextra
BIN = arduino
ZIP = arduino.zip
OBJ = obj/Serial.o obj/Timer.o
LIB = lib/Timer.hpp lib/Serial.hpp

# ------------------- # --- DIRETIVAS PRINCIPAIS --- # -------------------- #

# Global
all: $(BIN)

# Produção do executável
$(BIN): $(OBJ)
	$(CPP) -I lib/ $(OBJ) -o $(BIN)

# Execução convencional do programa
run:
	./$(BIN)

# Execução do programa com Valgrind
valgrind:
	valgrind -s --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(BIN)

# Compressão dos arquivos
zip: clean
	zip -r $(ZIP) Makefile lib/* src/* obj/

# Limpeza de objetos e de executável
clean:
	$(RM) $(BIN) $(ZIP) obj/*.o

# ----------------------- # --- OBJETIFICAÇÃO --- # ------------------------ #

obj/Serial.o: src/Serial.cpp lib/Serial.hpp lib/Timer.hpp
	$(CPP) -c src/Serial.cpp -o obj/Serial.o $(CFLAGS)

obj/Timer.o: src/Timer.cpp lib/Timer.hpp
	$(CPP) -c src/Timer.cpp -o obj/Timer.o $(CFLAGS)