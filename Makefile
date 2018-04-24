PRECONV = .tmac.tmac .titul.tr .content.tr .books.tr .appendixA.tr .appendixB.tr
IMG = $(subst .tr,.eps, $(wildcard img/*.tr))

TARGET = out.ps


$(TARGET): $(PRECONV) $(IMG) course.tr
	preconv course.tr | eqn | groff -Tps >out.ps

.%.tr: %.tr
	preconv $< >$@

.%.tmac: %.tmac
	preconv $< >$@

img/%.eps: img/%.tr
	preconv $< | groff -Tps -p >$@.ps
	ps2epsi $@.ps $@
	rm $@.ps

clean:
	rm -f $(IMG) $(PRECONV) $(TARGET)
