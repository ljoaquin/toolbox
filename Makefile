
SRC=a.cpp
OUTPUT=run

CFLAG = -lstdc++

$(OUTPUT): $(SRC)
	gcc $^ -o $@ $(CFLAG)

clean:
	rm $(OUTPUT)
