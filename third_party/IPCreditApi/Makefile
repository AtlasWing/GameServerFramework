TARGET  := ipDemostr
SERVER_PATH :=./IPCreditAPI/IPCreditServerTaf3
TAF_PATH :=./taf-3.0.0.26
INCLUDE := -I$(TAF_PATH)/include/ -I./IPCreditAPI/include -I$(SERVER_PATH)
LIB	:= ./IPCreditAPI/lib64/libipcmapi_taf3_64.a -lpthread -L$(TAF_PATH)/lib64 -ltaf -lutil
REMOTE_OBJ += $(SERVER_PATH)/FIPCredit.64.o $(SERVER_PATH)/IPCredit.64.o

all: $(TARGET)

$(TARGET): demo.64.o
	g++ -m64  -g -fPIC -Wno-deprecated -Wall -o $@ demo.64.o $(REMOTE_OBJ) $(INCLUDE)  $(LIB)

demo.64.o: demo.cpp
	g++ -m64  -g -fPIC -Wno-deprecated -Wall $(INCLUDE) -o $@ -c demo.cpp


clean:
	rm -vf demo.64.o  ipDemostr  demo.32.o demo.64.o


