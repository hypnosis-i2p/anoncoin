// Copyright (c) 2013-2014 The Anoncoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Many builder specific things set in the config file, don't see a need for it here, still better to not forget to include it in your source files.
#if defined(HAVE_CONFIG_H)
#include "config/anoncoin-config.h"
#endif

#include <QObject>
#include <QDialog>

#include "i2poptionsdialog.h"
#include "ui_i2poptionsdialog.h"

//#include "optionsmodel.h"
//#include "monitoreddatamapper.h"
#include "i2pshowaddresses.h"
#include "util.h"
#include "guiutil.h"

#include "i2pwrapper.h"
#include "i2pdatafile.h"
#include "i2pmanager.h"

bool changesAreDirty = false;

I2POptionsDialog::I2POptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::I2POptionsDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->pushButtonCurrentI2PAddress,  SIGNAL(clicked()), this, SLOT(pushButtonLoadI2PSettings()));
    QObject::connect(ui->pushButtonSaveI2PSettings, SIGNAL(clicked()), this, SLOT(pushButtonSaveI2PSettings()));

    QObject::connect(ui->checkBoxAllowZeroHop         , SIGNAL(stateChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->checkBoxInboundAllowZeroHop  , SIGNAL(stateChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->checkBoxI2PEnabled           , SIGNAL(stateChanged(int))   , this, SLOT(changeI2PSettingsState()));
    QObject::connect(ui->checkBoxStaticPrivateKey     , SIGNAL(stateChanged(int))   , this, SLOT(changeI2PSettingsState()));
    QObject::connect(ui->lineEditSAMHost              , SIGNAL(textChanged(QString)), this, SLOT(settingsModified()));
    QObject::connect(ui->lineEditTunnelName           , SIGNAL(textChanged(QString)), this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxInboundBackupQuantity , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxInboundIPRestriction  , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxInboundLength         , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxInboundLengthVariance , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxInboundQuantity       , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxOutboundBackupQuantity, SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxOutboundIPRestriction , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxOutboundLength        , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxOutboundLengthVariance, SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxOutboundPriority      , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxOutboundQuantity      , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->spinBoxSAMPort               , SIGNAL(valueChanged(int))   , this, SLOT(settingsModified()));
    QObject::connect(ui->plainTextEditPrivateKey      , SIGNAL(textChanged()), this, SLOT(settingsModified()));
    QObject::connect(ui->plainTextEditPublicKey       , SIGNAL(textChanged()), this, SLOT(settingsModified()));
    QObject::connect(ui->lineEditB32Address           , SIGNAL(textChanged(QString)), this, SLOT(settingsModified()));

    QObject::connect(this, SIGNAL(accepted()), this, SLOT(onCloseDialog()));
    // QObject::connect(this, SIGNAL(rejected()), this, SLOT(onCloseDialog()));
    QObject::connect(this, SIGNAL(dialogIsClosing()), this, SLOT(onCloseDialog()));

    QObject::connect(ui->okButton,  SIGNAL(clicked()), this, SLOT(hide()));
}

I2POptionsDialog::~I2POptionsDialog()
{
    delete ui;
}

/*void I2POptionsDialog::setMapper(MonitoredDataMapper& mapper)
{
    mapper.addMapping(ui->checkBoxI2PEnabled           , OptionsModel::eI2PUseI2POnly);
    mapper.addMapping((ui->checkBoxStaticPrivateKey    , OptionsModel::eI2PStaticPrivateKey);(ui->checkBoxStaticPrivateKey
    mapper.addMapping(ui->lineEditSAMHost              , OptionsModel::eI2PSAMHost);
    mapper.addMapping(ui->spinBoxSAMPort               , OptionsModel::eI2PSAMPort);
    mapper.addMapping(ui->lineEditTunnelName           , OptionsModel::eI2PSessionName);
    mapper.addMapping(ui->spinBoxInboundQuantity       , OptionsModel::I2PInboundQuantity);
    mapper.addMapping(ui->spinBoxInboundLength         , OptionsModel::I2PInboundLength);
    mapper.addMapping(ui->spinBoxInboundLengthVariance , OptionsModel::I2PInboundLengthVariance);
    mapper.addMapping(ui->spinBoxInboundBackupQuantity  , OptionsModel::I2PInboundBackupQuantity);
    mapper.addMapping(ui->checkBoxInboundAllowZeroHop  , OptionsModel::I2PInboundAllowZeroHop);
    mapper.addMapping(ui->spinBoxInboundIPRestriction  , OptionsModel::I2PInboundIPRestriction);
    mapper.addMapping(ui->spinBoxOutboundQuantity      , OptionsModel::I2POutboundQuantity);
    mapper.addMapping(ui->spinBoxOutboundLength        , OptionsModel::I2POutboundLength);
    mapper.addMapping(ui->spinBoxOutboundLengthVariance, OptionsModel::I2POutboundLengthVariance);
    mapper.addMapping(ui->spinBoxOutboundBackupQuantity, OptionsModel::I2POutboundBackupQuantity);
    mapper.addMapping(ui->checkBoxAllowZeroHop         , OptionsModel::I2POutboundAllowZeroHop);
    mapper.addMapping(ui->spinBoxOutboundIPRestriction , OptionsModel::I2POutboundIPRestriction);
    mapper.addMapping(ui->spinBoxOutboundPriority      , OptionsModel::I2POutboundPriority);
}*/

void I2POptionsDialog::settingsModified()
{
    ui->pushButtonSaveI2PSettings->setEnabled(true);

    changesAreDirty = true;
}

void I2POptionsDialog::changeI2PSettingsState()
{
    bool isI2PEnabled   = ui->checkBoxI2PEnabled->isChecked();
    bool isStaticKey    = ui->checkBoxStaticPrivateKey->isChecked();

    ui->checkBoxStaticPrivateKey->setEnabled(isI2PEnabled);
    ui->lineEditSAMHost->setEnabled(isI2PEnabled);
    ui->spinBoxSAMPort->setEnabled(isI2PEnabled);
    ui->lineEditTunnelName->setEnabled(isI2PEnabled);
    ui->spinBoxInboundQuantity->setEnabled(isI2PEnabled);
    ui->spinBoxInboundLength->setEnabled(isI2PEnabled);
    ui->spinBoxInboundLengthVariance->setEnabled(isI2PEnabled);
    ui->spinBoxInboundBackupQuantity->setEnabled(isI2PEnabled);
    ui->checkBoxInboundAllowZeroHop->setEnabled(isI2PEnabled);
    ui->spinBoxInboundIPRestriction->setEnabled(isI2PEnabled);
    ui->spinBoxOutboundQuantity->setEnabled(isI2PEnabled);
    ui->spinBoxOutboundLength->setEnabled(isI2PEnabled);
    ui->spinBoxOutboundLengthVariance->setEnabled(isI2PEnabled);
    ui->spinBoxOutboundBackupQuantity->setEnabled(isI2PEnabled);
    ui->checkBoxAllowZeroHop->setEnabled(isI2PEnabled);
    ui->spinBoxOutboundIPRestriction->setEnabled(isI2PEnabled);
    ui->spinBoxOutboundPriority->setEnabled(isI2PEnabled);
    ui->pushButtonCurrentI2PAddress->setEnabled(true);
    ui->pushButtonSaveI2PSettings->setEnabled(true);

    ui->plainTextEditPrivateKey->setEnabled(isI2PEnabled & isStaticKey);
    ui->plainTextEditPublicKey->setEnabled(isI2PEnabled & isStaticKey);
    ui->lineEditB32Address->setEnabled(isI2PEnabled & isStaticKey);

    changesAreDirty = true;
}

void I2POptionsDialog::UpdateParameters()
{
    bool I2PEnabled             = GetBoolArg(MAP_ARGS_I2P_OPTIONS_ENABLED,
                                         pI2PManager->getFileI2PPtr()->getEnableStatus());
    bool I2PStaticKey           = GetBoolArg(MAP_ARGS_I2P_MYDESTINATION_STATIC,
                                         pI2PManager->getFileI2PPtr()->getStatic());
    std::string samHost         = GetArg(MAP_ARGS_I2P_OPTIONS_SAMHOST,
                                         pI2PManager->getFileI2PPtr()->getSamHost());
    int samPort                 = GetArg(MAP_ARGS_I2P_OPTIONS_SAMPORT,
                                         pI2PManager->getFileI2PPtr()->getSamPort());
    std::string sessionName     = GetArg(MAP_ARGS_I2P_OPTIONS_SESSION,
                                         pI2PManager->getFileI2PPtr()->getSessionName());
    int inboundQuantity         = GetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_QUANTITY,
                                         pI2PManager->getFileI2PPtr()->getInboundQuantity());
    int inboundLength           = GetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_LENGTH,
                                         pI2PManager->getFileI2PPtr()->getInboundLength());
    int inboundLengthVariance   = GetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_LENGTHVARIANCE,
                                         pI2PManager->getFileI2PPtr()->getInboundLengthVariance());
    int inboundBackupQuantity   = GetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_BACKUPQUANTITY,
                                         pI2PManager->getFileI2PPtr()->getInboundBackupQuantity());
    bool inboundAllowZeroHop    = GetBoolArg(MAP_ARGS_I2P_OPTIONS_INBOUND_ALLOWZEROHOP,
                                         pI2PManager->getFileI2PPtr()->getInboundAllowZeroHop());
    int inboundIPRestriction    = GetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_IPRESTRICTION,
                                         pI2PManager->getFileI2PPtr()->getInboundIPRestriction());
    int outboutQuantity         = GetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_QUANTITY,
                                         pI2PManager->getFileI2PPtr()->getOutboundQuantity());
    int outboutLength           = GetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_LENGTH,
                                         pI2PManager->getFileI2PPtr()->getOutboundLength());
    int outboutLengthVariance   = GetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_LENGTHVARIANCE,
                                         pI2PManager->getFileI2PPtr()->getOutboundLengthVariance());
    int outboutBackupQuantity   = GetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_BACKUPQUANTITY,
                                         pI2PManager->getFileI2PPtr()->getOutboundBackupQuantity());
    bool outboutAllowZeroHop    = GetBoolArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_ALLOWZEROHOP,
                                         pI2PManager->getFileI2PPtr()->getOutboundAllowZeroHop());
    int outboutIPRestriction    = GetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_IPRESTRICTION,
                                         pI2PManager->getFileI2PPtr()->getOutboundIPRestriction());
    int outboundPriority        = GetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_PRIORITY,
                                         pI2PManager->getFileI2PPtr()->getOutboundPriority());
    std::string publicKey       = GetArg(MAP_ARGS_I2P_MYDESTINATION_PUBLICKEY,
                                        pI2PManager->getFileI2PPtr()->getPrivateKey().substr(0, NATIVE_I2P_DESTINATION_SIZE));
    std::string privateKey      = GetArg(MAP_ARGS_I2P_MYDESTINATION_PRIVATEKEY,
                                         pI2PManager->getFileI2PPtr()->getPrivateKey());
    std::string b32destaddr     = GetArg(MAP_ARGS_I2P_MYDESTINATION_B32KEY, I2PSession::GenerateB32AddressFromDestination(publicKey));

    // I2P Enabled
    ui->checkBoxI2PEnabled->setChecked( I2PEnabled );
    // I2P Static
    ui->checkBoxStaticPrivateKey->setChecked(I2PStaticKey);
    // I2P Private Key
    ui->plainTextEditPrivateKey->document()->setPlainText(QString::fromStdString(privateKey));
    // I2P Public Key
    ui->plainTextEditPublicKey->document()->setPlainText(QString::fromStdString(publicKey));
    // I2P Base32 Destination Address
    ui->lineEditB32Address->setText(QString::fromStdString(b32destaddr));
    // SAM Host
    ui->lineEditSAMHost->setText(QString::fromStdString( samHost ));
    // SAM Port
    ui->spinBoxSAMPort->setValue(samPort);
    // Session Name
    ui->lineEditTunnelName->setText(QString::fromStdString( sessionName ));
    // Inbound Quantity
    ui->spinBoxInboundQuantity->setValue( inboundQuantity );
    // Inbound Length
    ui->spinBoxInboundLength->setValue( inboundLength );
    // Inbound Length Variance
    ui->spinBoxInboundLengthVariance->setValue( inboundLengthVariance );
    // Inbound Backup Quantity
    ui->spinBoxInboundBackupQuantity->setValue( inboundBackupQuantity );
    // Inbound Allow Zero Hop
    ui->checkBoxInboundAllowZeroHop->setChecked( inboundAllowZeroHop );
    // Inbound IP Restriction
    ui->spinBoxInboundIPRestriction->setValue( inboundIPRestriction );
    // Outbound Quantity
    ui->spinBoxOutboundQuantity->setValue( outboutQuantity );
    // Outbound Length
    ui->spinBoxOutboundLength->setValue( outboutLength );
    // Outbound Length Variance
    ui->spinBoxOutboundLengthVariance->setValue( outboutLengthVariance );
    // Outbound Backup Quantity
    ui->spinBoxOutboundBackupQuantity->setValue( outboutBackupQuantity );
    // Outbound Allow Zero Hop
    ui->checkBoxAllowZeroHop->setChecked( outboutAllowZeroHop );
    // Outbound IP Restriction
    ui->spinBoxOutboundIPRestriction->setValue( outboutIPRestriction );
    // Outbound Priority
    ui->spinBoxOutboundPriority->setValue( outboundPriority );

    // Settings have been refreshed
    onCloseDialog();
}

