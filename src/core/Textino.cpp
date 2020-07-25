/*
 * Created Date: Friday, June 5th 2020, 3:34:39 pm
 * Author: Raymond Rhino
 *
 * Copyright (c) 2020 Textino
 */
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>
#include <QLabel>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerverilog.h>
#include <Qsci/qscilexersql.h>

#include "Textino.h"

Textino::Textino()
{
    config = new ConfigManager();

    CreateMainEditor();
    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();
}

Textino::~Textino() {

}


void Textino::CreateMainEditor() {
    main_editor = new QsciScintilla();

    setCentralWidget(main_editor);  // qscintilla editor as the central part

    connect(main_editor, SIGNAL(textChanged()), this, SLOT(Modified()));    // modified slot

    main_editor->setMarginType(0, QsciScintilla::NumberMargin);             // left margin content
    main_editor->setMarginsFont(QFont(config->GetDefaultFontFamily(),
                                      config->GetDefaultFontSize()));
    main_editor->setMarginWidth(0, 50);                                     // left margin width


    main_editor->SendScintilla(QsciScintilla::SCI_SETCODEPAGE, QsciScintilla::SC_CP_UTF8);
    main_editor->setIndentationGuides(QsciScintilla::SC_IV_LOOKBOTH);
    main_editor->setCaretLineVisible(true);                 // line number visible
    QColor line_color = QColor(Qt::green).lighter(195);     // line highlighting color
    main_editor->setCaretLineBackgroundColor(line_color);
    main_editor->setWrapMode(QsciScintilla::WrapWord);      // set content wrap

    setWindowIcon(QIcon(":/imgs/icon.png"));                // main icon
    resize(1280, 720);                                      // main size

    QFont font(config->GetFontFamily(), config->GetFontSize());
    main_editor->setFont(font);
    SetCurrentFile("");
}

void Textino::CreateActions()
{
    new_act = new QAction(QIcon(":/imgs/new.png"), tr("&New"), this);
    new_act->setShortcut(tr("Ctrl+N"));
    new_act->setStatusTip(tr("Create a new file"));
    connect(new_act, SIGNAL(triggered()), this, SLOT(NewFile()));

    open_act = new QAction(QIcon(":/imgs/open.png"), tr("&Open..."), this);
    open_act->setShortcut(tr("Ctrl+O"));
    open_act->setStatusTip(tr("Open an existing file"));
    connect(open_act, SIGNAL(triggered()), this, SLOT(Open()));

    save_act = new QAction(QIcon(":/imgs/save.png"), tr("&Save"), this);
    save_act->setShortcut(tr("Ctrl+S"));
    save_act->setStatusTip(tr("Save the document to disk"));
    connect(save_act, SIGNAL(triggered()), this, SLOT(Save()));

    save_as_act = new QAction(QIcon(":/imgs/save-as.png"), tr("Save &As..."), this);
    save_as_act->setShortcut(tr("Ctrl+Shift+S"));
    save_as_act->setStatusTip(tr("Save the document under a new name"));
    connect(save_as_act, SIGNAL(triggered()), this, SLOT(SaveAs()));

    exit_act = new QAction(QIcon(":/imgs/exit.png"), tr("E&xit"), this);
    exit_act->setShortcut(tr("Ctrl+W"));
    exit_act->setStatusTip(tr("Exit the application"));
    connect(exit_act, SIGNAL(triggered()), this, SLOT(close()));

    cut_act = new QAction(QIcon(":/imgs/cut.png"), tr("Cu&t"), this);
    cut_act->setShortcut(tr("Ctrl+X"));
    cut_act->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cut_act, SIGNAL(triggered()), main_editor, SLOT(cut()));

    copy_act = new QAction(QIcon(":/imgs/copy.png"), tr("&Copy"), this);
    copy_act->setShortcut(tr("Ctrl+C"));
    copy_act->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copy_act, SIGNAL(triggered()), main_editor, SLOT(copy()));

    paste_act = new QAction(QIcon(":/imgs/paste.png"), tr("&Paste"), this);
    paste_act->setShortcut(tr("Ctrl+V"));
    paste_act->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(paste_act, SIGNAL(triggered()), main_editor, SLOT(paste()));

    undo_act = new QAction(QIcon(":/imgs/undo.png"), tr("&Undo"), this);
    undo_act->setShortcut(tr("Ctrl+Z"));
    undo_act->setStatusTip(tr("Withdrawn last action"));
    connect(undo_act, SIGNAL(triggered()), main_editor, SLOT(undo()));

    redo_act = new QAction(QIcon(":/imgs/redo.png"), tr("&Rndo"), this);
    redo_act->setShortcut(tr("Ctrl+Shift+Z"));
    redo_act->setStatusTip(tr("Redo the action that has been withdrawn"));
    connect(redo_act, SIGNAL(triggered()), main_editor, SLOT(redo()));


    about_act = new QAction(QIcon(":/imgs/about.png"), tr("&About"), this);
    about_act->setStatusTip(tr("Show the application's About box"));
    connect(about_act, SIGNAL(triggered()), this, SLOT(About()));

    cut_act->setEnabled(false);
    copy_act->setEnabled(false);
    connect(main_editor, SIGNAL(copyAvailable(bool)),
            cut_act, SLOT(setEnabled(bool)));
    connect(main_editor, SIGNAL(copyAvailable(bool)),
            copy_act, SLOT(setEnabled(bool)));
}

