/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "clangtool.h"
#include "findusrforcursoraction.h"
#include "symbollocationfinderaction.h"
#include "sourcefilecallbacks.h"

#include <sourcelocationscontainer.h>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "clang/Tooling/Refactoring.h"

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

namespace ClangBackEnd {

class SymbolFinder : public ClangTool
{
public:
    SymbolFinder(uint line, uint column);

    void findSymbol();

    Utils::SmallString takeSymbolName();
    const std::vector<USRName> &unifiedSymbolResolutions();
    const SourceLocationsContainer &sourceLocations() const;
    SourceLocationsContainer takeSourceLocations();

private:
    Utils::SmallString symbolName;
    USRFindingAction usrFindingAction;
    SymbolLocationFinderAction symbolLocationFinderAction;
    SourceFileCallbacks sourceFileCallbacks;

    ClangBackEnd::SourceLocationsContainer sourceLocations_;
};

} // namespace ClangBackEnd
