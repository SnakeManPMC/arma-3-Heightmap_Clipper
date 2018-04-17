#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>
#include <QTextStream>
#include <QProgressDialog>

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);
	setWindowTitle("Heightmap Clipper v0.1.2");
}

Widget::~Widget()
{
	delete ui;
}

void Widget::on_openASC_clicked()
{
	// input ASC
	QString fileName = QFileDialog::getOpenFileName(this,
	tr("ASC"), "",
	tr("ASC files (*.asc)"));
	QFile file(fileName);
	file.open(QIODevice::ReadOnly);
	QTextStream in(&file);

	// new clipped ASC
	QFile outFile(fileName + "_my_clipped.asc");
	outFile.open(QIODevice::WriteOnly);
	QTextStream out(&outFile);

	QString line;
	bool ok;
	float xllcorner,yllcorner;
	int cellsize,ncols,nrows;

	QStringList list;

/*
ncols         205
nrows         205
xllcorner     392984.672
yllcorner     3617867.273
cellsize      10
*/

	// ncols         512
	line = in.readLine();
	list = line.split(" ", QString::SkipEmptyParts);
	ncols = list[1].toInt(&ok);

	// out file
	out << line << endl;

	// nrows         512
	line = in.readLine();
	list = line.split(" ", QString::SkipEmptyParts);
	nrows = list[1].toInt(&ok);

	out << line << endl;

	// xllcorner     200000.000000
	line = in.readLine();
	list = line.split(" ", QString::SkipEmptyParts);
	xllcorner = list[1].toFloat(&ok);

	line = "xllcorner     200000";
	out << line << endl;

	// yllcorner     0.000000
	line = in.readLine();
	list = line.split(" ", QString::SkipEmptyParts);
	yllcorner = list[1].toFloat(&ok);

	line = "yllcorner     0";
	out << line << endl;

	// cellsize      10.000000
	line = in.readLine();
	list = line.split(" ", QString::SkipEmptyParts);
	cellsize = list[1].toInt(&ok);

	out << line << endl;

	// NODATA_value  -9999
	line = in.readLine();

	out << line << endl;

	ui->textEdit->append("ncols: " + QString::number(ncols) + "\nnrows: " + QString::number(nrows)
			     + "\nxllcorner: " + QString::number(xllcorner) + "\nyllcorner: " + QString::number(yllcorner) + "\ncellsize: " + QString::number(cellsize));

	ui->textEdit->append("ocean_level: " + QString::number(ui->ocean_level->value()) + "\nocean_clip: " + QString::number(ui->ocean_clip->value()));

	ui->textEdit->append("Original ASC Header: X " + QString::number(xllcorner) + ", Y " + QString::number(yllcorner));
	ui->textEdit->append("Calculator: " + QString::number((200000 - xllcorner)) + " -" + QString::number(yllcorner));

	QProgressDialog progressDialog(this);
	progressDialog.setRange(0, (ncols * nrows));
	progressDialog.setWindowTitle(tr("Heightmap Grid Points"));

	// total count for the progressdialog value
	int wholeGrid = 0;

	while (!in.atEnd())
	{
		line = in.readLine();
		list = line.split(" ");
		//ui->textEdit->append(list[0]);

		// loop all the ncols through
		for (int i=0; i<ncols; i++)
		{
			wholeGrid++;
			float fnumbah = list[i].toFloat(&ok);
			//ui->textEdit->append("list[" + QString::number(i) + "]: " + list[i] + ", fnumbah: " + QString::number(fnumbah));

			// clip below 0 meters to 0.25 meter as thats how arma3 works :)
			if (fnumbah > ui->aboveGround_low->value() && fnumbah < ui->aboveGround_high->value())
			{
				// change the elevation value to one in clipped setting
				fnumbah = ui->aboveGround_clip->value();
				//ui->textEdit->append("Yippy the clip! new fnumbah: " + QString::number(fnumbah));

				// put the modified number back to qstringlist
				list[i] = QString::number(fnumbah);
			}

			// if our elevation value matches ocean value
			if (fnumbah == ui->ocean_level->value())
			{
				// change the elevation value to one in clipped setting
				fnumbah = ui->ocean_clip->value();
				//ui->textEdit->append("Yippy the clip! new fnumbah: " + QString::number(fnumbah));

				// put the modified number back to qstringlist
				list[i] = QString::number(fnumbah).append(".00");
			}

			// write to outFile individual grid point in THIS LINE
			out << list[i] << " ";
		}

		// update progressdialog, do not update this too ofte or it will slow your program down
		progressDialog.setValue(wholeGrid);
		progressDialog.setLabelText(tr("Processing grid point %1 of %2...").arg(wholeGrid).arg(ncols * nrows));
		qApp->processEvents();

		// write to outFile end of line
		out << endl;
	}

	ui->textEdit->append("All done, exit.");
	file.close();
	outFile.close();
}