void I2POptionsDialog::onCloseDialog()
{
    ui->pushButtonSaveI2PSettings->setEnabled(false);
    changesAreDirty = false;
}

//void I2POptionsDialog::reject(){
void I2POptionsDialog::reject()
{
    if (changesAreDirty)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "I2P Settings",
                                                                tr("Discard unwritten changes?"),
                                                                QMessageBox::Cancel | QMessageBox::Yes,
                                                                QMessageBox::Yes);

        if (resBtn == QMessageBox::Yes)
        {
            QDialog::accept();
            UpdateParameters();
        }
        //else
        //{
        //    QDialog::reject();
        //}
    }
    else
    {
        QDialog::accept();
    }
}

void I2POptionsDialog::pushButtonLoadI2PSettings()
{
    bool I2PEnabled             = pI2PManager->getFileI2PPtr()->getEnableStatus();
    bool I2PStaticKey           = pI2PManager->getFileI2PPtr()->getStatic();
    std::string samHost         = pI2PManager->getFileI2PPtr()->getSamHost();
    int samPort                 = pI2PManager->getFileI2PPtr()->getSamPort();
    std::string sessionName     = pI2PManager->getFileI2PPtr()->getSessionName();
    int inboundQuantity         = pI2PManager->getFileI2PPtr()->getInboundQuantity();
    int inboundLength           = pI2PManager->getFileI2PPtr()->getInboundLength();
    int inboundLengthVariance   = pI2PManager->getFileI2PPtr()->getInboundLengthVariance();
    int inboundBackupQuantity   = pI2PManager->getFileI2PPtr()->getInboundBackupQuantity();
    bool inboundAllowZeroHop    = pI2PManager->getFileI2PPtr()->getInboundAllowZeroHop();
    int inboundIPRestriction    = pI2PManager->getFileI2PPtr()->getInboundIPRestriction();
    int outboutQuantity         = pI2PManager->getFileI2PPtr()->getOutboundQuantity();
    int outboutLength           = pI2PManager->getFileI2PPtr()->getOutboundLength();
    int outboutLengthVariance   = pI2PManager->getFileI2PPtr()->getOutboundLengthVariance();
    int outboutBackupQuantity   = pI2PManager->getFileI2PPtr()->getOutboundBackupQuantity();
    bool outboutAllowZeroHop    = pI2PManager->getFileI2PPtr()->getOutboundAllowZeroHop();
    int outboutIPRestriction    = pI2PManager->getFileI2PPtr()->getOutboundIPRestriction();
    int outboundPriority        = pI2PManager->getFileI2PPtr()->getOutboundPriority();
    std::string publicKey       = pI2PManager->getFileI2PPtr()->getPrivateKey().substr(0, NATIVE_I2P_DESTINATION_SIZE);
    std::string privateKey      = pI2PManager->getFileI2PPtr()->getPrivateKey();
    std::string b32destaddr     = I2PSession::GenerateB32AddressFromDestination(publicKey);

    // I2P Enabled
    ui->checkBoxI2PEnabled->setChecked( I2PEnabled );
    //Static Key
    ui->checkBoxStaticPrivateKey->setChecked( I2PStaticKey );
    // I2P Private Key
    ui->plainTextEditPrivateKey->document()->setPlainText(QString::fromStdString(privateKey));
    // I2P Public Key
    ui->plainTextEditPublicKey->document()->setPlainText(QString::fromStdString(publicKey));
    // I2P Base32 Destination Address
    ui->lineEditB32Address->setText(QString::fromStdString(b32destaddr));
    // SAM Host
    ui->lineEditSAMHost->setText(QString::fromStdString( samHost ));
    // SAM Port
    ui->spinBoxSAMPort->setValue(samPort);
    // Session Name
    ui->lineEditTunnelName->setText(QString::fromStdString( sessionName ));
    // Inbound Quantity
    ui->spinBoxInboundQuantity->setValue( inboundQuantity );
    // Inbound Length
    ui->spinBoxInboundLength->setValue( inboundLength );
    // Inbound Length Variance
    ui->spinBoxInboundLengthVariance->setValue( inboundLengthVariance );
    // Inbound Backup Quantity
    ui->spinBoxInboundBackupQuantity->setValue( inboundBackupQuantity );
    // Inbound Allow Zero Hop
    ui->checkBoxInboundAllowZeroHop->setChecked( inboundAllowZeroHop );
    // Inbound IP Restriction
    ui->spinBoxInboundIPRestriction->setValue( inboundIPRestriction );
    // Outbound Quantity
    ui->spinBoxOutboundQuantity->setValue( outboutQuantity );
    // Outbound Length
    ui->spinBoxOutboundLength->setValue( outboutLength );
    // Outbound Length Variance
    ui->spinBoxOutboundLengthVariance->setValue( outboutLengthVariance );
    // Outbound Backup Quantity
    ui->spinBoxOutboundBackupQuantity->setValue( outboutBackupQuantity );
    // Outbound Allow Zero Hop
    ui->checkBoxAllowZeroHop->setChecked( outboutAllowZeroHop );
    // Outbound IP Restriction
    ui->spinBoxOutboundIPRestriction->setValue( outboutIPRestriction );
    // Outbound Priority
    ui->spinBoxOutboundPriority->setValue( outboundPriority );

    ui->pushButtonSaveI2PSettings->setEnabled(false);

    changesAreDirty = false;
}

