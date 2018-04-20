PRECONV = .tmac.tmac .titul.tr .content.tr .books.tr .appendixA.tr .appendixB.tr
IMG = img/mc1.eps img/matrix.eps

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
