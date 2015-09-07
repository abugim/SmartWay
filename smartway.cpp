#include "smartway.h"
#include "ui_smartway.h"

SmartWay::SmartWay(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SmartWay)
{
    ui->setupUi(this);

    int_validator = new QIntValidator(this);
    ui->txtAltura->setValidator(int_validator);
    ui->txtLargura->setValidator(int_validator);
    double_validator = new QDoubleValidator(this);
    ui->txtPesoDiagonal->setValidator(double_validator);
    ui->txtPesoHorizontal->setValidator(double_validator);
    ui->txtPesoVertical->setValidator(double_validator);

    mapa_definido = false;
    partida_definida = false;
    chegada_definida = false;
    mostrar_pesos = false;
}

SmartWay::~SmartWay()
{
    delete ui;
}

void SmartWay::on_txtAltura_textEdited()
{
    if(ui->txtAltura->text().isEmpty() || ui->txtLargura->text().isEmpty()){
        ui->btnDefinirMapa->setEnabled(false);
    } else {
        ui->btnDefinirMapa->setEnabled(true);
    }
}

void SmartWay::on_txtLargura_textEdited()
{
    if(ui->txtAltura->text().isEmpty() || ui->txtLargura->text().isEmpty()){
        ui->btnDefinirMapa->setEnabled(false);
    } else {
        ui->btnDefinirMapa->setEnabled(true);
    }
}

void SmartWay::on_btnDefinirMapa_clicked()
{
    if (mapa_definido){
        mapa_definido = false;

        // Desbloquear entrada de largura e altura
        ui->txtAltura->setEnabled(true);
        ui->txtLargura->setEnabled(true);
        ui->btnDefinirMapa->setText("Definir Área");

        // Bloquear seleção de marcações
        ui->radioChegada->setEnabled(false);
        ui->radioObstaculos->setEnabled(false);
        ui->radioPartida->setEnabled(false);

        // Bloquear entrada de pesos
        ui->txtPesoDiagonal->setEnabled(false);
        ui->txtPesoHorizontal->setEnabled(false);
        ui->txtPesoVertical->setEnabled(false);

        partida_x = -1; partida_y = -1;
        partida_definida = false;

        chegada_x = -1; chegada_y = -1;
        chegada_definida = false;

        SmartWay::apagar_mapa(ui->txtAltura->text().toInt());
    } else {
        mapa_definido = true;

        // Bloquear entrada de largura e altura
        ui->txtAltura->setEnabled(false);
        ui->txtLargura->setEnabled(false);
        ui->btnDefinirMapa->setText("Redefinir Área");

        // Desbloquear seleção de marcações
        ui->radioChegada->setEnabled(true);
        ui->radioObstaculos->setEnabled(true);
        ui->radioPartida->setEnabled(true);

        // Desbloquear entrada de pesos
        ui->txtPesoDiagonal->setEnabled(true);
        ui->txtPesoHorizontal->setEnabled(true);
        ui->txtPesoVertical->setEnabled(true);

        SmartWay::criar_mapa(ui->txtAltura->text().toInt(), ui->txtLargura->text().toInt());
    }
}

void SmartWay::on_chkPeso_toggled(bool checked)
{
    mostrar_pesos = checked;
}

void SmartWay::on_txtPesoHorizontal_textEdited()
{
    ui->btnCalcular->setEnabled(pronto_calculo());
}

void SmartWay::on_txtPesoVertical_textEdited()
{
    ui->btnCalcular->setEnabled(pronto_calculo());
}

void SmartWay::on_txtPesoDiagonal_textEdited()
{
    ui->btnCalcular->setEnabled(pronto_calculo());
}