void I2POptionsDialog::pushButtonSaveI2PSettings()
{
    if(changesAreDirty)
    {
        WriteToMapArgs();
        WriteToDataFile();

        // Refresh values
        UpdateParameters();

        ui->pushButtonSaveI2PSettings->setEnabled(false);
        QMessageBox::information(0, QObject::tr("I2P Settings Manager"),
                              QObject::tr("Successfully wrote setings to I2P data file!") );
    }
}


bool I2POptionsDialog::WriteToDataFile()
{
    // I2P Enabled
    pI2PManager->getFileI2PPtr()->setEnableStatus( ui ->checkBoxI2PEnabled->isChecked() );
    // I2P Static Key
    pI2PManager->getFileI2PPtr()->setStatic( ui->checkBoxStaticPrivateKey->isChecked() );

    // TODO finish the saving/getting of private & public keys
    pI2PManager->getFileI2PPtr()->setPrivateKey( ui->plainTextEditPrivateKey->document()->toPlainText().toStdString() );

    // SAM Host & Port
    pI2PManager->getFileI2PPtr()->setSam( ui ->lineEditSAMHost->text().toStdString(), ui ->spinBoxSAMPort->value() );
    // Session Name
    pI2PManager->getFileI2PPtr()->setSessionName( ui ->lineEditTunnelName->text().toStdString() );
    // Inbound Quantity
    pI2PManager->getFileI2PPtr()->setInboundQuantity( ui ->spinBoxInboundQuantity->value() );
    // Inbound Length
    pI2PManager->getFileI2PPtr()->setInboundLength( ui ->spinBoxInboundLength->value() );
    // Inbound Length Variance
    pI2PManager->getFileI2PPtr()->setInboundLengthVariance( ui ->spinBoxInboundLengthVariance->value() );
    // Inbound Backup Quantity
    pI2PManager->getFileI2PPtr()->setInboundBackupQuantity( ui ->spinBoxInboundBackupQuantity->value() );
    // Inbound Allow Zero Hop
    pI2PManager->getFileI2PPtr()->setInboundAllowZeroHop( ui ->checkBoxInboundAllowZeroHop->isChecked() );
    // Inbound IP Restriction
    pI2PManager->getFileI2PPtr()->setInboundIPRestriction( ui ->spinBoxInboundIPRestriction->value() );
    // Outbound Quantity
    pI2PManager->getFileI2PPtr()->setOutboundQuantity( ui ->spinBoxOutboundQuantity->value() );
    // Outbound Length
    pI2PManager->getFileI2PPtr()->setOutboundLength( ui ->spinBoxOutboundLength->value() );
    // Outbound Length Variance
    pI2PManager->getFileI2PPtr()->setOutboundLengthVariance( ui ->spinBoxOutboundLengthVariance->value() );
    // Outbound Backup Quantity
    pI2PManager->getFileI2PPtr()->setOutboundBackupQuantity( ui ->spinBoxOutboundBackupQuantity->value() );
    // Outbound Allow Zero Hop
    pI2PManager->getFileI2PPtr()->setOutboundAllowZeroHop( ui ->checkBoxAllowZeroHop->isChecked() );
    // Outbound IP Restriction
    pI2PManager->getFileI2PPtr()->setOutboundIPRestriction( ui ->spinBoxOutboundIPRestriction->value() );
    // Outbound Priority
    pI2PManager->getFileI2PPtr()->setOutboundPriority (ui ->spinBoxOutboundPriority->value() );

    return (pI2PManager->WriteToI2PSettingsFile());
}

