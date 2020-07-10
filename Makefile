SRC = src

.PHONY: all

all :
	@$(MAKE) -C $(SRC)

clean :
	@$(MAKE) -C $(SRC) clean
