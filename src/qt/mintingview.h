// Copyright (c) 2012-2019 The Donu developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef DONU_QT_MINTINGVIEW_H
#define DONU_QT_MINTINGVIEW_H

#include <QWidget>
#include <QComboBox>
#include <qt/mintingfilterproxy.h>

class WalletModel;


QT_BEGIN_NAMESPACE
class QTableView;
class QMenu;
QT_END_NAMESPACE

class MintingView : public QWidget
{
    Q_OBJECT
public:
    explicit MintingView(QWidget *parent = 0);
    void setModel(WalletModel *model);

    enum MintingEnum
    {
        Minting10min,
        Minting1day,
        Minting30days,
        Minting90days
    };

private:
    WalletModel *model;
    QTableView *mintingView;
    QComboBox *mintingCombo;
    MintingFilterProxy *mintingProxyModel;
    QMenu *contextMenu;

private Q_SLOTS:
    void contextualMenu(const QPoint &);
    void copyAddress();
    void copyTransactionId();

Q_SIGNALS:

public Q_SLOTS:
    void exportClicked();
    void chooseMintingInterval(int idx);
};

#endif // DONU_QT_MINTINGVIEW_H
