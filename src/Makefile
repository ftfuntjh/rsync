LDFLAGS := -fPIE
ifeq ($(verbose),1)
	quite =
	Q = 
else
	quite = quite_
	Q = @
endif

ifeq ($(debug), 1)
	$(CFLAGS) += -g
endif

LIB_RSYNC = buffer.c server.c rsync.c
rsync: $(patsubst %.c, %.o, $(LIB_RSYNC))
	$(Q) $(CC) $^ $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@

%.o : %.c
	$(Q) $(CC) -c $(CFLAGS) $(CXXFLAGS) $< -o $@

.PHONY: clean install

clean:
	$(Q) -rm -f $(patsubst %.c, %.o, $(LIB_RSYNC)) rsync