void SmartWay::on_tabelaMapa_itemClicked(QTableWidgetItem *item)
{
    QIcon icon;

    if (isPartida(item))
    {
        if (ui->radioPartida->isChecked())
        {
            mapa[item->row()][item->column()].setTipo(LIVRE);
            icon.addFile(QStringLiteral(":"), QSize(), QIcon::Normal, QIcon::Off);
            partida_definida = false;
            item->setIcon(icon);
        }
    }
    else if (isChegada(item))
    {
        if (ui->radioChegada->isChecked())
        {
            mapa[item->row()][item->column()].setTipo(LIVRE);
            icon.addFile(QStringLiteral(":"), QSize(), QIcon::Normal, QIcon::Off);
            chegada_definida = false;
            item->setIcon(icon);
        }
    }
    else if (isObstaculo(item))
    {
        if (ui->radioObstaculos->isChecked())
        {
            mapa[item->row()][item->column()].setTipo(LIVRE);
            icon.addFile(QStringLiteral(":"), QSize(), QIcon::Normal, QIcon::Off);
            item->setIcon(icon);
        }
    }
    else
    {
        if (ui->radioPartida->isChecked() && !partida_definida)
        {
            mapa[item->row()][item->column()].setTipo(PARTIDA);
            icon.addFile(QStringLiteral(":/imagens/largada_1.png"), QSize(), QIcon::Normal, QIcon::Off);
            partida_definida = true;
            item->setIcon(icon);
        }
        else if (ui->radioChegada->isChecked() && !chegada_definida)
        {
            mapa[item->row()][item->column()].setTipo(CHEGADA);
            icon.addFile(QStringLiteral(":/imagens/chegada_1.png"), QSize(), QIcon::Normal, QIcon::Off);
            chegada_definida = true;
            item->setIcon(icon);
        }
        else if (ui->radioObstaculos->isChecked())
        {
            mapa[item->row()][item->column()].setTipo(OBSTACULO);
            icon.addFile(getObstaculo(), QSize(), QIcon::Normal, QIcon::Off);
            item->setIcon(icon);
        }
    }
    ui->btnCalcular->setEnabled(pronto_calculo());
}

void SmartWay::apagar_mapa(int altura)
{
    ui->tabelaMapa->setRowCount(0);
    ui->tabelaMapa->setColumnCount(0);

    for (int i = 0; i < altura; ++i)
        delete [] mapa[i];
    delete [] mapa;

}

void SmartWay::criar_mapa(int altura, int largura)
{
    ui->tabelaMapa->setRowCount(altura);
    ui->tabelaMapa->setColumnCount(largura);
    QTableWidgetItem *item;
    for (int i = 0; i < altura; i++)
    {
        for (int j = 0; j < largura; j++)
        {
            item = new QTableWidgetItem();
            ui->tabelaMapa->setItem(i, j, item);
        }
    }

    mapa = new Celula*[altura];

    for (int i = 0; i < altura; i++)
        mapa[i] = new Celula[largura];
}

bool SmartWay::pronto_calculo()
{
    return (partida_definida
            && chegada_definida
            && mapa_definido
            && !ui->txtPesoDiagonal->text().isEmpty()
            && !ui->txtPesoHorizontal->text().isEmpty()
            && !ui->txtPesoVertical->text().isEmpty());
}

bool SmartWay::isPartida(QTableWidgetItem *item)
{
    return mapa[item->row()][item->column()].getTipo() == PARTIDA ? true : false;;
}

bool SmartWay::isChegada(QTableWidgetItem *item)
{
    return mapa[item->row()][item->column()].getTipo() == CHEGADA ? true : false;
}

bool SmartWay::isObstaculo(QTableWidgetItem *item)
{
    return mapa[item->row()][item->column()].getTipo() == OBSTACULO ? true : false;
}

QString SmartWay::getObstaculo()
{
    int i = rand() % 8;
    switch (i) {
    case 0:
        return QStringLiteral(":/imagens/obs_1.png");
        break;
    case 1:
        return QStringLiteral(":/imagens/obs_2.png");
        break;
    case 2:
        return QStringLiteral(":/imagens/obs_3.png");
        break;
    case 3:
        return QStringLiteral(":/imagens/obs_4.png");
        break;
    case 4:
        return QStringLiteral(":/imagens/obs_5.png");
        break;
    case 5:
        return QStringLiteral(":/imagens/obs_6.png");
        break;
    case 6:
        return QStringLiteral(":/imagens/obs_7.png");
        break;
    case 7:
        return QStringLiteral(":/imagens/obs_8.png");
        break;

    default:
        return QStringLiteral(":");
        break;
    }
}
