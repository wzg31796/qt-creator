/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
**************************************************************************/

#ifndef CDBSTACKTRACECONTEXT_H
#define CDBSTACKTRACECONTEXT_H

#include "stackhandler.h"

#include <windows.h>
#include <inc/dbgeng.h>

#include <QtCore/QString>
#include <QtCore/QVector>

namespace Debugger {
namespace Internal {

class CdbSymbolGroupContext;

/* Context representing a break point stack consisting of several frames.
 * Maintains an on-demand constructed list of CdbSymbolGroupContext
 * containining the local variables of the stack. */

class CdbStackTraceContext        
{
    Q_DISABLE_COPY(CdbStackTraceContext)

    explicit CdbStackTraceContext(IDebugSystemObjects4* pDebugSystemObjects,
                                  IDebugSymbols3* pDebugSymbols);
public:
    enum { maxFrames = 100 };

    ~CdbStackTraceContext();
    static CdbStackTraceContext *create(IDebugControl4* pDebugControl,
                                        IDebugSystemObjects4* pDebugSystemObjects,
                                        IDebugSymbols3* pDebugSymbols,
                                        unsigned long threadid,
                                        QString *errorMessage);

    QList<StackFrame> frames() const { return m_frames; }
    inline int frameCount() const { return m_frames.size(); }

    CdbSymbolGroupContext *symbolGroupContextAt(int index, QString *errorMessage);

private:
    bool init(unsigned long frameCount, QString *errorMessage);
    IDebugSymbolGroup2 *createSymbolGroup(int index, QString *errorMessage);

    IDebugSystemObjects4*   m_pDebugSystemObjects;
    IDebugSymbols3*         m_pDebugSymbols;

    DEBUG_STACK_FRAME m_cdbFrames[maxFrames];
    QVector <CdbSymbolGroupContext*> m_symbolContexts;
    QList<StackFrame> m_frames;
};

}
}

#endif // CDBSTACKTRACECONTEXT_H
