.PHONY: preconv

all: preconv
	preconv course.tr | groff -Tps >out.ps

preconv:
	preconv tmac.tmac >.tmac.tmac
	preconv titul.tr >.titul.tr
	preconv content.tr >.content.tr
	preconv books.tr >.books.tr

clean:
	rm .tmac.tmac .*.tr out.ps
