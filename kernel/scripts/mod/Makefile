hostprogs-y	:= modpost mk_elfconfig
always		:= $(hostprogs-y) empty.o

ifeq ($(CONFIG_PROVE_LOCKING),y)
modpost-objs	:= mtmodpost.o file2alias.o sumversion.o
# dependencies on generated files need to be listed explicitly

$(obj)/mtmodpost.o $(obj)/file2alias.o $(obj)/sumversion.o: $(obj)/elfconfig.h
else
modpost-objs	:= modpost.o file2alias.o sumversion.o
# dependencies on generated files need to be listed explicitly

$(obj)/modpost.o $(obj)/file2alias.o $(obj)/sumversion.o: $(obj)/elfconfig.h
endif


quiet_cmd_elfconfig = MKELF   $@
      cmd_elfconfig = $(obj)/mk_elfconfig < $< > $@

$(obj)/elfconfig.h: $(obj)/empty.o $(obj)/mk_elfconfig FORCE
	$(call if_changed,elfconfig)

targets += elfconfig.h