void I2POptionsDialog::WriteToMapArgs()
{
    bool I2PEnabled             = ui->checkBoxI2PEnabled->isChecked();
    bool I2PStaticKey           = ui->checkBoxStaticPrivateKey->isChecked();
    std::string samHost         = ui->lineEditSAMHost->text().toStdString();
    int samPort                 = ui->spinBoxSAMPort->value();
    std::string sessionName     = ui->lineEditTunnelName->text().toStdString();
    int inboundQuantity         = ui->spinBoxInboundQuantity->value();
    int inboundLength           = ui->spinBoxInboundLength->value();
    int inboundLengthVariance   = ui->spinBoxInboundLengthVariance->value();
    int inboundBackupQuantity   = ui->spinBoxInboundBackupQuantity->value();
    bool inboundAllowZeroHop    = ui->checkBoxInboundAllowZeroHop->isChecked();
    int inboundIPRestriction    = ui->spinBoxInboundIPRestriction->value();
    int outboutQuantity         = ui->spinBoxOutboundQuantity->value();
    int outboutLength           = ui->spinBoxOutboundLength->value();
    int outboutLengthVariance   = ui->spinBoxOutboundLengthVariance->value();
    int outboutBackupQuantity   = ui->spinBoxOutboundBackupQuantity->value();
    bool outboutAllowZeroHop    = ui->checkBoxAllowZeroHop->isChecked();
    int outboutIPRestriction    = ui->spinBoxOutboundIPRestriction->value();
    int outboundPriority        = ui->spinBoxOutboundPriority->value();

    std::string privateKey = ui->plainTextEditPrivateKey->document()->toPlainText().toStdString();
    std::string publicKey =  privateKey.substr(0, NATIVE_I2P_DESTINATION_SIZE);

    // I2P Private Key
    HardSetArg(MAP_ARGS_I2P_MYDESTINATION_PRIVATEKEY, privateKey);
    // I2P Public Key
    HardSetArg(MAP_ARGS_I2P_MYDESTINATION_PUBLICKEY, publicKey);
    // I2P Base32 Destination Address
    HardSetArg(MAP_ARGS_I2P_MYDESTINATION_B32KEY, I2PSession::GenerateB32AddressFromDestination(publicKey));

    HardSetBoolArg(MAP_ARGS_I2P_OPTIONS_ENABLED, I2PEnabled);
    HardSetBoolArg(MAP_ARGS_I2P_MYDESTINATION_STATIC, I2PStaticKey);
    HardSetArg(MAP_ARGS_I2P_OPTIONS_SAMHOST, samHost);
    HardSetArg(MAP_ARGS_I2P_OPTIONS_SAMPORT, NumberToString(samPort));
    HardSetArg(MAP_ARGS_I2P_OPTIONS_SESSION, sessionName);
    HardSetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_QUANTITY, NumberToString(inboundQuantity));
    HardSetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_LENGTH, NumberToString(inboundLength));
    HardSetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_LENGTHVARIANCE, NumberToString(inboundLengthVariance));
    HardSetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_BACKUPQUANTITY, NumberToString(inboundBackupQuantity));
    HardSetBoolArg(MAP_ARGS_I2P_OPTIONS_INBOUND_ALLOWZEROHOP, inboundAllowZeroHop);
    HardSetArg(MAP_ARGS_I2P_OPTIONS_INBOUND_IPRESTRICTION, NumberToString(inboundIPRestriction));
    HardSetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_QUANTITY, NumberToString(outboutQuantity));
    HardSetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_LENGTH, NumberToString(outboutLength));
    HardSetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_LENGTHVARIANCE, NumberToString(outboutLengthVariance));
    HardSetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_BACKUPQUANTITY, NumberToString(outboutBackupQuantity));
    HardSetBoolArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_ALLOWZEROHOP, outboutAllowZeroHop);
    HardSetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_IPRESTRICTION, NumberToString(outboutIPRestriction));
    HardSetArg(MAP_ARGS_I2P_OPTIONS_OUTBOUND_PRIORITY, NumberToString(outboundPriority));

}