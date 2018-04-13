.PHONY: preconv img

all: preconv img
	preconv course.tr | groff -Tps -e >out.ps

preconv:
	preconv tmac.tmac >.tmac.tmac
	preconv titul.tr >.titul.tr
	preconv content.tr >.content.tr
	preconv books.tr >.books.tr

img:
	preconv img/mc1.tr | groff -Tps -p >img/mc1.ps
	ps2epsi img/mc1.ps img/mc1.eps
	rm img/mc1.ps

clean:
	rm -f .tmac.tmac .*.tr out.ps
	rm img/mc1.eps
