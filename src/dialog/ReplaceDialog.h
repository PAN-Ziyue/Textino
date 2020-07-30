//*  Filename: ReplaceDialog.h
//*  Created Date: 2020-07-25 15:34:39
//*  Modified Date: 2020-07-28 11:05:46
//*  Description:
//*     Header files for replace function.
//*     A Class ReplaceDialog, the subclass of FindDialog

#ifndef _REPLACEDIALOG_H
#define _REPLACEDIALOG_H

#include "FindDialog.h"
#include "../ui/LineEdit.h"

class ReplaceDialog : public FindDialog {
    
    Q_OBJECT

protected:
    QLabel *replace_label;
    LineEdit *replace_edit;
    QPushButton *replace_btn;
    QPushButton *replace_all_btn;

    void InitControl();
    void ConnectSlot();

protected slots:
    void ReplaceClicked();
    void ReplaceAllClicked();

public:
    ReplaceDialog(QWidget* parent = nullptr, QsciScintilla* pText = nullptr);
};

#endif // _REPLACEDIALOG_H
