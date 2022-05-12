
TARGET=server
OBJS=server.o mysqldb.o common.o
LDFLAGS= -lpthread -lmysqlclient -I/usr/include/mysql/ -L/usr/lib/mysql

$(TARGET):$(OBJS)
	gcc $^ -o $@ $(LDFLAGS) 
%.c%.o:
	gcc -c $@

.PHONY:clean
clean:
	rm -rf $(OBJS) $(TARGET)
