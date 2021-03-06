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

#include "clangrefactoringplugin.h"

#include <cpptools/cppmodelmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/find/searchresultwindow.h>
#include <extensionsystem/pluginmanager.h>

#include <utils/hostosinfo.h>

namespace ClangRefactoring {

namespace {

QString backendProcessPath()
{
    return Core::ICore::libexecPath()
            + QStringLiteral("/clangrefactoringbackend")
            + QStringLiteral(QTC_HOST_EXE_SUFFIX);
}

} // anonymous namespace

std::unique_ptr<RefactoringClient> ClangRefactoringPlugin::refactoringClient;
std::unique_ptr<ClangBackEnd::RefactoringConnectionClient> ClangRefactoringPlugin::connectionClient;
std::unique_ptr<RefactoringEngine> ClangRefactoringPlugin::engine;
std::unique_ptr<QtCreatorSearch> ClangRefactoringPlugin::qtCreatorSearch;
std::unique_ptr<QtCreatorClangQueryFindFilter> ClangRefactoringPlugin::qtCreatorfindFilter;

bool ClangRefactoringPlugin::initialize(const QStringList & /*arguments*/, QString * /*errorMessage*/)
{
    refactoringClient.reset(new RefactoringClient);
    connectionClient.reset(new ClangBackEnd::RefactoringConnectionClient(refactoringClient.get()));
    engine.reset(new RefactoringEngine(connectionClient->serverProxy(), *refactoringClient));
    qtCreatorSearch.reset(new ClangRefactoring::QtCreatorSearch(*Core::SearchResultWindow::instance()));
    qtCreatorfindFilter.reset(new QtCreatorClangQueryFindFilter(connectionClient->serverProxy(),
                                                                *qtCreatorSearch.get(),
                                                                *refactoringClient));

    refactoringClient->setRefactoringEngine(engine.get());
    ExtensionSystem::PluginManager::addObject(qtCreatorfindFilter.get());

    connectBackend();
    startBackend();

    return true;
}

void ClangRefactoringPlugin::extensionsInitialized()
{
    CppTools::CppModelManager::setRefactoringEngine(&refactoringEngine());
}

ExtensionSystem::IPlugin::ShutdownFlag ClangRefactoringPlugin::aboutToShutdown()
{
    ExtensionSystem::PluginManager::removeObject(qtCreatorfindFilter.get());
    refactoringClient->setRefactoringEngine(nullptr);

    connectionClient->finishProcess();

    qtCreatorfindFilter.reset();
    engine.reset();
    connectionClient.reset();
    refactoringClient.reset();

    return SynchronousShutdown;
}

RefactoringEngine &ClangRefactoringPlugin::refactoringEngine()
{
    return *engine;
}

void ClangRefactoringPlugin::startBackend()
{
    connectionClient->setProcessPath(backendProcessPath());

    connectionClient->startProcessAndConnectToServerAsynchronously();
}

void ClangRefactoringPlugin::connectBackend()
{
    connect(connectionClient.get(),
            &ClangBackEnd::RefactoringConnectionClient::connectedToLocalSocket,
            this,
            &ClangRefactoringPlugin::backendIsConnected);
}

void ClangRefactoringPlugin::backendIsConnected()
{
    engine->setUsable(true);
}

} // namespace ClangRefactoring