void Textino::CreateMenus()
{
    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(new_act);
    file_menu->addAction(open_act);
    file_menu->addAction(save_act);
    file_menu->addAction(save_as_act);
    file_menu->addSeparator();
    file_menu->addAction(exit_act);

    edit_menu = menuBar()->addMenu(tr("&Edit"));
    edit_menu->addAction(cut_act);
    edit_menu->addAction(copy_act);
    edit_menu->addAction(paste_act);
    edit_menu->addAction(undo_act);
    edit_menu->addAction(redo_act);

    menuBar()->addSeparator();

    help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(about_act);

}

void Textino::CreateToolBars()
{
    file_tool_bar = addToolBar(tr("File"));
    file_tool_bar->addAction(new_act);
    file_tool_bar->addAction(open_act);
    file_tool_bar->addAction(save_act);
    file_tool_bar->addAction(save_as_act);

    edit_tool_bar = addToolBar(tr("Edit"));
    edit_tool_bar->addAction(cut_act);
    edit_tool_bar->addAction(copy_act);
    edit_tool_bar->addAction(paste_act);
    edit_tool_bar->addAction(undo_act);
    edit_tool_bar->addAction(redo_act);

    help_tool_bar = addToolBar(tr("Help"));
    help_tool_bar->addAction(about_act);
}

void Textino::CreateStatusBar()
{
    QStatusBar* status_bar = statusBar();
    status_bar->showMessage(tr("Ready"));

    status_label = new QLabel("Ready");
    status_cursor_label = new QLabel("Ready");
    status_modification_label = new QLabel("Ready");

    status_modification_label->setAlignment(Qt::AlignCenter);
    OnModificationChanged();
    status_bar->addPermanentWidget(status_modification_label);

    status_cursor_label->setMinimumWidth(150);
    status_cursor_label->setAlignment(Qt::AlignCenter);
    OnCursorPositionChanged();
    status_bar->addPermanentWidget(status_cursor_label);

    status_bar->addPermanentWidget(new QLabel());
    status_label->setMinimumWidth(150);
    status_label->setAlignment(Qt::AlignCenter);
    OnTextChanged();
    status_bar->addPermanentWidget(status_label);

    connect(main_editor, SIGNAL(copyAvailable(bool)), this, SLOT(OnSelected()));
    connect(main_editor, SIGNAL(textChanged()), this, SLOT(OnTextChanged()));
    connect(main_editor, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(OnCursorPositionChanged()));
    connect(main_editor, SIGNAL(modificationChanged(bool)), this, SLOT(OnModificationChanged()));
}

bool Textino::MaybeSave()
{
    if (main_editor->isModified()) {
        int ret = QMessageBox::warning(this, tr("Textino"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return Save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void Textino::LoadFile(const QString &file_name)
{
    QFile file(file_name);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Textino"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file_name)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    main_editor->setText(in.readAll());
    QApplication::restoreOverrideCursor();

    SetCurrentFile(file_name);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool Textino::SaveFile(const QString &file_name)
{
    QFile file(file_name);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Textino"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file_name)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << main_editor->text();
    QApplication::restoreOverrideCursor();

    SetCurrentFile(file_name);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void Textino::SetCurrentFile(const QString &given_path)
{
    current_path = given_path;
    main_editor->setModified(false);
    setWindowModified(false);

    QString shown_name;
    if (current_path.isEmpty())
        shown_name = "Untitled.txt";
    else
        shown_name = GetFileName(current_path);
    CreateLexer();
    setWindowTitle(tr("%1[*] - %2").arg(shown_name).arg(tr("Textino")));
}

QString Textino::GetFileName(const QString &current_path) {
    return QFileInfo(current_path).fileName();
}

void Textino::CreateLexer(){
    QString ext = QFileInfo(current_path).suffix();

    if(ext == "c" || ext == "cpp" || ext == "cc" || ext == "h" || ext=="hpp" || ext =="hh"){
        text_lexer = new QsciLexerCPP;
        text_lexer->setColor(QColor(Qt:: gray),QsciLexerCPP::CommentLine);
        main_editor->setAutoIndent(true);
        main_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);

    }
    else if(ext == "java") {
        text_lexer = new QsciLexerJava;
        text_lexer->setColor(QColor(Qt:: gray),QsciLexerJava::CommentLine);
        main_editor->setAutoIndent(true);
        main_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    }
    else if(ext =="py") {
        text_lexer = new QsciLexerPython;
        text_lexer->setColor(QColor(Qt:: gray),QsciLexerPython::Comment);
        main_editor->setAutoIndent(true);
        main_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    }
    else if(ext =="js" || ext =="ts") {
        text_lexer = new QsciLexerJavaScript;
        text_lexer->setColor(QColor(Qt:: gray),QsciLexerJavaScript::CommentLine);
        main_editor->setAutoIndent(true);
        main_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    }
    else if(ext =="v" || ext =="vhdl") {
        text_lexer = new QsciLexerVerilog;
        text_lexer->setColor(QColor(Qt:: gray), QsciLexerVerilog::CommentLine);
        main_editor->setAutoIndent(true);
        main_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    }
    else if(ext =="sql") {
        text_lexer = new QsciLexerSQL;
        text_lexer->setColor(QColor(Qt:: gray), QsciLexerSQL::CommentLine);
        main_editor->setAutoIndent(true);
        main_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    }
    else {
        text_lexer = nullptr;
        main_editor->setAutoIndent(false);
        main_editor->setBraceMatching(QsciScintilla::NoBraceMatch);
    }

    if(text_lexer)
        text_lexer->setFont(QFont(config->GetFontFamily(), config->GetFontSize()));

    main_editor->setLexer(text_lexer);
    main_editor->setAutoCompletionSource(QsciScintilla::AcsAll);
    main_editor->setAutoCompletionCaseSensitivity(true);
    main_editor->setAutoCompletionThreshold(1);
}